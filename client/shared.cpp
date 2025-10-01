#include "shared.hpp"

#include <string>
#include <windows.h>

#include <TlHelp32.h>

namespace shared
{
    std::wstring getCurrentAppFolder()
    {
        wchar_t buffer[1024];
        GetModuleFileNameW(NULL, buffer, 1024);
        std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
        return std::wstring(buffer).substr(0, pos);
    }

    int wcharToChar(int wideChar, char* charArray)
    {
        // UTF-8 encoding conversion logic
        if (wideChar <= 0x7F) {
            // 1-byte character (ASCII)
            charArray[0] = static_cast<char>(wideChar);
            return 1;
        }
        if (wideChar <= 0x7FF) {
            // 2-byte character
            charArray[0] = static_cast<char>(0xC0 | (wideChar >> 6));
            charArray[1] = static_cast<char>(0x80 | (wideChar & 0x3F));
            return 2;
        }
        if (wideChar <= 0xFFFF) {
            // 3-byte character
            charArray[0] = static_cast<char>(0xE0 | (wideChar >> 12));
            charArray[1] = static_cast<char>(0x80 | ((wideChar >> 6) & 0x3F));
            charArray[2] = static_cast<char>(0x80 | (wideChar & 0x3F));
            return 3;
        }
        if (wideChar <= 0x10FFFF) {
            // 4-byte character
            charArray[0] = static_cast<char>(0xF0 | (wideChar >> 18));
            charArray[1] = static_cast<char>(0x80 | ((wideChar >> 12) & 0x3F));
            charArray[2] = static_cast<char>(0x80 | ((wideChar >> 6) & 0x3F));
            charArray[3] = static_cast<char>(0x80 | (wideChar & 0x3F));
            return 4;
        }

        return 0;
    }

    size_t wcharArrayToCharArray(const wchar_t* arr, char* out)
    {
        size_t index = 0;
        size_t i = 0;

        while (arr[i] != L'\0')
        {
            char utf8[4];
            size_t utf8_len = wcharToChar(arr[i], utf8);

            for (size_t j = 0; j < utf8_len; ++j)
            {
                out[index++] = utf8[j];
            }

            ++i;
        }

        out[index] = '\0';
        return index;
    }

    void charArrayToWcharArray(const char* in, wchar_t* out)
    {
        const char* p_in = in;
        wchar_t* p_out = out;

        while (*p_in)
        {
            uint32_t codepoint = 0;
            unsigned char c = static_cast<unsigned char>(*p_in);

            if (c <= 0x7F)
            {
                codepoint = c;
                ++p_in;
            }
            else if ((c & 0xE0) == 0xC0)
            {
                if ((p_in[1] & 0xC0) != 0x80)
                    return;

                codepoint = ((c & 0x1F) << 6) | (p_in[1] & 0x3F);
                p_in += 2;
            }
            else if ((c & 0xF0) == 0xE0)
            {
                if ((p_in[1] & 0xC0) != 0x80 || (p_in[2] & 0xC0) != 0x80)
                    return;

                codepoint = ((c & 0x0F) << 12) | ((p_in[1] & 0x3F) << 6) | (p_in[2] & 0x3F);
                p_in += 3;
            }
            else if ((c & 0xF8) == 0xF0)
            {
                if ((p_in[1] & 0xC0) != 0x80 || (p_in[2] & 0xC0) != 0x80 || (p_in[3] & 0xC0) != 0x80)
                    return;

                codepoint = ((c & 0x07) << 18) | ((p_in[1] & 0x3F) << 12) | ((p_in[2] & 0x3F) << 6) | (p_in[3] & 0x3F);
                p_in += 4;
            }
            else
            {
                return;
            }

            *p_out++ = static_cast<wchar_t>(codepoint);
        }

        *p_out = L'\0';
    }

    HANDLE getProcessIdByName(hash_t processHash)
    {
        DWORD pid = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (snapshot != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32W processEntry;
            processEntry.dwSize = sizeof(processEntry);

            if (Process32FirstW(snapshot, &processEntry))
            {
                char buffer[256];

                do
                {
                    wcharArrayToCharArray(processEntry.szExeFile, buffer);

                    if (HASH_RT(buffer) == processHash)
                    {
                        pid = processEntry.th32ProcessID;
                        break;
                    }
                } while (Process32NextW(snapshot, &processEntry));
            }
            CloseHandle(snapshot);
        }
        return reinterpret_cast<HANDLE>(pid);
    }
}
