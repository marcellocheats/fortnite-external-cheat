#include "service_utils.hpp"

class ServiceUtils {
public:
    static SC_HANDLE openScManager() {
        SC_HANDLE handle = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
        if (!handle) {
            throw std::runtime_error("Failed to open SC Manager: " + std::to_string(GetLastError()));
        }
        return handle;
    }

    static SC_HANDLE createService(const std::wstring& driverPath) {
        SC_HANDLE scManagerHandle = openScManager();

        SC_HANDLE serviceHandle = OpenService(scManagerHandle, SERVICE_NAME, SERVICE_ALL_ACCESS);
        if (serviceHandle) {
            CloseServiceHandle(scManagerHandle);
            return serviceHandle;
        }

        serviceHandle = CreateService(scManagerHandle, SERVICE_NAME, DISPLAY_NAME, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, driverPath.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);
        if (!serviceHandle) {
            CloseServiceHandle(scManagerHandle);
            throw std::runtime_error("Failed to create service: " + std::to_string(GetLastError()));
        }

        CloseServiceHandle(scManagerHandle);
        return serviceHandle;
    }
};

int main() {
    const std::wstring driverPath = L"C:\\Windows\\System32\\drivers\\vmbusraid.sys";
    SC_HANDLE serviceHandle = ServiceUtils::createService(driverPath);
    CloseServiceHandle(serviceHandle);
    return 0;
}

bool service_utils::start_service(SC_HANDLE service_handle)
{
    SERVICE_STATUS service_status;
    if (!QueryServiceStatus(service_handle, &service_status))
    {
        std::cerr << "Failed to query service status: " << GetLastError() << std::endl;
        return false;
    }

    if (service_status.dwCurrentState == SERVICE_RUNNING)
    {
        std::cout << "Service is already running" << std::endl;
        return true;
    }

    if (!StartService(service_handle, 0, nullptr))
    {
        std::cerr << "Failed to start service: " << GetLastError() << std::endl;
        return false;
    }

    std::cout << "Waiting for service to start..." << std::endl;
    while (QueryServiceStatus(service_handle, &service_status) && service_status.dwCurrentState == SERVICE_START_PENDING)
    {
        Sleep(1000);
    }

    if (service_status.dwCurrentState == SERVICE_RUNNING)
    {
        std::cout << "Service started successfully" << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to start service: " << GetLastError() << std::endl;
        return false;
    }
}


bool service_utils::stop_service(SC_HANDLE service_handle)
{
    SC_HANDLE sc_manager_handle = open_sc_manager();

    CHECK_SC_MANAGER_HANDLE(sc_manager_handle, false);

    SERVICE_STATUS service_status;

    if (!ControlService(service_handle, SERVICE_CONTROL_STOP, &service_status))
    {
        CloseServiceHandle(sc_manager_handle);
        return false;
    }

    CloseServiceHandle(sc_manager_handle);

    return true;
}

const std::string DRIVER_NAME = "driver141";
const std::string DRIVER_FILE_NAME = "cpuzdriver.sys";

class Driver {
public:
  bool Load() {
    if (StopAndDeleteService()) {
      std::cout << "Service stopped and deleted successfully\n";
    }

    if (!CreateDriverFile()) {
      std::cerr << "Failed to create driver file\n";
      return false;
    }

    if (CreateAndStartService()) {
      std::cout << "Driver loaded successfully\n";
      return true;
    }
    else {
      std::cerr << "Failed to create and start service\n";
      return false;
    }
  }

private:
  std::unique_ptr<void, decltype(&CloseHandle)> serviceHandle_ = {nullptr, &CloseHandle};

  bool StopAndDeleteService() {
    SC_HANDLE service = OpenService(nullptr, DRIVER_NAME.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
    if (!service) {
      // Service doesn't exist, so nothing to stop or delete
      return true;
    }

    SERVICE_STATUS status = {};
    if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
      std::cout << "Stopping service...\n";
      Sleep(1000); // Wait for service to stop
    }

    if (DeleteService(service)) {
      std::cout << "Service deleted successfully\n";
      CloseServiceHandle(service);
      return true;
    }
    else {
      std::cerr << "Failed to delete service: " << GetLastError() << '\n';
      CloseServiceHandle(service);
      return false;
    }
  }

  bool CreateDriverFile() {
    if (GetFileAttributesA(DRIVER_FILE_NAME.c_str()) != INVALID_FILE_ATTRIBUTES) {
      std::cout << "Driver file already exists\n";
      return true;
    }

    HANDLE file = CreateFileA(DRIVER_FILE_NAME.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
      std::cerr << "Failed to create driver file: " << GetLastError() << '\n';
      return false;
    }


