#include "filemanager.h"

//TODO: massive refactor
//move everything out of main

int main(int argc, char **argv)
{
	int currentMenuPage = 0;
	int screenSpace = 32;
	std::string copyPath = "";
	bool copyIsFile = true;
	u64 emmcTotalSpace = 0;
	u64 emmcFreeSpace = 0;
	u64 sdmcTotalSpace = 0;
	u64 sdmcFreeSpace = 0;

	std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	);

	std::chrono::milliseconds msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	);

	double timerDuration;

	int cursorOffset = 6;
	cursorPos currentCursorPos;
	cursorPos cursorHomePos;
	cursorHomePos.x = cursorOffset;
	cursorHomePos.y = 2;
	currentCursorPos.x = cursorOffset;
	currentCursorPos.y = 2;
	long unsigned int cursorIndex = cursorHomePos.x - cursorOffset;
	bool inMainMenu = true;
	bool inDeletePrompt = false;

	std::string rootDeviceName = "";
	std::string rootDevicePath = rootDeviceName;
	rootDevicePath.append(":/");
	std::string currentDirPath = rootDevicePath;
	std::string currentFolder;
	std::string previousFolder;
	
	DirectoryList currentDirList;
	
	gfxInitDefault();
	consoleInit(NULL);

	FsFileSystem emmcFs;

	fsInitialize();

	const char *emmcStrBuff = "";
	FsFileSystem *sdmcFs = fsdevGetDefaultFileSystem();
	moveCursor(0, 0);
	fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
	fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
	printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
	moveCursor(cursorHomePos.x, cursorHomePos.y);
	printMainMenu();   
		

	while(appletMainLoop()){
		hidScanInput();
		
		ms = std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()
		);

		timerDuration = (ms - msStart).count() / (double) CLOCKS_PER_SEC;

		moveCursor(currentCursorPos.x, currentCursorPos.y);
		printf(">");

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

		if(kHeld & KEY_DOWN)
		{
			if(timerDuration >= 1){
				if(inMainMenu){
					if((currentCursorPos.x + 1) <= 3 + cursorOffset){
						moveCursor(currentCursorPos.x, currentCursorPos.y);
						printf(" ");			
						currentCursorPos.x++;
						moveCursor(currentCursorPos.x, currentCursorPos.y);
						printf(">");
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
							std::chrono::system_clock::now().time_since_epoch()
						);
					}
				} else {
					if(cursorIndex + 1 < currentDirList.size())
					{
						if((currentCursorPos.x + 1) > screenSpace + cursorOffset - 1)
						{
							currentMenuPage++;
							clearScr();
							moveCursor(0, 0);
							fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
							fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
							fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
							fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
							printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
							moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
							printDirectory(currentDirList, screenSpace, currentMenuPage);
							currentCursorPos = cursorHomePos;
							cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
							msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
								std::chrono::system_clock::now().time_since_epoch()
							);
						} else {
							moveCursor(currentCursorPos.x, currentCursorPos.y);
							printf(" ");			
							currentCursorPos.x++;
							moveCursor(currentCursorPos.x, currentCursorPos.y);
							printf(">");
							cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
							msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
								std::chrono::system_clock::now().time_since_epoch()
							);
						}
					}
				}
			}
		}

		else if(kHeld & KEY_UP)
		{
			if(timerDuration >= 1)
			{
				if((currentCursorPos.x - 1) > 0 + cursorOffset - 1)
				{
					moveCursor(currentCursorPos.x, currentCursorPos.y);
					printf(" ");
					currentCursorPos.x--;
					moveCursor(currentCursorPos.x, currentCursorPos.y);
					printf(">");
					cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
					msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
						std::chrono::system_clock::now().time_since_epoch()
					);
				} else {
					if(currentMenuPage > 0)
					{
						currentMenuPage--;
						clearScr();
						moveCursor(0, 0);
						fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
						fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
						fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
						fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
							std::chrono::system_clock::now().time_since_epoch()
						);
					}
				}
			}
		}

		else if(kHeld & KEY_RIGHT)
		{
			if(timerDuration >= 1)
			{
				if(currentMenuPage < (int)currentDirList.size() / screenSpace)
				{
					currentMenuPage++;
					clearScr();
					moveCursor(0, 0);
					fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
					fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
					fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
					fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
					printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
					moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
					printDirectory(currentDirList, screenSpace, currentMenuPage);
					cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
					msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
						std::chrono::system_clock::now().time_since_epoch()
					);
					if(cursorIndex > currentDirList.size())
					{
						moveCursor(currentCursorPos.x, currentCursorPos.y);
						printf(" ");
						currentCursorPos.x -= cursorIndex - currentDirList.size() + 1;
						cursorIndex = currentDirList.size();
						moveCursor(currentCursorPos.x, currentCursorPos.y);
						printf(">");
					}
				}
			}
		}

		else if(kHeld & KEY_LEFT)
		{
			if(timerDuration >= 1)
			{
				if(currentMenuPage > 0)
				{
					currentMenuPage--;
					clearScr();
					moveCursor(0, 0);
					fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
					fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
					fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
					fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
					printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
					moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
					printDirectory(currentDirList, screenSpace, currentMenuPage);
					cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
					msStart = std::chrono::duration_cast< std::chrono::milliseconds >(
						std::chrono::system_clock::now().time_since_epoch()
					);
				}
			}
		}
		
		else if(kDown & KEY_A){

			if(inMainMenu){
				switch(cursorIndex){
					case 0: 
						if(rootDeviceName.compare("sdmc") != 0){
							fsdevUnmountDevice(rootDeviceName.c_str());
						}
						rootDeviceName = "kSAFE";
						rootDevicePath = rootDeviceName;
						rootDevicePath.append(":/");
						currentDirPath = rootDevicePath; 
						fsOpenBisFileSystem(&emmcFs, 29, emmcStrBuff);
						fsdevMountDevice(rootDeviceName.c_str(), emmcFs);
						clearScr();
						moveCursor(0, 0);
						fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
						fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
						fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
						fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
						currentMenuPage = 0;
						populateDirectory(currentDirPath, currentDirList);
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						moveCursor(cursorHomePos.x, cursorHomePos.y);
						currentCursorPos = cursorHomePos;
						inMainMenu = !inMainMenu;
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						break;

					case 1:
						if(rootDeviceName.compare("sdmc") != 0){
							fsdevUnmountDevice(rootDeviceName.c_str());
						}
						rootDeviceName = "kSYSTEM";
						rootDevicePath = rootDeviceName;
						rootDevicePath.append(":/");
						currentDirPath = rootDevicePath; 
						fsOpenBisFileSystem(&emmcFs, 31, emmcStrBuff);
						fsdevMountDevice(rootDeviceName.c_str(), emmcFs);
						clearScr();
						moveCursor(0, 0);
						fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
						fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
						fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
						fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
						currentMenuPage = 0;
						populateDirectory(currentDirPath, currentDirList);
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						moveCursor(cursorHomePos.x, cursorHomePos.y);
						currentCursorPos = cursorHomePos;
						inMainMenu = !inMainMenu;
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						break;
					case 2:
						if(rootDeviceName.compare("sdmc") != 0){
							fsdevUnmountDevice(rootDeviceName.c_str());
						}
						rootDeviceName = "kUSER";
						rootDevicePath = rootDeviceName;
						rootDevicePath.append(":/");
						currentDirPath = rootDevicePath; 
						fsOpenBisFileSystem(&emmcFs, 30, emmcStrBuff);
						fsdevMountDevice(rootDeviceName.c_str(), emmcFs);
						clearScr();
						moveCursor(0, 0);
						fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
						fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
						fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
						fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
						currentMenuPage = 0;
						populateDirectory(currentDirPath, currentDirList);
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						moveCursor(cursorHomePos.x, cursorHomePos.y);
						currentCursorPos = cursorHomePos;
						inMainMenu = !inMainMenu;
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						break;
					case 3:
						rootDeviceName = "sdmc";
						rootDevicePath = rootDeviceName;
						rootDevicePath.append(":/");
						currentDirPath = rootDevicePath; 
						clearScr();
						moveCursor(0, 0);
						fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
						fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
						populateDirectory(currentDirPath, currentDirList);
						currentMenuPage = 0;
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						moveCursor(cursorHomePos.x, cursorHomePos.y);
						currentCursorPos = cursorHomePos;
						inMainMenu = !inMainMenu;
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						break;
				}
			}
			
			else if(!inMainMenu){

				if(!currentDirList.at(cursorIndex).isempty){
					if(!currentDirList.at(cursorIndex).isfile){

						clearScr();
						currentFolder = currentDirList.at(cursorIndex).filename;

						currentDirPath.append(currentDirList.at(cursorIndex).filename);
						moveCursor(cursorOffset, 2);
						currentDirPath.append("/");

						clearScr();
						moveCursor(0, 0);

						fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
						fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
						fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
						fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
						populateDirectory(currentDirPath, currentDirList);
						currentMenuPage = 0;
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						currentCursorPos = cursorHomePos;
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
					}
				}
			}
		}

		else if(kDown & KEY_B){
			if(currentDirPath.compare(rootDevicePath) != 0){
				currentDirPath = currentDirPath.substr(0, currentDirPath.find_last_of("/"));
				currentDirPath = currentDirPath.substr(0, currentDirPath.find_last_of("/"));
				currentDirPath.append("/");
				clearScr();
				moveCursor(0, 0);
				fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
				fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
				fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
				fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
				printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
				moveCursor(cursorHomePos.x - 1, cursorHomePos.y);
				populateDirectory(currentDirPath, currentDirList);
				currentMenuPage = 0;
				printDirectory(currentDirList, screenSpace, currentMenuPage);
				cursorIndex = cursorHomePos.x - cursorOffset;
				moveCursor(cursorHomePos.y, cursorHomePos.x);
				currentCursorPos = cursorHomePos;
				cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
			}

			else{
				if(!inMainMenu){
					clearScr();
					moveCursor(0, 0);
					fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
					fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
					fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
					fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
					printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
					moveCursor(cursorHomePos.x, cursorHomePos.y);
					printMainMenu();
					currentMenuPage = 0;
					cursorIndex = cursorHomePos.x - cursorOffset;
					moveCursor(cursorHomePos.x, cursorHomePos.y);
					currentCursorPos = cursorHomePos;
					cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
					inMainMenu = !inMainMenu;
				}
			}
		}

		else if(kDown & KEY_X){
			if(!inMainMenu){
				if(!currentDirList.at(cursorIndex).isempty){
					copyPath = currentDirPath;
					copyPath.append(currentDirList.at(cursorIndex).filename);
					copyIsFile = currentDirList.at(cursorIndex).isfile;
					clearScr();
					moveCursor(0, 0);
					fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
					fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
					fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
					fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
					printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
					populateDirectory(currentDirPath, currentDirList);
					printDirectory(currentDirList, screenSpace, currentMenuPage);
				}
			}
		}

		else if(kDown & KEY_Y){			
			if(!inMainMenu){
				inDeletePrompt = true;
				moveCursor(5, 50);
				printf("______________________________");
				moveCursor(6, 50);
				printf("|                            |");
				moveCursor(7, 50);
				printf("|         COPY ENTRY         |");
				moveCursor(8, 50);
				printf("|        Are you sure?       |");
				moveCursor(9, 50);
				printf("| Press the A Button to copy |");
				moveCursor(10, 50);
				printf("| Any other button to cancel |");
				moveCursor(11, 50);
				printf("|____________________________|");
				moveCursor(currentCursorPos.y, currentCursorPos.x);
				while(inDeletePrompt){
					gfxFlushBuffers();
					gfxSwapBuffers();
					gfxWaitForVsync();

					hidScanInput();
					u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

					if(kDown & KEY_A)
					{
						std::string dstPath;
						dstPath = currentDirPath;
						dstPath.append(currentDirList.at(cursorIndex).filename);
						dstPath = dstPath.substr(0, dstPath.find_last_of("/") + 1);
						if(copyPath.substr(0, copyPath.find_last_of("/") + 1).compare(dstPath) != 0)
						{
							dstPath.append(copyPath.substr(copyPath.find_last_of("/") + 1));
							if(copyIsFile)
								copyFile(copyPath, dstPath, rootDeviceName);
							else
								copyDirectory(copyPath, dstPath, rootDeviceName);
							clearScr();
							moveCursor(0, 0);
							fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
							fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
							fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
							fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
							printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
							populateDirectory(currentDirPath, currentDirList);
							printDirectory(currentDirList, screenSpace, currentMenuPage);
							moveCursor(4, 2);
							printf("Copied!");
							cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
							currentCursorPos = cursorHomePos;
						}
						inDeletePrompt = false;
						break;
					} else if(kDown && !(kDown & KEY_A)) {
						clearScr();
						moveCursor(0, 0);
						printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
						populateDirectory(currentDirPath, currentDirList);
						printDirectory(currentDirList, screenSpace, currentMenuPage);
						moveCursor(4, 2);
						printf("Cancelled!");
						currentCursorPos = cursorHomePos;
						cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
						inDeletePrompt = !inDeletePrompt;
						break;
					}
				}
			}
		}

		else if(kDown & KEY_MINUS){
			if(!inMainMenu){
				if(!currentDirList.at(cursorIndex).isempty){
					inDeletePrompt = true;
					moveCursor(5, 50);
					printf("______________________________");
					moveCursor(6, 50);
					printf("|                            |");
					moveCursor(7, 50);
					printf("|        DELETE ENTRY        |");
					moveCursor(8, 50);
					printf("|        Are you sure?       |");
					moveCursor(9, 50);
					printf("|Press the A Button to delete|");
					moveCursor(10, 50);
					printf("| Any other button to cancel |");
					moveCursor(11, 50);
					printf("|____________________________|");
					moveCursor(currentCursorPos.y, currentCursorPos.x);
					while(inDeletePrompt){
						gfxFlushBuffers();
						gfxSwapBuffers();
						gfxWaitForVsync();

						hidScanInput();
						u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

						if(kDown & KEY_A)
						{
							std::string rmName = currentDirList.at(cursorIndex).filename;
							rm(currentDirList.at(cursorIndex));
							clearScr();
							moveCursor(0, 0);
							fsFsGetTotalSpace(&emmcFs, "/", &emmcTotalSpace);
							fsFsGetTotalSpace(sdmcFs, "/", &sdmcTotalSpace);
							fsFsGetFreeSpace(&emmcFs, "/", &emmcFreeSpace);
							fsFsGetFreeSpace(sdmcFs, "/", &sdmcFreeSpace);
							printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
							populateDirectory(currentDirPath, currentDirList);

							printDirectory(currentDirList, screenSpace, currentMenuPage);
							moveCursor(4, 2);
							printf("Deleted: %s", rmName.c_str());
							currentCursorPos = cursorHomePos;
							cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
							inDeletePrompt = !inDeletePrompt;
							break;
						} else if(kDown && !(kDown & KEY_A)) {
							clearScr();
							moveCursor(0, 0);
							printHeader(emmcTotalSpace, emmcFreeSpace, sdmcTotalSpace, sdmcFreeSpace, copyPath);
							populateDirectory(currentDirPath, currentDirList);
							printDirectory(currentDirList, screenSpace, currentMenuPage);
							moveCursor(4, 2);
							printf("Cancelled!");
							currentCursorPos = cursorHomePos;
							cursorIndex = (currentCursorPos.x - cursorOffset) + (screenSpace * currentMenuPage);
							inDeletePrompt = !inDeletePrompt;
							break;
						}
					}
				}
			}
		}

		else if(kDown & KEY_PLUS){
			fsdevUnmountDevice(rootDeviceName.c_str());
			fsdevUnmountAll();
			break;
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	gfxExit();
	return 0;
	
}

