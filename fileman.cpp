#include<iostream>
#include<string.h>
#include<filesystem>

//namespacing this for easier shit
namespace fs=std::filesystem;

int main()
{
    //let this be
    fs::path CurrentPath{fs::current_path()};

    while(true)
    {
        std::cout<<"Enter prompt:  ";
        std::string prompt;
        std::getline(std::cin,prompt);      //got the prompt baby
        if(prompt=="pwd")
        {
            std::cout<<"Current Directory: ";
            std::cout<<CurrentPath.string();
            std::cout<<std::endl;
        }
        else if(prompt=="ls")
            for( const auto& entry: fs::directory_iterator(CurrentPath))
            {

                /**** to differentiate between directories and files ****/
                if(entry.is_directory()) std::cout<<"[DIR]";         //just directories
                else if(entry.is_regular_file()) std::cout<<"[REG]";//regular file liek .txt, .py,.thattypahit
                else std::cout<<"[UNK]";                            //unknown

                std::cout<<entry.path().filename().string()<<'\n';
                
            }

        else if(prompt=="q") break;

        else std::cout<<"invalid Input! \n";
        


    }

    return 0;

}



