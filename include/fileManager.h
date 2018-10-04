#ifndef FILEMANAGER_H__
#define FILEMANAGER_H__

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sys/stat.h>
#include <math.h>
#include <vector>
#include <switch.h>
#include <chrono>
#include <unistd.h>

struct cursorPos{
	int x;
	int y;
};

struct DirectoryEntry{
	int index;
	std::string filename;
	std::string fullpath;
	bool isfile;
	bool isempty;
};

typedef std::vector<DirectoryEntry> DirectoryList;

std::string tail(std::string const& source, size_t const length);
void moveCursor(int y, int x);
void clearScr();

void printHeader(u64 emmcTotal, u64 emmcFree, u64 sdmcTotal, u64 sdmcFree, std::string copyPath);
void printMainMenu();
void printDirectory(DirectoryList dirList, int screenSpace, int page);


bool is_file(const char* path);
bool dirExists(std::string path);
bool fileExists(std::string path);
void populateDirectory(std::string path, DirectoryList& dirList);
void copyFile(std::string srcPath, std::string dstPath, std::string deviceName);
void copyDirectory(std::string srcPath, std::string dstPath, std::string deviceName);
void rm(DirectoryEntry path);


#endif