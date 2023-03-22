#pragma once
#include <Windows.h>
#include <string>
#include "singed.hpp"

#define CHECK_HANDLE(x) (x && x != INVALID_HANDLE_VALUE)
#define MIN_ADDRESS ((ULONG_PTR)0x8000000000000000)

#define CHECK_SC_MANAGER_HANDLE(x, ret_type)												\
if (!CHECK_HANDLE(x))																		\
{																							\
	printf(XorStr("[!] Failed to obtain service handle \n").c_str());										\
	return ret_type;																		\
}																							\

namespace service_utils
{
    // Open a handle to the Service Control Manager
    SC_HANDLE open_sc_manager();

    // Create a new service with the specified driver path
    SC_HANDLE create_service(const std::string& driver_path, const std::string& service_name, const std::string& display_name);

    // Delete the service with the specified handle
    bool delete_service(SC_HANDLE service_handle);

    // Start the service with the specified handle
    bool start_service(SC_HANDLE service_handle);

    // Stop the service with the specified handle
    bool stop_service(SC_HANDLE service_handle);
}
