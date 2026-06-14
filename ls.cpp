#include<filesystem>
#include<vector>
#include<iostream>
#include<algorithm>

namespace fs = std::filesystem;

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
                std::cout<<"[DIR]:"<<d.path().filename().string()<<std::endl;
            }
            std::cout<<"\n";
            for(const auto& f : file)
            {
                std::cout<<"[FILE]:"<<f.path().filename().string()<<std::endl;
            }
}