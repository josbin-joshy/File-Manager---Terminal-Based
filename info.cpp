#include<iostream>
#include<filesystem>
#include"styles.h"
#include<string>

namespace fs = std::filesystem;
namespace tc = color;


void info(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<tc::BRIGHT_RED<<"Missing argument for info"<<tc::RESET<<std::endl;
            else
            {
                fs::path target{ CurrentPath / argument};
                if(!fs::exists(target)) std::cout<<tc::BRIGHT_RED<<"Target not Found!"<<tc::RESET<<std::endl;
                else
                {
                    std::cout<<tc::BRIGHT_CYAN<<"NAME: "<<target.filename().string()<<"\n";
                    if(fs::is_directory(target)) std::cout<<tc::BRIGHT_YELLOW<<"DIRECTORY: "<<std::endl;
                    else std::cout<<tc::BRIGHT_MAGENTA<<"FILE"<<std::endl;
                    std::cout<<tc::BRIGHT_BLUE<<"SIZE: "<<fs::file_size(target)<<"bytes\n"<<tc::RESET;
                }
            }
}