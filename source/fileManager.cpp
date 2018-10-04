#include "fileManager.h"

//////////////////////
// String utilities //
//////////////////////

std::string tail(std::string const& source, size_t const length)
{
	if (length >= source.size()) 
		return source;
	return source.substr(source.size() - length);
}

void moveCursor(int y, int x)
{
	printf("\033[%d;%dH", (y), (x));
}

void clearScr()
{
	printf("\e[1;1H\e[2J");
}

//////////////////////////
// Display file manager //
//////////////////////////

void printHeader(u64 emmcTotal, u64 emmcFree, u64 sdmcTotal, u64 sdmcFree, std::string copyPath)
{
	moveCursor(1, 0);
	printf("\033[0;31m");
	printf("partition total: ");
	printf("\033[0m");
	printf(std::to_string((int)trunc(emmcTotal / pow(1024, 2))).c_str());
	printf(" MB");
	printf("\033[0;31m");
	moveCursor(1, 50);
	printf("sdmc total: ");
	printf("\033[0m");
	printf(std::to_string((int)trunc(sdmcTotal / pow(1024, 2))).c_str());
	printf(" MB");
	moveCursor(1,31);
	printf("\033[0;32m");
	printf("kEx NAND Explorer");
	moveCursor(2,37);
	printf("\033[0;32m");
	printf("v0.04");
	moveCursor(2, 0);
	printf("\033[0;31m");
	printf("partition free: ");
	printf("\033[0m");
	printf(std::to_string((int)trunc(emmcFree / pow(1024, 2))).c_str());
	printf(" MB");
	moveCursor(40, 1);
	printf("Clipboard: %s", copyPath.c_str());
	moveCursor(2, 50);
	printf("\033[0;31m");
	printf("sdmc free: ");
	printf("\033[0m");
	printf(std::to_string((int)trunc(sdmcFree / pow(1024, 2))).c_str());
	printf(" MB");
	printf("\n");
	printf("\033[0;31m");
	printf("--------------------------------------------------------------------------------");
	printf("\033[0m");
	printf("\n");
}

void printMainMenu()
{
	printf(" ");
	printf("SAFE:/");
	printf("\n");
	printf("  ");
	printf("SYSTEM:/");
	printf("\n");
	printf("  ");
	printf("USER:/");
	printf("\n");
	printf("  ");
	printf("SD:/");
}

void printDirectory(DirectoryList dirList, int screenSpace, int page)
{
	long unsigned int space = (long unsigned int)screenSpace;
	printf(dirList.at(0).fullpath.substr(0, dirList.at(0).fullpath.find_last_of("/") + 1).c_str());
	printf("\n");
	for(long unsigned int i = (page*space); (i < (page + 1) * space); i++)
	{
		if(i < dirList.size())
		{
			printf("  %s", dirList.at(i).filename.c_str());
			if(!dirList.at(i).isfile)
				printf("/");
		}
		printf("\n");
	}
	if(dirList.size() > space)
			printf("Page %d/%lu\n", page+1, (dirList.size()/space)+1);
}

////////////////////
// File utilities //
////////////////////

bool is_file(const char* path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

bool dirExists(std::string path)
{
	DIR* dir = opendir(path.c_str());
	if(dir)
	{
		closedir(dir);
		return true;
	} else
		return false;
}

bool fileExists(std::string path)
{
	struct stat buf;
	stat(path.c_str(), &buf);
	if(ENOENT == errno)
		return false;
	return true;
}

void populateDirectory(std::string path, DirectoryList& dirList)
{
	struct dirent *ent;
	DIR* dir = opendir(path.c_str());
	int count = 1;
	
	dirList.clear();
	DirectoryEntry blankEntry;
	blankEntry.index = 0;
	blankEntry.isfile = false;
	blankEntry.filename = ".";
	blankEntry.isempty = true;
	blankEntry.fullpath = path;
	blankEntry.fullpath.append(blankEntry.filename);
	dirList.push_back(blankEntry);

	if(dir != NULL)
	{ 
		while ((ent = readdir(dir)) != NULL)
		{
			std::string fname = ent->d_name;
			std::string fpath = path;
			fpath.append(fname);
			DirectoryEntry dirEntry;
			if(is_file(fpath.c_str()) == false)
			{
				std::string fnameApp = fname;
				fnameApp.append("/");
				dirEntry.isfile = false;
			} else
				dirEntry.isfile = true;
			dirEntry.isempty = false;
			dirEntry.index = count;
			dirEntry.filename = fname;
			dirEntry.fullpath = path;
			dirEntry.fullpath.append(dirEntry.filename);
			dirList.push_back(dirEntry);
			count++;
		} 
	}
	closedir(dir);
}

void copyFile(std::string srcPath, std::string dstPath, std::string deviceName)
{
	srcPath.append("/");
	std::ifstream src;
	std::ofstream dst;
	src.open(srcPath, std::ios::binary);
	dst.open(dstPath, std::ios::binary);
	
	dst << src.rdbuf();
	src.close();
	dst.close();
}


void copyDirectory(std::string srcPath, std::string dstPath, std::string deviceName) {
	DirectoryList dirList;

	srcPath.append("/");
	if(!dirExists(dstPath.c_str()))
		mkdir(dstPath.c_str(), 777);
	populateDirectory(srcPath, dirList);
	for(long unsigned int i = 0; i < dirList.size(); i++)
	{
		DirectoryEntry e = dirList.at(i);
		if(is_file(e.fullpath.c_str()))
			copyFile(e.fullpath, dstPath + "/" + e.filename, deviceName);
		else {
			if(fileExists(dstPath + e.filename) && is_file((dstPath + e.filename).c_str()))
					continue;
			if(!e.isempty)
				copyDirectory(e.fullpath + "/", dstPath + "/" + e.filename, deviceName);
		}
	}
}

void rm(DirectoryEntry path)
{
	DirectoryList dirList;
	if(path.isfile)
		remove(path.fullpath.c_str());
	else {
		populateDirectory(path.fullpath + "/", dirList);
		for(long unsigned int i = 0; i < dirList.size(); i++)
		{
			DirectoryEntry e = dirList.at(i);
			if(!e.isempty)
				rm(e);
		}
		rmdir(path.fullpath.c_str());
	}
}
