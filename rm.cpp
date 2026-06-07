#include<iostream>
#include<filesystem>
#include"styles.h"
#include<string>

namespace fs = std::filesystem;
namespace tc = color;


void rm(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<tc::BRIGHT_RED<<"rm Command is missing argument!\n"<<tc::RESET;
            else
            {
                fs::path newpath{ CurrentPath / argument};
                if(!fs::exists(newpath)) std::cout<<tc::BRIGHT_RED<<"File Not Found!\n"<<tc::RESET;
                else if(fs::is_directory(newpath)) std::cout<<tc::BRIGHT_RED<<"Cannot remove directory. Try rmdir [dir-name]!\n"<<tc::RESET;
                else 
                {
                    if(fs::remove(newpath)) std::cout<<tc::BRIGHT_GREEN<<"File removed!"<<tc::RESET<<std::endl;
                    else std::cout<<tc::BRIGHT_RED<<"Ffailed to Remove File!"<<tc::RESET<<std::endl;
                }
            }
}