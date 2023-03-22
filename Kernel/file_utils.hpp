#pragma once
#include <string>
#include <fstream>

namespace file_utils
{
    bool create_file_from_buffer(const std::string& file_path, const char* buffer, std::streamsize size);
}
