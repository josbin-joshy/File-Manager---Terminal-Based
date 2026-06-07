# Terminal File Manager

A simple terminal-based file manager written in C++.

This project is being built from scratch as a way to learn:

- Modern C++
- std::filesystem
- Command parsing
- Iterators
- Vectors
- Terminal applications
- File system navigation

---

## Features

### Navigation

- `pwd` — Display the current directory
- `cd <directory>` — Change directory
- `cd ..` — Move to the parent directory

### Directory Listing

- `ls` — List the contents of the current directory

Entries are classified as:

```text
[DIR]  Documents
[DIR]  Downloads
[FILE] notes.txt
[FILE] main.cpp
```

### Program Control

- `q` — Quit the application

---


## Technologies Used

- C++17
- std::filesystem
- std::vector
- std::stringstream
- Range-based for loops

---

## How It Works

The application maintains a current working directory using:

```cpp
fs::path CurrentPath{fs::current_path()};
```

Commands entered by the user are parsed using:

```cpp
std::stringstream
```

For example:

```text
cd Downloads
```

becomes:

```cpp
command  = "cd";
argument = "Downloads";
```

The application then executes the corresponding filesystem operation.

---

## Example Session

```text
Enter prompt: pwd
Current Directory:
/home/user

Enter prompt: ls
[DIR]  Documents
[DIR]  Downloads
[FILE] notes.txt

Enter prompt: cd Downloads

Enter prompt: pwd
Current Directory:
/home/user/Downloads

Enter prompt: q
```

---

## Concepts Learned

### Filesystem

- `fs::path`
- `fs::current_path()`
- `fs::exists()`
- `fs::is_directory()`
- `fs::canonical()`
- `fs::directory_iterator`

### Containers

- `std::vector`

Used to store directory entries before processing and displaying them.

### Iterators

Example:

```cpp
for (const auto& entry : fs::directory_iterator(CurrentPath))
{
    // Process entry
}
```

### Command Parsing

Using:

```cpp
std::stringstream
```

to split user input into:

- command
- arguments

---

## Future Improvements
- File size display
- File permissions display
- Hidden file support
- Colored terminal output
- Delete directories
- Copy and move operations
- Interactive navigation using arrow keys
- Full-screen terminal interface using ncurses

---

## Building

### Linux

```bash
g++ -std=c++17 main.cpp -o filemanager
./filemanager
```

### Windows (MinGW)

```bash
g++ -std=c++17 main.cpp -o filemanager.exe
filemanager.exe
```

---

## Motivation

This project was created as a hands-on way to learn how file systems work in C++, how command-line applications are structured, and how terminal-based file managers can be built from scratch before moving on to more advanced interfaces such as ncurses-based applications.
