#include<iostream>
#include<string>
#include<filesystem>
#include<sstream>
#include<vector>
#include<algorithm>
#include<fstream>
#include"styles.h"
//namespacing this for easier shit
namespace fs = std::filesystem;
namespace tc = color;

int main()
{
    //let this be
    fs::path CurrentPath{fs::current_path()};

    while(true)
    {
        std::cout<<CurrentPath.string()<<"$:";
        std::string command,argument,input;
        std::getline(std::cin,input);      //got the prompt baby
        std::stringstream ss(input);
        ss>>command>>argument;

        if(command=="q") break;          //quiting

        else if(command=="ls")           //listing files and directories
        {
            std::vector<fs::directory_entry> dir,file,entries;

            for( const auto& entry: fs::directory_iterator(CurrentPath))
            {                /**** to differentiate between directories and files ****/
                if(entry.is_directory()) dir.push_back(entry);  //just directories 
                else file.push_back(entry);                
            }

            std::sort(dir.begin(),dir.end(),[](const auto& a, const auto& b)
            {
                return a.path().filename().string() < b.path().filename().string();
            });

            std::sort(file.begin(),file.end(),[](const auto& a, const auto& b)
            {
                return a.path().filename().string() < b.path().filename().string();
            });

            for(const auto& d : dir)
            {
                std::cout<<tc::BRIGHT_YELLOW<<"[DIR]:"<<d.path().filename().string()<<tc::RESET<<std::endl;
            }
            std::cout<<"\n";
            for(const auto& f : file)
            {
                std::cout<<tc::BRIGHT_MAGENTA<<"[FILE]:"<<f.path().filename().string()<<tc::RESET<<std::endl;
            }
        }

        else if(command=="pwd")      //listing the current directory
        {
            std::cout<<tc::BRIGHT_BLUE<<"Current Directory: ";
            std::cout<<CurrentPath.string()<<tc::RESET;
            std::cout<<std::endl;
        }

        else if(command=="cd")
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

        else if(command=="mkdir")
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

        else if(command == "touch")
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

        else if(command == "rm")
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

        else if(command == "info")
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


        else std::cout<<tc::BRIGHT_RED<<"invalid Input! \n"<<tc::RESET;        //exceptions


    }

    return 0;

}



