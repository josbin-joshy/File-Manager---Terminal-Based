#include<filesystem>
#include<vector>
#include<iostream>
#include"styles.h"
#include<algorithm>

namespace fs = std::filesystem;
namespace tc = color;

void ls(const fs::path& CurrentPath)
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