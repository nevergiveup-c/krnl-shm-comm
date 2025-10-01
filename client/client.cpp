#include "driverClient.h"

#include <filesystem>
#include <iostream>
#include <vector>

#include "allocateMemoryCommand.h"
#include "freeMemoryCommand.h"
#include "readMemoryCommand.h"
#include "writeMemoryCommand.h"

#include <kdmapper.hpp>
#include <intel_driver.hpp>

#include "symbolParser.h"

bool loadDriver()
{
    std::cout << "Loading driver" << '\n';

    std::wstring driverPath = L"driver.sys";

    if (!std::filesystem::exists(driverPath))
    {
        return false;
    }

    auto const pid = GetCurrentProcessId();
    auto const data = SymbolParser::instance().parse();

    auto const iqvw64e_device_handle = intel_driver::Load();

    if (!iqvw64e_device_handle)
    {
        return false;
    }

    std::vector<uint8_t> raw_image{ 0 };

    if (!utils::ReadFileToMemory(driverPath, &raw_image))
    {
        Log(L"[-] Failed to read image to memory" << std::endl);
        intel_driver::Unload();
        return false;
    }

    NTSTATUS exitCode = 0;

    if (!kdmapper::MapDriver(raw_image.data(), pid, reinterpret_cast<ULONG64>(data.data()), false,
        false, kdmapper::AllocationMode::AllocateIndependentPages, false, nullptr, &exitCode))
    {
        Log(L"[-] Failed to map " << driverPath.data() << std::endl);
        intel_driver::Unload();
        return false;
    }

    if (!intel_driver::Unload())
    {
        Log(L"[-] Warning failed to fully unload vulnerable driver " << std::endl);
    }

    Log(L"[+] success" << std::endl);

    return true;
}

int main()
{
    if (!ClientConnection::isDriverLoaded())
    {
        if (!loadDriver())
        {
            std::cout << "Failed to load driver\n";
            std::cin.get();
            return -1;
        }
        Sleep(1000);
    }

    DriverClient client;

    if (!client.connect())
    {
        std::cout << "Failed to connect\n";
        return -1;
    }

    std::cout << "[+] Connected to driver\n\n";

    auto pid = GetCurrentProcessId();

    PVOID addr = nullptr;
    client.command<AllocateMemoryCommand>(pid, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE, &addr);
    std::cout << "Allocated at: 0x" << addr << "\n";

    int data = 0xDEADBEEF;
    client.command<WriteMemoryCommand>(pid, addr, &data, sizeof(int));

    int readBack = 0;
    client.command<ReadMemoryCommand>(pid, addr, &readBack, sizeof(int));
    std::cout << "Written: 0xDEADBEEF, Read: 0x" << std::hex << readBack << "\n";

    client.command<FreeMemoryCommand>(pid, addr, 0, MEM_RELEASE);

    std::cout << "\nPress Enter to shutdown...";
    std::cin.ignore();
    std::cin.get();

    client.disconnect();
    return 0;
}
