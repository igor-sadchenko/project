#include "stdafx.h"
#include "System.h"

#include <tlhelp32.h>
#include <psapi.h>

namespace manager {

	System::System()
	{

	}

	System::~System()
	{

	}



	/*
* 在远程进程中搜索模块句柄
*/
	HMODULE FindRemoteMod(HANDLE hProcess, const wchar_t* pszModShortName)
	{
		HMODULE _hModules[0x100] = { 0 };
		int _cbNeeded = 0;
		if (!EnumProcessModules(hProcess
			, _hModules
			, sizeof(_hModules)
			, (unsigned long*)&_cbNeeded))
			return (HMODULE)0;

		for (int _i = 0; _i < (_cbNeeded >> 2); _i++)
		{
			wchar_t _szModuleName[0x200] = { 0 };
			GetModuleBaseNameW(hProcess
				, _hModules[_i]
				, _szModuleName
				, sizeof(_szModuleName) / sizeof(_szModuleName[0]));

			if (!wcsicmp(_szModuleName, pszModShortName))
				return _hModules[_i];
		}
		return (HMODULE)0;
	}

	/*
* 加载dll到远程进程
*/
	BOOL LoadDll(HANDLE hProcess, const wchar_t* dllpath)
	{
		// 计算路径长度，分配远程进程空间内虚拟内存
		SIZE_T dwRemoteBufLength = (wcslen(dllpath) + 1) * sizeof(wchar_t);

		// 在目标进程中分配一点空间来存放LoadLibraryW的参数     
		wchar_t* _pBuf = (wchar_t*)VirtualAllocEx(hProcess
			, NULL
			, dwRemoteBufLength
			, MEM_COMMIT
			, PAGE_READWRITE);
		if (!_pBuf)
			return FALSE;

		// 把dll的全路径写入到远程进程地址空间
		if (!(WriteProcessMemory(hProcess
			, _pBuf
			, (PVOID)dllpath
			, dwRemoteBufLength
			, NULL)))
			return FALSE;

		// 以LoadLibraryW为线程体来创建远程线程
		HANDLE _hThread = CreateRemoteThread(hProcess
			, NULL
			, 0
			, (PTHREAD_START_ROUTINE)(LoadLibraryW)
			, (PVOID)_pBuf
			, 0
			, NULL);
		// 内存泄漏?! 
		return (_hThread != INVALID_HANDLE_VALUE);
	}

	BOOL System::UnLoadDllAt(const sk::string& processName, const sk::string& moduleName) const
	{
		sk::string tmpstr = _TEXT("");
		PROCESSENTRY32 pinfo;
		pinfo.dwSize = sizeof(pinfo);
		DWORD dw_pid = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == NULL)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("CreateToolhelp32Snapshot Failed({}).", GetLastError());
			return FALSE;
		}
		bool flag = Process32First(snapshot, &pinfo);
		while (flag)
		{
			tmpstr = sk::string(pinfo.szExeFile);
			if (tmpstr == processName)
			{
				dw_pid = pinfo.th32ProcessID;
				Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("Find target process PID({}).", dw_pid);
				break;
			}
			flag = Process32Next(snapshot, &pinfo);
		}
		CloseHandle(snapshot);

		if (dw_pid == 0)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", "Target process not found.");
			return FALSE;
		}

		// 打开进程，具备写入权限和创建线程权限 
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ
			| PROCESS_VM_WRITE
			| PROCESS_VM_OPERATION
			| PROCESS_SUSPEND_RESUME
			| PROCESS_CREATE_THREAD
			| PROCESS_QUERY_INFORMATION
			, FALSE
			, dw_pid);
		if (NULL == hProcess)
			return FALSE;

		// 检查是否是同一个用户会话中的进程
		HANDLE _hToken = INVALID_HANDLE_VALUE;
		if (!OpenProcessToken(hProcess, TOKEN_READ, &_hToken))
		{
			CloseHandle(hProcess);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("OpenProcessToken Failed({}).", GetLastError());
			return FALSE;
		}
		CloseHandle(_hToken);

		HMODULE _hDllModule = FindRemoteMod(hProcess, moduleName.c_str());
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("Find target process module address({:X}).", TypeToPtr(_hDllModule));
		if (_hDllModule != 0)
		{
			// 以FreeLibrary为线程体创建线程，卸载目标进程中的特定DLL. 
			unsigned long _tid = 0;
			CreateRemoteThread(hProcess
				, 0, 0
				, (LPTHREAD_START_ROUTINE)FreeLibrary
				, _hDllModule
				, 0
				, &_tid);
		}
		CloseHandle(hProcess);
		hProcess = nullptr;
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}", "处理完成!");
		//FreeLibraryAndExitThread()
		return TRUE;
#if 0
		sk::string tmpstr = _TEXT("");
		PROCESSENTRY32 pinfo;
		pinfo.dwSize = sizeof(pinfo);
		DWORD dw_pid = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == NULL)
			return FALSE;
		bool flag = Process32First(snapshot, &pinfo);
		while (flag)
		{
			tmpstr = sk::string(pinfo.szExeFile);
			if (tmpstr == processName)
			{
				dw_pid = pinfo.th32ProcessID;
				Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("Target process ID({}).", dw_pid);
				break;
			}
			flag = Process32Next(snapshot, &pinfo);
		}
		CloseHandle(snapshot);

		if (dw_pid == 0)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->info("{}", "Process not found");
			return FALSE;
		}

		DWORD dwHandle = 0;
		HANDLE hProcess1 = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dw_pid);
		if (!hProcess1)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("OpenProcess failed({}).", GetLastError());
			return FALSE;
		}

		// 向目标进程地址空间写入DLL名称
		SIZE_T dwSize, dwWritten;
		dwSize = moduleName.length() + 1;
		LPVOID lpBuf = VirtualAllocEx(hProcess1, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);

		if (!WriteProcessMemory(hProcess1, lpBuf, (LPVOID)moduleName.c_str(), dwSize, &dwWritten))
		{
			VirtualFreeEx(hProcess1, lpBuf, dwSize, MEM_DECOMMIT);
			CloseHandle(hProcess1);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("WriteProcessMemory failed({}).", GetLastError());
			return FALSE;
		}

		LPVOID pFun = GetProcAddress(GetModuleHandleA("Kernel32"), "GetModuleHandleA");//GetModuleHandleA;
		HANDLE hThread = CreateRemoteThread(hProcess1, NULL, 0, (LPTHREAD_START_ROUTINE)pFun,lpBuf, 0, NULL);
		if (hThread == NULL)
		{
			CloseHandle(hProcess1);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("CreateRemoteThread failed({}).", GetLastError());
			return FALSE;
		}
		// 等待GetModuleHandle运行完毕
		WaitForSingleObject(hThread, INFINITE);
		// 获得GetModuleHandle的返回值
		GetExitCodeThread(hThread, &dwHandle);

		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("Return target module handle({}).", dwHandle);
		// 释放目标进程中申请的空间
		VirtualFreeEx(hProcess1, lpBuf, dwSize, MEM_DECOMMIT);
		CloseHandle(hThread);
		// 使目标进程调用FreeLibrary，卸载DLL
		pFun = GetProcAddress(GetModuleHandleA("Kernel32"), "FreeLibraryAndExitThread");
		hThread = CreateRemoteThread(hProcess1, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, (LPVOID)dwHandle, 0, NULL);
		// 等待FreeLibrary卸载完毕
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		CloseHandle(hProcess1);
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}","process success.");
#endif






		return TRUE;
	}
}///namespace manager