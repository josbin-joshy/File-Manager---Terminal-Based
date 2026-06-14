#include<iostream>
#include<filesystem>
#include<string>

namespace fs = std::filesystem;


void rm(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<"rm Command is missing argument!\n";
            else
            {
                fs::path newpath{ CurrentPath / argument};
                if(!fs::exists(newpath)) std::cout<<"File Not Found!\n";
                else if(fs::is_directory(newpath)) std::cout<<"Cannot remove directory. Try rmdir [dir-name]!\n";
                else 
                {
                    if(fs::remove(newpath)) std::cout<<"File removed!"<<std::endl;
                    else std::cout<<"Ffailed to Remove File!"<<std::endl;
                }
            }
}