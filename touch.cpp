#include<iostream>
#include<filesystem>
#include"styles.h"
#include<string>
#include<fstream>

namespace fs = std::filesystem;
namespace tc = color;


void touch(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<tc::BRIGHT_RED<<"touch Command missing argument!\n"<<tc::RESET;
            else 
            {
                fs::path newpath{ CurrentPath / argument};
                if(fs::exists(newpath)) std::cout<<tc::BRIGHT_RED<<"File already exists!\n"<<tc::RESET;
                else
                {
                    std::ofstream file(newpath.string());
                    if(file) std::cout<<tc::BRIGHT_GREEN<<"File Created!\n"<<tc::RESET;
                    else std::cout<<tc::BRIGHT_RED<<"File Failed to create!\n"<<tc::RESET;
                }
            }
}