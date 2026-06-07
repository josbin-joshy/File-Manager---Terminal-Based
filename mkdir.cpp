#include<iostream>
#include<filesystem>
#include"styles.h"
#include<string>

namespace fs = std::filesystem;
namespace tc = color;

void mkdir(const fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<tc::BRIGHT_RED<<"mkdir Command requires argument!\n"<<tc::RESET;
            else
            {
                fs::path newpath{CurrentPath / argument};
                if(fs::exists(newpath))
                {
                    std::cout<<tc::BRIGHT_RED<<"Directory already exists!\n"<<tc::RESET;
                }
                else
                {
                    if(fs::create_directory(newpath)) std::cout<<tc::BRIGHT_GREEN<<"Directory Created!\n"<<tc::RESET;
                    else std::cout<<tc::BRIGHT_RED<<"Directory Failed to Create!"<<tc::RESET<<std::endl;
                }
            }
}