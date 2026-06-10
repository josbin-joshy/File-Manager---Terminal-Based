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
    int selected{};
    int offset{};
    

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);



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

        if(!entries.empty())
        {
            if(selected >= entries.size())
                selected = entries.size()-1;
            if(offset > selected)
                offset = selected;
        }
        else
        {
            selected = 0;
            offset = 0;
        }

        //RENDER-THINGS(not real render)

        clear();
        /*---------------------------HEADER--------------------------------*/
        mvprintw(0,0,"[PATH]:%s",CurrentPath.c_str());

        /*-------------------------FILE LIST----------------------------------*/
        int visible_height{LINES - 2};

        for(int i = offset; i<(int)entries.size() && i< (offset + visible_height); ++i)
        {
            int row = i - offset + 1;

            if(i == selected)
                attron(A_REVERSE);
            
            if(entries[i].is_directory())
                mvprintw(row,0,"[DIR]:%s",entries[i].path().filename().c_str());
            else
                mvprintw(row,0,"[FILE]:%s",entries[i].path().filename().c_str());

            if(i == selected)
                attroff(A_REVERSE);
        }


        /*----------------------INPUT--------------------*/
        int key{getch()};
        //DEBUG
        mvprintw(0,0,"key = %d ", key);


        //--------------------LOGIC SHIT -----------------------------------
        if(key == 'q')
            break;
        else if(key == 'j' || key == KEY_DOWN)
        {
            if(selected+1 < entries.size())
                ++selected;
            if(selected >= offset + visible_height)
                ++offset;
        }
        else if(key == 'k' || key == KEY_UP)
        {
            if(selected >0)
                --selected;
            if(selected < offset)
                --offset;
        }
        else if( key == 'l'|| key == '\n')
        {
            if(entries[selected].is_directory())
            {
                CurrentPath = entries[selected].path();
            }
        }
        else if(key =='h' || key == KEY_BACKSPACE)
        {
            if(CurrentPath.has_parent_path())
            {
                CurrentPath = CurrentPath.parent_path();
                selected = 0;
                offset = 0;
            }
        }

        //FINALE REFRESHING  :)
        refresh();
    }
    endwin();
    return 0;

}



