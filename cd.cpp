#include<iostream>
#include<filesystem>
#include<string>

namespace fs = std::filesystem;


void cd(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty())  std::cout<<"cd missing argument!\n";

            else
            {
                if(argument=="..") CurrentPath = CurrentPath.parent_path();
                else
                {
                    fs::path newpath{CurrentPath / argument};

                    //changing or moving to the new directory
                    if(fs::exists(newpath) && fs::is_directory(newpath))  CurrentPath = fs::canonical(newpath);
                    
                    //if the newpath doesn't exist or isnt a directory
                    else std::cout<<"Invalid Argument!\n ***Either argument is not directory or the directory does not exist***\n";
                }
            }
}