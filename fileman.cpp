#include<iostream>
#include<string>
#include<filesystem>
#include<sstream>
#include<vector>
#include<algorithm>
#include<fstream>
#include<ncurses.h>
// #include"styles.h"
#include"all_commands.h"


enum class Mode
{
    NORMAL,
    INPUT,
};


enum class Action
{
    NONE,
    MKDIR,
    TOUCH,
    RENAME,
    REMOVE,
};


//namespacing this for easier shit
namespace fs = std::filesystem;
//namespace tc = color;


int main()
{
    //let this be
    fs::path CurrentPath{fs::current_path()};
    int selected{};
    int offset{};

    Mode mode = Mode::NORMAL;
    Action action = Action::NONE;
    std::string inputbuffer;
    

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
        {
            selected = 0;
            offset = 0;
        }
        else
        {
            if(selected >= entries.size())
                selected = entries.size()-1;
            if(offset > selected)
                offset = selected;
        }
        if(selected >= entries.size() && !entries.empty()) 
            selected = entries.size() - 1;
        
        

        //RENDER-THINGS(not real render)

        clear();
        /*---------------------------HEADER--------------------------------*/
        mvprintw(0,0,"\t\t[PATH]:%s",CurrentPath.c_str());

        /*-------------------------FILE LIST----------------------------------*/
        int visible_height{LINES - 1};

        for(int i = offset; i<(int)entries.size() && i< (offset + visible_height); ++i)
        {
            int row = i - offset + 1;

            if(i == selected)
                attron(A_REVERSE);

            std::string name{entries[i].path().filename().string()};

            #if 1
            if(mode == Mode::INPUT && action == Action::RENAME && i == selected)
            {
                name = inputbuffer;
                move(row, (entries[i].is_directory()?7:8)+inputbuffer.size());
                curs_set(1);
            }
            else if(mode == Mode::INPUT && action == Action::MKDIR)
            {
                mvprintw(LINES - 1 , 0, "[DIR]: %s", inputbuffer.c_str());
            }

            else if(mode ==Mode::INPUT && action == Action::TOUCH)
            {
                mvprintw(LINES - 1 , 0, "[FILE]: %s", inputbuffer.c_str());
            }
            #endif
            else
            {
                curs_set(0);
            }

            if(entries[i].is_directory())
                mvprintw(row,0,"[DIR]:%s",name.c_str());
            else
                mvprintw(row,0,"[FILE]:%s",name.c_str());           

            if(i == selected)
                attroff(A_REVERSE);
        }


        /*----------------------INPUT--------------------*/
        int key{getch()};


        //--------------------NAVIGATION ACTUALLY(maybe logic) -----------------------------------
        if(mode == Mode::NORMAL)
        {
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
            else if(( key == 'l'|| key == '\n') && !entries.empty())
            {
                if(entries[selected].is_directory())
                {
                    CurrentPath = entries[selected].path();
                }
            }
            else if(key =='h' || key == KEY_BACKSPACE || key == 127 || key == 8)
            {
                if(CurrentPath.has_parent_path())
                {
                    CurrentPath = CurrentPath.parent_path();
                    selected = 0;
                    offset = 0;
                }
            }

            if(key == 'm')
            {
                mode = Mode::INPUT;
                action = Action::MKDIR;
                inputbuffer.clear();
            }
            
            if(key == 't')
            {
                mode = Mode::INPUT;
                action = Action::TOUCH;
                inputbuffer.clear();
            }

            if(key == 'r' && !entries.empty())
            {
                mode = Mode::INPUT;
                action = Action::RENAME;
                inputbuffer.clear();
            }
        }

        else if(mode == Mode::INPUT)
        {
            if(isprint(key))
            {
                inputbuffer+=(char)key;
            }
            else if(key == KEY_BACKSPACE)
            {
                if(!inputbuffer.empty())
                    inputbuffer.pop_back();
            }
            else if(key == '\n' || key == KEY_ENTER)
            {
                if(action == Action::MKDIR)
                {
                    mkdir(CurrentPath, inputbuffer);
                }
                else if(action == Action::TOUCH)
                {
                    touch(CurrentPath, inputbuffer);
                }
                else if(action == Action::RENAME)
                {
                    if(!entries.empty())
                        renamefile((entries[selected].path()), (CurrentPath / inputbuffer));
                }
                inputbuffer.clear();
                mode = Mode::NORMAL;
                action = Action::NONE;
            }
            else if(key == 27)  //ESC i dont know if there is a KEY_ESC variant
            {
                inputbuffer.clear();
                mode = Mode::NORMAL;
                action = Action::NONE;
            }
        }

        //FINALE REFRESHING  :)
        refresh();
    }
    endwin();
    return 0;

}



