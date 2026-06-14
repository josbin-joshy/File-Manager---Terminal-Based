#include<filesystem>
#include<iostream>
#include<ncurses.h>
#include"styles.h"


namespace fs = std::filesystem;

void renamefile(const fs::path& oldname, const fs::path& newname)
{
    if(fs::exists(newname))
        mvprintw(LINES + 1 , 0, "File Already Exists");
    else
        fs::rename(oldname,newname);
}