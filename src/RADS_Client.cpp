#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <deque>

typedef void(__cdecl *Logger_GetFlags_f)(int*, char const*);
typedef void(__cdecl *Logger_Message_f)(DWORD, int, DWORD, char*, va_list);
struct Logger
{
	/* 0x00 */ char unknown1[0x44];
	/* 0x44 */ Logger_GetFlags_f Logger_GetFlags; // Function pointer 
	/* 0x48 */ char unknown2[0x1c];
	/* 0x64 */ Logger_Message_f Logger_Message; // Function pointer
};
struct FileHandle;
struct FileFinder;

typedef int(__cdecl *RadsInitialize_f)(char const*, DWORD, wchar_t const*, char const*);
typedef int(__cdecl *RadsSetLogger_f)(Logger*);
typedef bool(__cdecl *RadsFileExists_f)(char const* filename);
typedef FileHandle*(__cdecl *RadsOpenFile_f)(char const* filename);
typedef int(__cdecl *RadsCloseFile_f)(FileHandle*);
typedef int(__cdecl *RadsGetFileSize_f)(FileHandle*);
typedef int(__cdecl *RadsRead_f)(FileHandle*, DWORD, DWORD, int size, DWORD, char* buffer);
typedef FileFinder*(__cdecl *RadsFindFirstFile_f)(char const* filename);
typedef int(__cdecl *RadsFindNextFile_f)(FileFinder*, int* isDirectory, int filesize[2], int maxLength, char* filename);
typedef int(__cdecl *RadsFindClose_f)(void*);


static bool g_showMessages = false;

void __cdecl Logger_GetFlags(int *flags, char const* prefix)
{
	if (g_showMessages)
	{
		std::wcout << "Logger_GetFlags(" << std::hex << *flags << ", " << prefix << ")" << std::endl;
	}

	*flags = 0;
}

void __cdecl Logger_Message(DWORD a, int flags, DWORD c, char* message, va_list args)
{
	if (!g_showMessages)
	{
		return;
	}

	char buffer[500];
	memset(buffer, 0, sizeof(buffer));
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	std::wcout << "Logger_Message(" << std::hex << a << ", " << flags << ", " << c << ", " << buffer << ")" << std::dec << std::endl;
}


int main(int argc, char* argv[])
{
	HMODULE radsio = LoadLibraryW(L"C:\\Riot Games\\League of Legends\\RADS\\RiotRadsIO.dll");
	if (!radsio)
	{
		std::wcerr << "Could not load RADS lib" << std::endl;
		return 1;
	}

	auto RadsInitialize = RadsInitialize_f(GetProcAddress(radsio, "RadsInitialize"));
	auto RadsSetLogger = RadsSetLogger_f(GetProcAddress(radsio, "RadsSetLogger"));
	auto RadsFileExists = RadsFileExists_f(GetProcAddress(radsio, "RadsFileExists"));
	auto RadsOpenFile = RadsOpenFile_f(GetProcAddress(radsio, "RadsOpenFile"));
	auto RadsCloseFile = RadsCloseFile_f(GetProcAddress(radsio, "RadsCloseFile"));
	auto RadsGetFileSize = RadsGetFileSize_f(GetProcAddress(radsio, "RadsGetFileSize"));
	auto RadsRead = RadsRead_f(GetProcAddress(radsio, "RadsRead"));
	auto RadsFindFirstFile = RadsFindFirstFile_f(GetProcAddress(radsio, "RadsFindFirstFile"));
	auto RadsFindNextFile = RadsFindNextFile_f(GetProcAddress(radsio, "RadsFindNextFile"));
	auto RadsFindClose = RadsFindClose_f(GetProcAddress(radsio, "RadsFindClose"));

	g_showMessages = false;

	Logger l;
	l.Logger_GetFlags = Logger_GetFlags;
	l.Logger_Message = Logger_Message;
	RadsSetLogger(&l);
	if (0 != RadsInitialize("LeagueOfLegends", 0x121, L"-rtcore-gdi-rgn-l1-1-1", "C:\\Riot Games\\League of Legends\\RADS\\solutions\\lol_game_client_sln\\releases\\0.0.1.110\\deploy"))
	{
		std::wcerr << "Could not initialize RADS lib" << std::endl;
		return 1;
	}
	g_showMessages = true;

	// Display a file's content
	if (true)
	{
		char *archiveFile = "DATA/CFG/defaults/Game.cfg";
		if (RadsFileExists(archiveFile))
		{
			FileHandle* fp = RadsOpenFile(archiveFile);
			int fileSize = RadsGetFileSize(fp);
			char* buffer = new char[fileSize + 1];
			RadsRead(fp, 0, 0, fileSize, 0, buffer);
			buffer[fileSize] = '\0';
			RadsCloseFile(fp);
			std::wcout << "File size: " << fileSize << std::endl << buffer << std::endl;
			delete[] buffer;
		}
	}

	// List all the files in the RADS folder
	if (false)
	{
		std::deque<char*> folders;
		char* root = new char[2] {'\0'};
		folders.push_back(root);
		while (!folders.empty())
		{
			char* currentDirectory = folders.front();
			int wildcardLen = strlen(currentDirectory) + 3;
			char* finderWildcard = new char[wildcardLen];
			strcpy_s(finderWildcard, wildcardLen, currentDirectory);
			strcat_s(finderWildcard, wildcardLen, "*");

			auto finder = RadsFindFirstFile(finderWildcard);
			int isDirectory;
			int filesize[2];
			char filename[_MAX_PATH];
			while (0 == RadsFindNextFile(finder, &isDirectory, filesize, _MAX_PATH, filename))
			{
				if (!isDirectory)
				{
					std::wcout << currentDirectory << filename << std::endl;
				}
				else
				{
					int len = strlen(currentDirectory) + strlen(filename) + 2;
					char *dirname = new char[len];
					strcpy_s(dirname, len, currentDirectory);
					strcat_s(dirname, len, filename);
					strcat_s(dirname, len, "/");
					folders.push_back(dirname);
				}
			}

			RadsFindClose(finder);
			folders.pop_front();
			delete[] finderWildcard;
			delete[] currentDirectory;
		}
	}

	return 0;
}

