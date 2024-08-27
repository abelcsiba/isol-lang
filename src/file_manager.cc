
#include <fstream>
#include <streambuf>

#include "file_manager.hh"

int FileManager::loadFile(CodeFile &file, const char* full_path)
{

    std::ifstream source(full_path, std::ios::binary);

    if (source.is_open())
    {
        source.seekg(0, std::ios::end);  
        auto length = source.tellg();
        file.code = new char[length];
        source.seekg(0, std::ios::beg);

        source.read(file.code, length);
    }
    else
    {
        return -1;
    }

    return 0;
}