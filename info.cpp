#include<iostream>
#include<filesystem>
#include<string>

namespace fs = std::filesystem;


void info(fs::path& CurrentPath, const std::string& argument)
{
    if(argument.empty()) std::cout<<"Missing argument for info"<<std::endl;
            else
            {
                fs::path target{ CurrentPath / argument};
                if(!fs::exists(target)) std::cout<<"Target not Found!"<<std::endl;
                else
                {
                    std::cout<<"NAME: "<<target.filename().string()<<"\n";
                    if(fs::is_directory(target)) std::cout<<"DIRECTORY: "<<std::endl;
                    else std::cout<<"FILE"<<std::endl;
                    std::cout<<"SIZE: "<<fs::file_size(target)<<"bytes\n";
                }
            }
}