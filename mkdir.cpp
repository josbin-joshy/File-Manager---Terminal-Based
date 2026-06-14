#include<iostream>
#include<filesystem>
#include<string>

namespace fs = std::filesystem;

void mkdir(const fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<"mkdir Command requires argument!\n";
            else
            {
                fs::path newpath{CurrentPath / argument};
                if(fs::exists(newpath))
                {
                    std::cout<<"Directory already exists!\n";
                }
                else
                {
                    if(fs::create_directory(newpath)) std::cout<<"Directory Created!\n";
                    else std::cout<<"Directory Failed to Create!"<<std::endl;
                }
            }
}