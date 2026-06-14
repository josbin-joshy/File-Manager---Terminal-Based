#include<iostream>
#include<filesystem>
#include<string>

namespace fs = std::filesystem;

void pwd(const fs::path& CurrentPath)
{
    std::cout<<"Current Directory: ";
    std::cout<<CurrentPath.string();
    std::cout<<std::endl;
}
