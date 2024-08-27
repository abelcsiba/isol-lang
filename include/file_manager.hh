
#pragma once

#include "util.hh"

class FileManager {

public:
    FileManager() = default;

    int loadFile(CodeFile &file, const char *full_path);


};

