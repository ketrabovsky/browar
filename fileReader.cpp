#include "fileReader.hpp"

#include <fstream>

namespace FileUtils
{
    std::string FileReader::read_file(const std::string &filename)
    {
        std::fstream file;
        file.open(filename, std::fstream::in);

        if (!file.is_open()) 
        {
            return "";
        }

        std::string file_lines;

        while(!file.eof()) {
            std::string line;
            std::getline(file, line);
            if (line.size() < 1) continue;
            file_lines.append(line.c_str());
        }

        file.close();

        return file_lines;
    }
}
