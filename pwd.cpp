#include<iostream>
#include<filesystem>
#include"styles.h"
#include<string>

namespace fs = std::filesystem;
namespace tc = color;

void pwd(const fs::path& CurrentPath)
{
    std::cout<<tc::BRIGHT_BLUE<<"Current Directory: ";
    std::cout<<CurrentPath.string()<<tc::RESET;
    std::cout<<std::endl;
}