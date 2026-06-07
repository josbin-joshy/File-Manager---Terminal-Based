#pragma once
#include<filesystem>
#include<string>

namespace fs = std::filesystem;

void ls(const fs::path& CurrentPath);
void pwd(const fs::path& CurrentPath);
void cd(fs::path& CurrentPath, const std::string& argument);
void mkdir(const fs::path& CurrentPath, const std::string& argument);
void touch(fs::path& CurrentPath, const std::string& argument);
void rm(fs::path& CurrentPath, const std::string& argument);
void info(fs::path& CurrentPath, const std::string& argument);