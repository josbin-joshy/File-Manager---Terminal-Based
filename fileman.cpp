#include<iostream>
#include<string>
#include<filesystem>
#include<sstream>
#include<vector>
#include<algorithm>
#include<fstream>
#include<ncurses.h>
// #include"styles.h"
// #include"all_commands.h"
//namespacing this for easier shit
namespace fs = std::filesystem;
//namespace tc = color;


int main()
{
    //let this be
    fs::path CurrentPath{fs::current_path()};
    int selected = 0;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr,TRUE);



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

        clear();

        mvprintw(0,0,"[PATH]:%s\n",CurrentPath.c_str());

        for(size_t i = 0; i<entries.size(); ++i)
        {
            if(i == selected)
                mvprintw(i+2,0,">>");
            else
                mvprintw(i+2,0," ");
            
            if(entries[i].is_directory())
                printw("[DIR]:");
            else
                printw("[FILE]:");
            
            printw("%s\n",entries[i].path().filename().c_str());
        }


        //INPUT
        char key{getch()};
        
        if(key == 'q')
            break;
        else if(key == 'j')
        {
            if(selected+1 <= entries.size())
                ++selected;
        }
        else if(key == 'k')
        {
            if(selected >0)
            {
                --selected;
            }
        }
        refresh();
    }
    return 0;

}



