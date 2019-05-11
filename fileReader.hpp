#pragma once

#include <string>

namespace FileUtils
{
    class FileReader
    {
    public:
        static std::string read_file(const std::string &filename);
    };
}