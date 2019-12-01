#include <Windows.h>
#include <stdio.h>

#include <string>

#include "sdk/class.h"
#include "hook/hook.h"

FILE* a = 0;
FILE* b = 0;
FILE* c = 0;

void SetupConsole()
{
	AllocConsole();
	a = freopen("CONOUT$", "wb", stdout);
	b = freopen("CONOUT$", "wb", stderr);
	c = freopen("CONIN$", "rb", stdin);
}


std::string get_dll_dir(HMODULE module) {
	char path[MAX_PATH];
	GetModuleFileNameA(module, path, sizeof(path));
	std::string buffer = std::string(path);
	size_t last_slash = buffer.find_last_of("\\");
	buffer = buffer.substr(0, last_slash+1);
	return buffer;
}

DWORD WINAPI main_thread(LPVOID param)
{
	HMODULE h_module = static_cast<HMODULE>(param);

	SetConsoleTitleA("DEBUG [x]");

	sdk::Initialize();
	Hook::Hook(FindWindowA(0,"Deceit"));
	while (true) {
		if (GetAsyncKeyState(VK_DELETE))
			break;
	}
	
	printf("unhooked \n");
	FreeLibraryAndExitThread(h_module, 0);
	return TRUE;
}


typedef LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);

void suspend()
{
	HANDLE processHandle = GetCurrentProcess();


	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle("ntdll"), "NtSuspendProcess");

	pfnNtSuspendProcess(processHandle);
	CloseHandle(processHandle);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		SetupConsole();
		CreateThread(0, 0, main_thread, hModule, 0, NULL);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Hook::UnHook();
		FreeConsole();
		break;
	}
	return TRUE;
}