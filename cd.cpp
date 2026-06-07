#include<iostream>
#include<filesystem>
#include"styles.h"
#include<string>

namespace fs = std::filesystem;
namespace tc = color;


void cd(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty())  std::cout<<tc::BRIGHT_RED<<"cd missing argument!\n"<<tc::RESET;

            else
            {
                if(argument=="..") CurrentPath = CurrentPath.parent_path();
                else
                {
                    fs::path newpath{CurrentPath / argument};

                    //changing or moving to the new directory
                    if(fs::exists(newpath) && fs::is_directory(newpath))  CurrentPath = fs::canonical(newpath);
                    
                    //if the newpath doesn't exist or isnt a directory
                    else std::cout<<tc::BRIGHT_RED<<"Invalid Argument!\n ***Either argument is not directory or the directory does not exist***\n"<<tc::RESET;
                }
            }
}