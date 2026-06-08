#include<iostream>
#include<string>
#include<filesystem>
#include<sstream>
#include<vector>
#include<algorithm>
#include<fstream>
// #include"styles.h"
// #include"all_commands.h"
//namespacing this for easier shit
namespace fs = std::filesystem;
//namespace tc = color;


int main()
{
    //let this be
    fs::path CurrentPath{fs::current_path()};
    size_t selected = 0;
    while(true)
    {

        std::vector<fs::directory_entry> entries;

        //LOAD STATE
        for(auto& entry: fs::directory_iterator(CurrentPath)) 
            entries.push_back(entry);

            if(entries.empty()) 
                selected = 0;
            if(selected >= entries.size() && !entries.empty()) 
                selected = entries.size() - 1;

        //RENDER

        system("clear");

        std::cout<<"[PATH]:"<<CurrentPath<<'\n'<<std::endl;

        for(size_t i = 0; i<entries.size(); ++i)
        {
            if(i == selected)
                std::cout<<">>";
            else
                std::cout<<" ";
            
            if(entries[i].is_directory())
                std::cout<<"[DIR]:";
            else
                std::cout<<"[FILE]:";
            
            std::cout<<entries[i].path().filename().string()<<std::endl;
        }


        //INPUT
        char key;
        std::cin>>key;
        
        if(key == 'q')
            break;
        else if(key == 'j')
        {
            if(selected+1 <= entries.size())
                ++selected;
        }
        else if(key == 'k')
        {
            if(selected-1 >0)
            {
                --selected;
            }
        }
    }
    return 0;

}



