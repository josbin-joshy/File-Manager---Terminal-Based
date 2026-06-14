#include<iostream>
#include<filesystem>
#include<string>
#include<fstream>

namespace fs = std::filesystem;


void touch(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<"touch Command missing argument!\n";
            else 
            {
                fs::path newpath{ CurrentPath / argument};
                if(fs::exists(newpath)) std::cout<<"File already exists!\n";
                else
                {
                    std::ofstream file(newpath.string());
                    if(file) std::cout<<"File Created!\n";
                    else std::cout<<"File Failed to create!\n";
                }
            }
}