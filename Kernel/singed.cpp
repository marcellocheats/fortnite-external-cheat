#include <Windows.h>
#include <string>

struct CommandResult {
  int exitCode;
  std::string output;
};

CommandResult system_with_output(const std::string& command) {
  // Create pipes for the child process's STDOUT and STDERR.
  HANDLE stdout_read_handle, stdout_write_handle;
  HANDLE stderr_read_handle, stderr_write_handle;
  if (!CreatePipe(&stdout_read_handle, &stdout_write_handle, NULL, 0)) {
    throw std::runtime_error("Error creating pipe for STDOUT");
  }
  if (!CreatePipe(&stderr_read_handle, &stderr_write_handle, NULL, 0)) {
    CloseHandle(stdout_read_handle);
    CloseHandle(stdout_write_handle);
    throw std::runtime_error("Error creating pipe for STDERR");
  }

  // Create the child process.
  STARTUPINFO startup_info;
  PROCESS_INFORMATION process_info;
  ZeroMemory(&startup_info, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);
  startup_info.dwFlags |= STARTF_USESTDHANDLES;
  startup_info.hStdOutput = stdout_write_handle;
  startup_info.hStdError = stderr_write_handle;
  if (!CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &startup_info, &process_info)) {
    CloseHandle(stdout_read_handle);
    CloseHandle(stdout_write_handle);
    CloseHandle(stderr_read_handle);
    CloseHandle(stderr_write_handle);
    throw std::runtime_error("Error creating child process");
  }

  // Close the write ends of the pipes.
  CloseHandle(stdout_write_handle);
  CloseHandle(stderr_write_handle);

  // Read the child process's output from the pipes.
  DWORD read_size;
  const DWORD buffer_size = 4096;
  char buffer[buffer_size];
  std::stringstream output_stream;
  for (;;) {
    if (!ReadFile(stdout_read_handle, buffer, buffer_size, &read_size, NULL) || read_size == 0) {
      break;
    }
    output_stream.write(buffer, read_size);
  }
  for (;;) {
    if (!ReadFile(stderr_read_handle, buffer, buffer_size, &read_size, NULL) || read_size == 0) {
      break;
    }
    output_stream.write(buffer, read_size);
  }

  // Close the read ends of the pipes.
  CloseHandle(stdout_read_handle);
  CloseHandle(stderr_read_handle);

  // Wait for the child process to exit and get its exit code.
  DWORD exit_code;
  if (WaitForSingleObject(process_info.hProcess, INFINITE) != WAIT_OBJECT_0 || !GetExitCodeProcess(process_info.hProcess, &exit_code)) {
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    throw std::runtime_error("Error waiting for child process to exit");
  }

  // Close the process and thread handles.
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);

  // Return the command result.
  return { static_cast<int>(exit_code), output_stream.str() };
}

void ExecuteCommand(const std::wstring& command, HANDLE stdout_write_handle, HANDLE stderr_write_handle, std::string &stdout_output, std::string &stderr_output, DWORD &exit_code) {
  // Create a new process to execute the command.
  STARTUPINFOW startup_info = {0};
  startup_info.cb = sizeof(STARTUPINFO);
  startup_info.hStdError = stderr_write_handle;
  startup_info.hStdOutput = stdout_write_handle;
  startup_info.hStdInput = NULL;
  startup_info.dwFlags |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION process_info = {0};
  std::wstring cmd = L"/C " + command;
  if (!CreateProcessW(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info)) {
    DWORD error = GetLastError();
    throw std::runtime_error("Error creating process: " + std::to_string(error));
  }

  // Close the write ends of the pipes.
  CloseHandle(stdout_write_handle);
  CloseHandle(stderr_write_handle);

  // Read the output from the child process's pipes.
  for (;;) {
    char buffer[1024];
    DWORD bytes_read;
    if (!ReadFile(stdout_read_handle, buffer, sizeof(buffer), &bytes_read, NULL) || bytes_read == 0) {
      break;
    }
    stdout_output.append(buffer, bytes_read);
  }
  for (;;) {
    char buffer[1024];
    DWORD bytes_read;
    if (!ReadFile(stderr_read_handle, buffer, sizeof(buffer), &bytes_read, NULL) || bytes_read == 0) {
      break;
    }
    stderr_output.append(buffer, bytes_read);
  }

  // Wait for the child process to finish and retrieve its exit code.
  WaitForSingleObject(process_info.hProcess, INFINITE);
  if (!GetExitCodeProcess(process_info.hProcess, &exit_code)) {
    throw std::runtime_error("Error getting exit code");
  }

  // Close the process and thread handles.
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);
}

void InitializeConsole
{
    if (!AllocConsole())
    {
        throw std::runtime_error("Error allocating console window.");
    }
    std::freopen("CONIN$", "r", stdin);
    std::freopen("CONOUT$", "w", stdout);
    return true;
}

// Get the module handle for the VALORANT game process
HMODULE GetGameModuleHandle()
{
    HMODULE module = GetModuleHandleA(nullptr);
    if (!module)
    {
        throw std::runtime_error("Error getting module handle for VALORANT game process.");
    }
    return module;
}

// Decrypt the address for the UWorld class
uintptr_t DecryptUWorldAddress(HMODULE module, uintptr_t key, State* state)
{
    uintptr_t moduleBaseAddress = reinterpret_cast<uintptr_t>(module);
    uintptr_t UWorldAddress = Decryption::Decrypt_UWorld(key, reinterpret_cast<uintptr_t*>(state));
    if (!UWorldAddress)
    {
        throw std::runtime_error("Error decrypting UWorld class address.");
    }
    return UWorldAddress;
}

// Read the UWorld class instance
std::unique_ptr<UWorld> GetUWorldInstance(uintptr_t uworldAddress)
{
    std::unique_ptr<UWorld> uworld = Memory::ReadStub<UWorld*>(uworldAddress);
    if (!uworld)
    {
        throw std::runtime_error("Error reading UWorld class instance.");
    }
    return uworld;
}

// Read the ULocalPlayer instance
std::unique_ptr<ULocalPlayer> GetLocalPlayerInstance(UWorld* uworld)
{
    UGameInstance* gameInstance = Memory::ReadStub<UGameInstance*>(uworld + Offsets::GameInstance);
    if (!gameInstance)
    {
        throw std::runtime_error("Error reading UGameInstance.");
    }
    std::unique_ptr<ULocalPlayer> localPlayer = Memory::ReadStub<ULocalPlayer*>(gameInstance + Offsets::LocalPlayer);
    if (!localPlayer)
    {
        throw std::runtime_error("Error reading ULocalPlayer.");
    }
    return localPlayer;
}

void hookPostRenderMethod(uintptr_t localPlayer, uintptr_t* pOriginalRender) {
    uintptr_t* vmt = *(uintptr_t**)(localPlayer + 0x78);
    *pOriginalRender = vmt[0x68];
    Hook::VMT(vmt, PostRender, 0x68, (void*)pOriginalRender);
  {
     return;
}
  

