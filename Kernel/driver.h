#include "std_include.hpp"
#include "logging.hpp"
#include "sleep_callback.hpp"
#include "irp.hpp"
#include "exception.hpp"
#include "hypervisor.hpp"

#define DOS_DEV_NAME L"\\DosDevices\\HelloDev"
#define DEV_NAME L"\\Device\\HelloDev"

class GlobalDriver
{
public:
    // Constants for device names
    static constexpr const char* DEV_NAME = "MyDevice";
    static constexpr const char* DOS_DEV_NAME = "\\DosDevices\\MyDevice";

    // Constructor initializes member objects
    GlobalDriver(const PDRIVER_OBJECT driverObject)
        : irp_(driverObject, DEV_NAME, DOS_DEV_NAME),
          hypervisor_()
    {
        // Initialize the sleep callback
        sleepCallback_ = [this](const SleepCallback::Type type)
        {
            sleepNotification(type);
        };

        // Output message to debug log
        Log::info("Driver started");

        // Initialize the hypervisor
        if (!hypervisor_.initialize())
        {
            Log::error("Failed to initialize hypervisor");
            // Add error handling code here
        }
    }

    // Destructor cleans up member objects
    ~GlobalDriver()
    {
        // Output message to debug log
        Log::info("Unloading driver");

        // Disable all EPT hooks
        hypervisor_.disableAllEptHooks();

        // Cleanup the hypervisor
        if (!hypervisor_.cleanup())
        {
            Log::error("Failed to cleanup hypervisor");
            // Add error handling code here
        }
    }

    // Disallow copy and move construction/assignment
    GlobalDriver(const GlobalDriver&) = delete;
    GlobalDriver& operator=(const GlobalDriver&) = delete;
    GlobalDriver(GlobalDriver&&) = delete;
    GlobalDriver& operator=(GlobalDriver&&) = delete;

private:
    // Member objects
    IRP irp_;
    Hypervisor hypervisor_;
    SleepCallback sleepCallback_;

    // Private method for handling sleep notifications
    void sleepNotification(const SleepCallback::Type type)
    {
        // Add sleep notification handling code here
    }
};


class DriverClass {
public:
    DriverClass() = default;
    ~DriverClass() = default;

    void preDestroy(PDRIVER_OBJECT driverObject)
    {
        UNREFERENCED_PARAMETER(driverObject);
        // Perform any necessary cleanup before the driver is unloaded
    }

private:
    bool hypervisor_enabled_ = false;
    Hypervisor hypervisor_;
    SleepCallback sleep_callback_;
    PDEVICE_OBJECT device_object_ = nullptr;

    static void sleep_notification(const SleepCallback::Type type, void* context)
    {
        auto driver_class = reinterpret_cast<DriverClass*>(context);
        if (driver_class == nullptr) {
            return;
        }

        if (type == SleepCallback::Type::Sleep)
        {
            DebugLog("Going to sleep...\n");

            // Save the previous hypervisor state before disabling it
            driver_class->hypervisor_enabled_ = driver_class->hypervisor_.is_enabled();
            driver_class->hypervisor_.disable();
        }
        else if (type == SleepCallback::Type::Wakeup && driver_class->hypervisor_enabled_)
        {
            DebugLog("Waking up...\n");

            // Restore the previous hypervisor state
            driver_class->hypervisor_.enable();
        }
    }

static NTSTATUS dispatch(PDEVICE_OBJECT device_object, PIRP irp)
    {
        auto status = STATUS_SUCCESS;

        auto stack = IoGetCurrentIrpStackLocation(irp);
        switch (stack->MajorFunction)
        {
        case IRP_MJ_CREATE:
        case IRP_MJ_CLOSE:
            break;

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        irp->IoStatus.Status = status;
        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return status;
    }

public:
    NTSTATUS initialize(PDRIVER_OBJECT driver_object)
    {
        UNREFERENCED_PARAMETER(driver_object);

        // Create device object
        UNICODE_STRING device_name;
        RtlInitUnicodeString(&device_name, L"\\Device\\MyDevice");
        auto status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, 0, FALSE, &device_object_);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        // Create symbolic link
        UNICODE_STRING symlink_name;
        RtlInitUnicodeString(&symlink_name, L"\\DosDevices\\MyDevice");
        status = IoCreateSymbolicLink(&symlink_name, &device_name);
        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(device_object_);
            return status;
        }

        // Register sleep callback
        sleep_callback_.register_callback(&sleep_notification, this);

        // Set dispatch function for the device object
        auto device_extension = static_cast<PDEVICE_EXTENSION>(device_object_->DeviceExtension);
        device_extension->driver_class = this;
        device_object_->Flags |= DO_BUFFERED_IO;
        device_object_->Flags &= ~DO_DEVICE_INITIALIZING;
        auto driver_dispatch = device_object_->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];
        device_object_->DriverObject->MajorFunction[IRP_MJ_CREATE] = driver_dispatch;
        device_object_->DriverObject->MajorFunction[IRP_MJ_CLOSE] = driver_dispatch;
        device_object_->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &dispatch;

        return STATUS_SUCCESS;
    }


extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING)
{
    driver_object->DriverUnload = unload;
    global_driver_instance = new global_driver(driver_object);
    return STATUS_SUCCESS;
}
