#include <Windows.h>
#include <iostream>

typedef void(__cdecl *Logger_GetFlags_f)(DWORD*, char const*);
typedef void(__cdecl *Logger_Message_f)(DWORD, DWORD, DWORD, char*, va_list);
struct Logger
{
	/* 0x00 */ char unknown1[0x44];
	/* 0x44 */ Logger_GetFlags_f Logger_GetFlags; // Function pointer 
	/* 0x48 */ char unknown2[0x1c];
	/* 0x64 */ Logger_Message_f Logger_Message; // Function pointer
};

typedef int(__cdecl *RadsInitialize_f)(char const*, DWORD, wchar_t const*, char const*);
typedef int(__cdecl *RadsSetLogger_f)(Logger*);


void __cdecl Logger_GetFlags(DWORD *flags, char const* prefix)
{
	std::wcout << "Logger_GetFlags(" << std::hex << *flags << ", " << prefix << ")" << std::endl;
	*flags = 0;
}

void __cdecl Logger_Message(DWORD a, DWORD flags, DWORD c, char* message, va_list args)
{
	char buffer[500];
	memset(buffer, 0, sizeof(buffer));
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	std::wcout << "Logger_Message(" << std::hex << a << ", " << flags << ", " << c << ", " << buffer << ")" << std::endl;
}


int main(int argc, char* argv[])
{
	HMODULE radsio = LoadLibraryW(L"C:\\Riot Games\\League of Legends\\RADS\\RiotRadsIO.dll");
	std::wcout << "Rads IO Lib: " << std::hex << radsio << std::endl;

	RadsInitialize_f RadsInitialize = RadsInitialize_f(GetProcAddress(radsio, "RadsInitialize"));
	RadsSetLogger_f RadsSetLogger = RadsSetLogger_f(GetProcAddress(radsio, "RadsSetLogger"));
	/*RadsFileExists = GetProcAddress(radsio, "RadsFileExists");
	RadsOpenFile = GetProcAddress(radsio, "RadsOpenFile");
	RadsCloseFile = GetProcAddress(radsio, "RadsCloseFile");
	RadsGetFileSize = GetProcAddress(radsio, "RadsGetFileSize");
	RadsRead = GetProcAddress(radsio, "RadsRead");
	RadsFindFirstFile = GetProcAddress(radsio, "RadsFindFirstFile");
	RadsFindNextFile = GetProcAddress(radsio, "RadsFindNextFile");
	RadsFindClose = GetProcAddress(radsio, "RadsFindClose");*/

	Logger l;
	l.Logger_GetFlags = Logger_GetFlags;
	l.Logger_Message = Logger_Message;
	RadsSetLogger(&l);

	RadsInitialize("LeagueOfLegends", 0x121, L"-rtcore-gdi-rgn-l1-1-1", "C:\\Riot Games\\League of Legends\\RADS\\solutions\\lol_game_client_sln\\releases\\0.0.1.109\\deploy");

	std::wcin.get();
	return 0;
}

