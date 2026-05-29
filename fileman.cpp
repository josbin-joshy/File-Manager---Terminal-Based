

/************************* header files *****************************/
#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <dirent.h>
#include <malloc.h>
#include <sys/stat.h>
#include <cerrno>
#include <iostream>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cstdio>

using namespace std;

/************************* global vars ***********************************/
struct termios orig_termios;
char cwd[256];
vector<char> buffer;
string commandStatus;
vector<string> cwd_stack;
int top = -1;
string home_dir;

/******************************* File class ***************************************/
class File
{
public:
    string fileName;
    string fileSize;
    char *owner;
    char *group;
    char *permissions;
    string last_modified;
    bool is_directory;
    bool is_File_Valid;

public:
    File()
    {
        is_File_Valid = false;
    }
    string getNearestFileSize(int size)
    {
        if (size < 1024)
        {
            return to_string(size) + "  B";
        }
        if (size < pow(2, 20))
        {
            size = size / 1024;
            return to_string(size) + " KB";
        }
        if (size < pow(2, 30))
        {
            size = size / pow(2, 20);
            return to_string(size) + " MB";
        }
        // else in GB
        size = size / pow(2, 30);
        return to_string(size) + " GB";
    }
    File(struct dirent *file)
    {
        fileName = string(file->d_name);
        fileName = file->d_name;

        struct stat st;
        if (stat(file->d_name, &st) != 0)
        {
            is_File_Valid = false;
            return;
        }
        is_File_Valid = true;

        fileSize = getNearestFileSize(st.st_size);

        struct passwd *pw = getpwuid(st.st_uid);
        owner = pw->pw_name;

        struct group *gr = getgrgid(st.st_gid);
        group = gr->gr_name;

        last_modified = string(ctime(&st.st_mtime));

        mode_t perm = st.st_mode;

        is_directory = (perm & S_ISDIR(perm)) ? true : false;
        permissions = (char *)malloc(sizeof(char) * 9 + 1);

        permissions[0] = (perm & S_IRUSR) ? 'r' : '-';
        permissions[1] = (perm & S_IWUSR) ? 'w' : '-';
        permissions[2] = (perm & S_IXUSR) ? 'x' : '-';
        permissions[3] = (perm & S_IRGRP) ? 'r' : '-';
        permissions[4] = (perm & S_IWGRP) ? 'w' : '-';
        permissions[5] = (perm & S_IXGRP) ? 'x' : '-';
        permissions[6] = (perm & S_IROTH) ? 'r' : '-';
        permissions[7] = (perm & S_IWOTH) ? 'w' : '-';
        permissions[8] = (perm & S_IXOTH) ? 'x' : '-';
        permissions[9] = '\0';
    }

    void printFile(int screencols)
    {
        // to eliminate text wrapping
        cout << "\x1b[?7l";
        fprintf(stdout, "%-30.30s\t  %s%s %15s %15s \t%10s \t %.24s\n", fileName.c_str(), (is_directory ? "d\0" : "-\0"), permissions, owner, group, fileSize.c_str(), last_modified.c_str());
    }
};
std::string toLower(std::string s)
{
    // converting string to lower case
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return s;
}
bool sortFilesByNameComparator(File f1, File f2)
{
    string s1 = toLower(f1.fileName);
    string s2 = toLower(f2.fileName);

    if (s1 < s2)
        return true;
    return false;
}

vector<File> listFiles(const char *dirname)
{
    vector<File> output_List;
    DIR *dir = opendir(dirname);
    if (opendir == NULL)
    {
        // failed to open
        return output_List;
    }

    if (dir == NULL)
    {
        return output_List;
    }
    struct dirent *entity;
    entity = readdir(dir);
    chdir(dirname);
    vector<File> directory_List;
    vector<File> file_List;
    File current_directory;
    File parent_directory;
    while (entity != NULL)
    {
        File newFile(entity);

        if (!newFile.is_File_Valid)
        {
            entity = readdir(dir);

            continue;
        }
        if (newFile.fileName == ".")
        {
            current_directory = newFile;
        }
        else if (newFile.fileName == "..")
        {
            parent_directory = newFile;
        }
        else if (newFile.is_directory)
        {

            directory_List.push_back(newFile);
        }
        else
        {
            file_List.push_back(newFile);
        }

        entity = readdir(dir);
    }
    closedir(dir);
    sort(file_List.begin(), file_List.end(), sortFilesByNameComparator);
    sort(directory_List.begin(), directory_List.end(), sortFilesByNameComparator);

    output_List.push_back(current_directory);
    output_List.push_back(parent_directory);

    for (File f1 : directory_List)
    {
        output_List.push_back(f1);
    }
    for (File f2 : file_List)
    {
        output_List.push_back(f2);
    }
    return output_List;
}

/*********************** Terminal Configs ******************************/
struct terminalConfig
{
    int c_row, c_col;
    int screenrows;
    int screencols;
    int display_row_end;
    int total_display_lines;
    int vertical_overflow;
    File curFile;
    vector<File> outputList;
    bool is_vertical_overflow;
};

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

struct terminalConfig E;

/*********************** Miscellaneous functions ******************************/

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode()
{
    // in raw mode,
    // turning off ECHO -> to not display keypress
    // turning off canonical mode -> to enable reading byte by byte
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    atexit(disableRawMode);
}
void cls()
{
    // clearing screen
    cout << "\033[2J\033[1;1H";
}
void positionCursor(int x, int y)
{
    // to print cursor at given row  x and col y
    E.c_row = x;
    E.c_col = y;
    cout << "\033[" << x << ";" << y << "H";
    fflush(stdout);
}
void initFileManager()
{
    // initializing rows and columns of terminal
    E.c_row = 0;
    E.c_col = 0;
    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    {
        cout << "Error in getting window size. Terminating application" << endl;
        exit(1);
    }
}
void resetDisplayFilesConfig()
{

    E.display_row_end = E.screenrows - 3;
    E.total_display_lines = E.display_row_end - 2;
    E.vertical_overflow = 0;
}
void printAppName()
{
    positionCursor(1, 1);
    string appName = "FILE MANAGER";
    int cursorPos = (E.screencols - appName.length()) / 2;
    positionCursor(1, cursorPos);
    cout << appName;
    fflush(stdout);
}

void push_directory()
{
    int size = cwd_stack.size();
    string temp(cwd);
    if (top + 1 == size)
    {

        cwd_stack.push_back(temp);
        top++;
    }
    else
    {
        for (int i = size - 1; i >= top + 1; i--)
        {
            cwd_stack.pop_back();
        }
        cwd_stack.push_back(temp);
        top++;
    }
}
/*************************** normal mode ****************************/
void printNormalMode(int x, int y, const char *msg, int vertical_overflow)
{
    initFileManager();
    cls();
    printAppName();
    // positionCursor(2, 1);
    // cout << "screen rows " << E.screenrows << " "
    //  << "disp end line = " << E.display_row_end << " ";

    int outputLines = E.outputList.size();
    int totalLines = (E.screenrows - 3) - 2;

    int startIndex = 0, endIndex = outputLines - 1;
    E.display_row_end = E.screenrows - 3;

    if (totalLines < outputLines)
    {
        startIndex += vertical_overflow;
        endIndex = startIndex + (totalLines - 1);
        if (endIndex >= outputLines)
        {
            endIndex = outputLines - 1;
            startIndex = endIndex - totalLines + 1;
            (E.vertical_overflow)--;
        }
    }
    else
    {
        E.display_row_end = 3 + outputLines - 1;
        E.vertical_overflow = 0;
    }
    // cout << msg << "  s " << startIndex << " e " << endIndex << " off " << E.vertical_overflow << endl;
    positionCursor(3, 1);
    for (int i = startIndex; i <= endIndex; i++)
    {
        // cout << i << "\t";
        E.outputList[i].printFile(E.screencols);
        if (x - 3 + vertical_overflow == i)
        {

            E.curFile = E.outputList[i];
        }
    }
    fflush(stdout);
    positionCursor(E.screenrows - 2, 0);
    cout
        << "\nNORMAL MODE | FILE: " << E.curFile.fileName << "\nPATH: " << cwd;
    fflush(stdout);
    positionCursor(x, y);
}

void goToParentFolder()
{
    string tempStr(cwd);
    // if it is already root, we cannot go to parent
    if (tempStr == "/")
    {
        return;
    }
    char *ptr = strrchr(cwd, '/');
    char *temp = cwd;
    int i = 0;
    while (temp != ptr)
    {

        temp++;
        i++;
    }
    cwd[i] = '\0';
    if (cwd[0] == '\0')
    {

        cwd[0] = '/';
        cwd[1] = '\0';
    }
    push_directory();
    E.outputList = listFiles(cwd);
    resetDisplayFilesConfig();
    printNormalMode(3, 1, "went to parent", 0);
}
void openFile()
{

    // current folder itself
    if (E.curFile.fileName == ".")
    {
        return;
    }
    // parentFolder
    if (E.curFile.fileName == "..")
    {
        goToParentFolder();
        return;
    }
    // directory
    if (E.curFile.is_directory)
    {
        int cwd_len = strlen(cwd);
        // if last char is not /, then concat /
        if (cwd[cwd_len - 1] != '/')
            strcat(cwd, "/");
        strcat(cwd, ((E.curFile).fileName).c_str());
        push_directory();
        E.outputList = listFiles(cwd);
        // cout << cwd << endl;
        resetDisplayFilesConfig();
        printNormalMode(3, 1, "moved to dir", 0);
        return;
    }
    // files

    if (!fork())
    {
        execl("/usr/bin/xdg-open", "xdg-open", (E.curFile.fileName).c_str(), (char *)0);
    }
}
void setHomeDirectory()
{
    char *homedir = getenv("HOME");
    home_dir = string(homedir);
    if (homedir != NULL)
    {
        strcpy(cwd, homedir);
    }
}

void goPrevious()
{
    if (top > 0)
    {
        top--;
        string temp = cwd_stack[top];
        char *c = new char[temp.length() + 1];
        strcpy(c, temp.c_str());
        strcpy(cwd, c);
        E.outputList = listFiles(cwd);
        resetDisplayFilesConfig();
        printNormalMode(3, 1, "previous", 0);
    }
}
void revisitNext()
{
    int size = cwd_stack.size();
    if (top < size - 1)
    {
        top++;
        string temp = cwd_stack[top];
        char *c = new char[temp.length() + 1];
        strcpy(c, temp.c_str());
        strcpy(cwd, c);
        E.outputList = listFiles(cwd);
        resetDisplayFilesConfig();
        printNormalMode(3, 1, "next", 0);
    }
}

void normalMode()
{

    char c = '\0';

    // setting initial normal mode
    E.outputList = listFiles(cwd);
    E.outputList = listFiles(cwd);

    resetDisplayFilesConfig();
    push_directory();
    printNormalMode(3, 1, "initial", 0);

    while (true)
    {
        // reading key press
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        {
            cout << "error in reading" << endl;
            exit(1);
        }

        // handling exit
        if (c == 'q' || c == 'Q')
        {
            cls();
            positionCursor(1, 1);
            exit(0);
            break;
        }

        // handling control keys starting with esc
        if (c == 27)
        {
            char first;
            char second;
            if (read(STDIN_FILENO, &first, 1) != 1)
            {
                continue;
            }
            if (read(STDIN_FILENO, &second, 1) != 1)
            {
                continue;
            }
            if (first == '[')
            {
                switch (second)
                {
                // move cursor up
                case 'A':
                    if (E.c_row > 3)
                    {
                        printNormalMode(E.c_row - 1, 1, "up", E.vertical_overflow);
                    }
                    else if (E.c_row == 3)
                    {
                        if (E.vertical_overflow > 0)
                        {
                            (E.vertical_overflow)--;
                        }
                        printNormalMode(E.c_row, 1, "up", E.vertical_overflow);
                    }
                    break;

                // move cursor down
                case 'B':
                    if (E.c_row < E.display_row_end)
                        printNormalMode(E.c_row + 1, 1, "down", E.vertical_overflow);
                    else
                    {
                        (E.vertical_overflow)++;
                        printNormalMode(E.c_row, 1, "down", E.vertical_overflow);
                    }

                    break;

                // right - revisit next directory
                case 'C':
                    revisitNext();
                    break;

                // left - previously visited directory
                case 'D':
                    goPrevious();
                    break;

                default:
                    break;
                }
            }
            else if (first == '\0')
            {
                exit(2);
            }
        }
        else
        {
            if (c == 127)
            {
                // backspace
                goToParentFolder();
            }
            else if (c == 'h')
            {
                // home
                setHomeDirectory();
                push_directory();
                E.outputList = listFiles(cwd);
                resetDisplayFilesConfig();
                printNormalMode(3, 1, "home", 0);
            }
            else if (c == 10)
            {
                // enter key
                openFile();
            }
            else if (c == 58)
            {
                // command mode
                return;
            }
        }
    }
    cout << endl;
}

/************ command mode *********************/
void enableIcanon()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    atexit(disableRawMode);
}
void printCommandMode(vector<File> &commandModeList, vector<char> &buffer)
{
    initFileManager();
    cls();
    printAppName();

    int outputLines = commandModeList.size();
    int totalLines = (E.screenrows - 3) - 2;

    int startIndex = 0, endIndex = outputLines - 1;
    E.display_row_end = E.screenrows - 3;

    if (totalLines < outputLines)
    {
        endIndex = startIndex + (totalLines - 1);
    }
    else
    {
        E.display_row_end = 3 + outputLines - 1;
        E.vertical_overflow = 0;
    }

    positionCursor(3, 1);
    for (int i = startIndex; i <= endIndex; i++)
    {
        commandModeList[i].printFile(E.screencols);
    }
    fflush(stdout);
    positionCursor(E.screenrows, 0);
    cout << commandStatus;
    fflush(stdout);
    positionCursor(E.screenrows - 1, 0);
    cout
        << "COMMAND MODE | " << cwd << " $ ";
    int size = buffer.size();
    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == 27 && i != size - 1)
        {
            // not escape, so erasing
            buffer.pop_back();
            buffer.pop_back();
            buffer.pop_back();
            break;
        }
        else if (buffer[i] == 27 && i == size - 1)
        {
            // escape
        }
        else
        {

            cout << buffer[i];
        }
    }
    // cout << x << " " << y << "\t" << E.curFile.fileName;
    fflush(stdout);
}

string handleRelativePaths(string dest)
{
    string cur_path(cwd);
    int n = dest.length();
    // resolving tilda
    if (dest[0] == '~')
    {
        dest.erase(0, 1); // erasing ~
        dest = home_dir + dest;
        return dest;
    }
    // resolving ../
    else if (n > 3 && dest[0] == '.' && dest[1] == '.' && dest[2] == '/')
    {
        int last_index = cur_path.find_last_of('/');
        if (last_index != 0)
        { // if not root
            cur_path = cur_path.substr(0, last_index);
        }
        dest.erase(0, 3); // erasing ../
        string path = cur_path + "/" + dest;
        return path;
    }
    // resolving ..
    else if (n == 2 && dest[0] == '.' && dest[1] == '.')
    {
        int last_index = cur_path.find_last_of('/');
        if (last_index != 0)
        { // if not root
            cur_path = cur_path.substr(0, last_index);
        }
        return cur_path;
    }
    // resolving ./
    else if (n > 2 && dest[0] == '.' && dest[1] == '/')
    {
        dest.erase(0, 1); // erasing .
        dest = cur_path + dest;
        return dest;
    }
    // resolving .
    else if (n == 1 && dest[0] == '.')
    {
        return cur_path;
    }
    return dest; // as it is
}
void changeDirectory(vector<string> args)
{

    string goto_location = "";
    int n = args.size();

    // handling spaces
    for (int i = 1; i < args.size() - 1; i++)
    {
        goto_location += args[i] + " ";
    }
    goto_location += args[n - 1];

    goto_location = handleRelativePaths(goto_location);

    if (goto_location.find_last_of('/') == string::npos)
    {
        goto_location = string(cwd) + "/" + goto_location;
    }
    struct stat goto_stat;
    stat(goto_location.c_str(), &goto_stat);
    if (S_ISDIR(goto_stat.st_mode))
    {

        strcpy(cwd, goto_location.c_str());
        chdir(goto_location.c_str());
        push_directory();
    }
    else
    {
        commandStatus = "ERROR! INVALID DESTINATION";
    }
}

int does_file_exist(string filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
void createFile(vector<string> args)
{
    string dest = "";
    if (args.size() == 2)
    {
        dest = string(cwd);
    }
    else
    {
        // concatenating with spaces
        int n = args.size();
        for (int i = 2; i < args.size() - 1; i++)
        {
            dest += args[i] + " ";
        }
        dest += args[n - 1];
    }

    // resolving relative paths
    dest = handleRelativePaths(dest);

    dest += "/" + args[1]; // adding filename

    // checking if file already exists
    if (does_file_exist(dest))
    {
        commandStatus = "ERROR! FILE ALREADY EXISTS!";
        return;
    }

    // creating
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = creat(dest.c_str(), mode);
    if (fd == -1)
    {
        commandStatus = "ERROR! FILE CREATION FAILED : " + dest;
    }
    else
    {
        commandStatus = "File created successfully";
    }
    close(fd);
}
void createFolder(vector<string> args)
{
    string folderName = args[1];

    string dest = "";
    if (args.size() == 2)
    {
        dest = string(cwd);
    }
    else
    {
        // concatenating with spaces
        int n = args.size();
        for (int i = 2; i < args.size() - 1; i++)
        {
            dest += args[i] + " ";
        }
        dest += args[n - 1];
    }

    // resolving relative paths
    dest = handleRelativePaths(dest);

    dest += "/" + folderName;

    if (mkdir(dest.c_str(), 0777) == -1)
    {
        commandStatus = "ERROR! DIRECTORY CREATION FAILED : " + dest;
    }
    else
    {
        commandStatus = "Directory created successfully";
    }
}
void renameFile(vector<string> args)
{
    string path(cwd);
    string oldName = path + "/" + string(args[1]);
    string newName = path + "/" + string(args[2]);

    if (rename(oldName.c_str(), newName.c_str()) != 0)
        commandStatus = "ERROR! COULD NOT RENAME : " + oldName;
    else
        commandStatus = "File renamed successfully";
}
void moveSingleFile(string source, string destination)
{
    string dest = handleRelativePaths(destination);
    string fileName = "";

    // checking for slash
    if (source.find_last_of('/') == string::npos)
    {
        fileName = source;
        source = string(cwd) + "/" + source;
    }
    else
    {
        source = handleRelativePaths(source);
        int last_index = source.find_last_of('/');
        last_index++;
        fileName = source.substr(last_index, source.length() - last_index);
    }
    dest += "/" + fileName;
    // positionCursor(2, 1);
    // cout << source << "    |||   " << dest << "   |||   " << destination << endl;
    // moving
    if (rename(source.c_str(), dest.c_str()) != 0)
    {
        commandStatus = "ERROR! COULD NOT MOVE FILE(S)";
    }
    else
    {
        commandStatus = "File(s) moved successfully";
    }
}
void moveFiles(vector<string> args)
{
    int n = args.size();
    for (int i = 1; i < n - 1; i++)
    {
        moveSingleFile(args[i], args[n - 1]);
    }
}

void copyFile(string source, string dest)
{
    char buf[BUFSIZ];
    size_t size;

    FILE *source_file = fopen(source.c_str(), "rb");
    FILE *dest_file = fopen(dest.c_str(), "wb");
    if (source_file == NULL || dest_file == NULL)
    {
        commandStatus = "ERROR! INVALID SOURCE OR DESTINATION";
        return;
    }

    while (size = fread(buf, 1, BUFSIZ, source_file))
    {
        fwrite(buf, 1, size, dest_file);
    }

    // copying permissions

    struct stat fst;
    fstat(fileno(source_file), &fst);
    fchown(fileno(dest_file), fst.st_uid, fst.st_gid);
    fchmod(fileno(dest_file), fst.st_mode);
    fclose(source_file);
    fclose(dest_file);
    commandStatus = "File(s) copied successfully";
}
void copyDirectory(string source, string dest)
{

    DIR *source_dir = opendir(source.c_str());
    if (source_dir == NULL)
    {
        commandStatus = "ERROR! INVALID SOURCE : " + source;
        return;
    }
    chdir(source.c_str());
    // extracting mode of source folder to keep same permissions
    struct stat src_stat;
    stat(source.c_str(), &src_stat);

    // creating new folder
    if (mkdir(dest.c_str(), src_stat.st_mode) == -1)
    {
        commandStatus = "ERROR! DIRECTORY CREATION FAILED : " + dest;
        return;
    }
    struct dirent *entity;
    while (entity = readdir(source_dir))
    {
        string entityName(entity->d_name);
        if (entityName == "." || entityName == "..")
        {
            // skip them
            continue;
        }
        // resolving paths
        string entitySource = source + "/" + entityName;
        string entityDestination = dest + "/" + entityName;

        struct stat entity_stat;
        stat(entitySource.c_str(), &entity_stat);

        // if entity is directory
        if (S_ISDIR(entity_stat.st_mode))
        {
            copyDirectory(entitySource, entityDestination);
        }
        else
        {
            copyFile(entitySource, entityDestination);
        }
    }
    chdir(source.c_str()); // resetting
    commandStatus = "Folder(s) copied successfully";
}
void executeCopy(string source, string destination)
{
    string dest = handleRelativePaths(destination);
    string fileName = "";

    // checking for slash
    if (source.find_last_of('/') == string::npos)
    {
        fileName = source;
        source = string(cwd) + "/" + source;
    }
    else
    {
        source = handleRelativePaths(source);
        int last_index = source.find_last_of('/');
        last_index++;
        fileName = source.substr(last_index, source.length() - last_index);
    }

    // to check if folder or file already exists
    string origFileName = "", extension = "";
    int dot_index = fileName.find_last_of('.');
    if (dot_index == string::npos)
    {
        origFileName = fileName;
        extension = "";
    }
    else
    {
        int len = fileName.length();
        origFileName = fileName.substr(0, dot_index);
        extension = fileName.substr(dot_index, len - dot_index);
    }
    int i = 0;
    vector<string> copyName = {"", "_copy", "_another_copy", "_3rd_copy"};
    while (does_file_exist(dest + "/" + fileName))
    {
        i++;
        if (i <= 3)
        {
            fileName = origFileName + copyName[i] + extension;
        }
        else
        {
            fileName = origFileName + "_" + to_string(i) + "th_copy" + extension;
        }
    }

    dest += "/" + fileName;
    // checking if given path is file or directory
    struct stat s;
    if (stat(source.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            // its directory
            copyDirectory(source, dest);
        }
        else if (s.st_mode & S_IFREG)
        {
            // it's a file
            copyFile(source, dest);
        }
    }
    else
    {
        commandStatus = "ERROR! INVALID FILE OR FOLDER : " + source;
    }
}
void copyCommand(vector<string> args)
{
    int n = args.size();
    for (int i = 1; i < n - 1; i++)
    {
        executeCopy(args[i], args[n - 1]);
    }
}
bool search(string name, string path)
{

    DIR *cur_dir = opendir(path.c_str());
    if (cur_dir == NULL)
    {
        commandStatus = "ERROR! INVALID SOURCE : " + path;
        return false;
    }
    struct dirent *entity;
    bool res = false;
    while (entity = readdir(cur_dir))
    {
        string entityName = entity->d_name;
        if (entityName == "." || entityName == "..")
            continue;
        if (entityName == name)
        {
            res = res | true;
        }
        if (res)
        {
            return true;
        }
        // if folder
        if (entity->d_type == DT_DIR)
        {
            string folderPath = path + "/" + entityName;
            res = res | search(name, folderPath);
        }
    }

    return res;
}
bool searchUtil(vector<string> args)
{
    // resolving spaces
    string filename = "";
    int n = args.size();
    for (int i = 1; i < args.size() - 1; i++)
    {
        filename += args[i] + " ";
    }
    filename += args[n - 1];
    getcwd(cwd, 256);
    return search(filename, string(cwd));
}
void deleteFile(string source)
{
    if (remove(source.c_str()) == 0)
    {
        commandStatus = "File deleted successfully";
    }
    else
    {
        commandStatus = "ERROR! FILE COULD NOT BE DELETED : " + source;
    }
}

void deleteDirectory(string source)
{
    DIR *source_dir = opendir(source.c_str());
    if (source_dir == NULL)
    {
        commandStatus = "ERROR! INVALID SOURCE : " + source;
        return;
    }

    struct dirent *entity;
    while (entity = readdir(source_dir))
    {
        string entityName(entity->d_name);
        if (entityName == "." || entityName == "..")
        {
            // skip them
            continue;
        }
        // resolving paths
        string entitySource = source + "/" + entityName;

        struct stat entity_stat;
        stat(entitySource.c_str(), &entity_stat);

        // if entity is directory
        if (S_ISDIR(entity_stat.st_mode))
        {
            deleteDirectory(entitySource);
        }
        else
        {
            deleteFile(entitySource);
        }
    }
    closedir(source_dir);
    if (rmdir(source.c_str()) == 0)
    {
        commandStatus = "Folder deleted successfully";
    }
    else
    {
        commandStatus = "ERROR! FOLDER COULD NOT BE DELETED";
        return;
    }
}
void executeDelete(string source)
{
    string fileName = "";
    getcwd(cwd, 256);
    // checking for slash
    if (source.find_last_of('/') == string::npos)
    {
        fileName = source;
        source = string(cwd) + "/" + source;
    }
    else
    {
        source = handleRelativePaths(source);
    }

    // checking if given path is file or directory
    struct stat s;
    if (stat(source.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            // its directory
            deleteDirectory(source);
        }
        else if (s.st_mode & S_IFREG)
        {
            // it's a file
            deleteFile(source);
        }
    }
    else
    {
        commandStatus = "ERROR! INVALID FILE OR FOLDER : " + source;
    }
}
void deleteCommand(vector<string> args)
{
    int n = args.size();
    for (int i = 1; i < n; i++)
    {
        executeDelete(args[i]);
    }
}
void executeCommand(vector<string> args)
{
    string command = args[0];
    int n = args.size();
    if (command == "quit" || command == "Quit" || command == "QUIT")
    {
        cls();
        exit(0);
    }
    else if (args.size() == 1)
    {
        commandStatus = "ERROR! TOO FEW ARGUMENTS";
    }
    else if (command == "copy" && n >= 3)
    {
        copyCommand(args);
    }
    else if (command == "move" && n >= 3)
    {
        moveFiles(args);
    }
    else if (command == "rename" && n >= 3)
    {
        renameFile(args);
    }
    else if (command == "create_file" && n >= 2)
    {
        createFile(args);
    }
    else if (command == "create_dir" && n >= 2)
    {
        createFolder(args);
    }
    else if (command == "delete_file" && n >= 2)
    {
        deleteCommand(args);
    }
    else if (command == "delete_dir" && n >= 2)
    {
        deleteCommand(args);
    }
    else if (command == "goto" && n == 2)
    {
        changeDirectory(args);
    }
    else if (command == "search" && n == 2)
    {
        if (searchUtil(args))
        {
            commandStatus = "True";
        }
        else
        {
            commandStatus = "False";
        }
    }
    else
    {
        commandStatus = "ERROR! INVALID COMMAND";
    }
}
void commandExtractor(vector<char> &buffer)
{

    // tokenizing
    string temp = "";

    int i = 0;
    vector<string> args;
    for (; i < buffer.size(); i++)
    {
        if (buffer[i] != ' ')
            temp += buffer[i];
        else
        {
            string strCopy = temp;
            args.push_back(strCopy);
            temp = "";
        }
    }
    string strCopy = temp;
    args.push_back(strCopy);
    temp = "";

    executeCommand(args);
}
void commandMode()
{
    vector<File> commandModeList = listFiles(cwd);

    // enableIcanon();
    vector<char> buffer;
    resetDisplayFilesConfig();
    printCommandMode(commandModeList, buffer);

    while (true)
    {
        char ch[3] = {'\0'};
        // char ch;

        if (read(STDIN_FILENO, &ch, 3) == -1 && errno != EAGAIN)
        {
            cout << "error in reading" << endl;
            exit(1);
        }
        if (ch[0] == 27 && ch[1] == '\0')
        {

            return;
        }
        else if (ch[0] == 27 && ch[1] != '\0')
        {
            continue;
        }
        else if (ch[0] == 127)
        { // backspace
            if (buffer.empty())
                continue;
            buffer.pop_back();
            printCommandMode(commandModeList, buffer);

            continue;
        }
        else if (ch[0] == 10)
        { // enter
            commandStatus = "";
            commandExtractor(buffer);
            buffer.clear();
            printCommandMode(commandModeList, buffer);
            continue;
        }
        if (ch[0] != '\0')
            buffer.push_back(ch[0]);
        if (ch[1] != '\0')
            buffer.push_back(ch[1]);
        if (ch[2] != '\0')
            buffer.push_back(ch[2]);
        printCommandMode(commandModeList, buffer);
    }
}
int main()
{
    // setting home
    char *homedir = getenv("HOME");
    home_dir = string(homedir);

    initFileManager();
    enableRawMode();
    cls();

    // getting current working directory
    getcwd(cwd, 256);

    while (true)
    {
        normalMode();
        commandMode();
    }

    return 0;
}
