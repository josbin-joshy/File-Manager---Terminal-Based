#include<iostream>
#include<string>
#include<filesystem>
#include<sstream>
#include<vector>

//namespacing this for easier shit
namespace fs=std::filesystem;

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
            std::vector<fs::directory_entry> dir,file;

            for( const auto& entry: fs::directory_iterator(CurrentPath))
            {

                /**** to differentiate between directories and files ****/
                if(entry.is_directory()) dir.push_back(entry);  //just directories 
                else file.push_back(entry);                
            }

            for(const auto& d : dir)
            {
                std::cout<<"[DIR]:"<<d.path().filename().string()<<std::endl;
            }
            std::cout<<"\n";
            for(const auto& f : file)
            {
                std::cout<<"[FILE]:"<<f.path().filename().string()<<std::endl;
            }
        }

        else if(command=="pwd")      //listing the current directory
        {
            std::cout<<"Current Directory: ";
            std::cout<<CurrentPath.string();
            std::cout<<std::endl;
        }

        else if(command=="cd")
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
        else std::cout<<"invalid Input! \n";        //exceptions


    }

    return 0;

}



