#include<iostream>
#include<string>
#include<filesystem>
#include<sstream>
#include<vector>
#include<algorithm>
#include<fstream>
#include"styles.h"
#include"all_commands.h"
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
            ls(CurrentPath);
        }

        else if(command=="pwd")      //listing the current directory
        {
            pwd(CurrentPath);
        }

        else if(command=="cd")
        {
            cd(CurrentPath,argument);

        }

        //to create a new directory
        else if(command=="mkdir")
        {
            mkdir(CurrentPath, argument);
        }



        //to create a new file
        else if(command == "touch")
        {
            touch(CurrentPath, argument);
        }


        //to remove a file
        else if(command == "rm")
        {
            rm(CurrentPath, argument);
        }

        //to get info of a file
        else if(command == "info")
        {
            info(CurrentPath, argument);
        }


        //just when you write some random bullshit
        else std::cout<<tc::BRIGHT_RED<<"invalid Input! \n"<<tc::RESET;        //exceptions


    }
    return 0;

}



