#include "file_utils.hpp"


bool file_utils::create_file_from_buffer(const std::string_view file_path, const void* buffer, size_t size) {
    try {
        std::ofstream stream(file_path.data(), std::ios::binary | std::ios::trunc);
        if (!stream) {
            log::error("Failed to open file {} for writing.", file_path);
            return false;
        }

        stream.write(reinterpret_cast<const char*>(buffer), size);

        if (stream.bad()) {
            log::error("Failed to write data to file {} due to a bad stream state.", file_path);
            return false;
        }

        if (stream.fail()) {
            log::error("Failed to write data to file {} due to a write failure.", file_path);
            return false;
        }

        stream.close();
    } catch (const std::ofstream::failure& e) {
        log::error("Failed to write data to file {}: {}", file_path, e.what());
        return false;
    }

    return true;
}
