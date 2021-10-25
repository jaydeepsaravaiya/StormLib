#pragma once
#define STORMLIB_NO_AUTO_LINK 1


#include "StormLib.h"
#ifdef _DEBUG

#pragma comment(lib, "StormLibDUS.lib") // Debug Unicode CRT-LIB version
#else
#pragma comment(lib, "StormLibRUS.lib") // Release Unicode CRT-LIB version
#endif 
#undef CreateFile
#undef FindFirstFile
#undef FindNextFile
#include "ManagedTypes.h"
using namespace System;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;
using namespace System::Globalization;
using namespace System::Collections::Generic;
using namespace System::Runtime::CompilerServices;
#pragma unmanaged
DWORD GetNativeLastError();
#pragma managed
namespace ManagedStormLib {
	ref class MPQArchive;
	ref class MPQFile;

	public delegate void FileDownloadNativeDelegate(void* pvUserData, unsigned long long ByteOffset, unsigned long dwTotalBytes);//For when Archive callbacks
	public delegate void FileDownloadDelegate(MPQUserData pvUserData, unsigned long long ByteOffset, unsigned long dwTotalBytes);//Forwarding towards managed

	public delegate void FileAddedNativeDelegate(void* pvUserData, unsigned long dwBytesWritten, unsigned long dwTotalBytes, bool bFinalCall);
	public delegate void FileAddedDelegate(MPQUserData pvUserData, unsigned long dwBytesWritten, unsigned long dwTotalBytes, bool bFinalCall);

	public delegate void CompactNativeDelegate(void* pvUserData, unsigned long dwWorkType, unsigned long long BytesProcessed, unsigned long long TotalBytes);
	public delegate void CompactDelegate(MPQUserData pvUserData, unsigned long dwWorkType, unsigned long long BytesProcessed, unsigned long long TotalBytes);

	public delegate unsigned long HashString(String^ szFileName, unsigned long dwHashType);
	public delegate unsigned long NativeHashString(const char* szFileName, unsigned long dwHashType);

	const char* ConvertToAnsiString(String^ managedString);
	const TCHAR* ConvertToUnicodeString(String^ managedString);

	public ref class FileInformation {
	private:
		void* _pInfo;
	public:
		FileInformation(HANDLE _parent, InfoClass _class);
		~FileInformation();
		!FileInformation();
		InfoClass Class;
		[Dynamic]
		Object^ info;
		//File Info
		// Info classes for archives
		void FreeFileInfo();
	};
	public ref class FileFinder {
	private:
		HANDLE _hFinder;
	public:
		FileFinder(HANDLE hFinder);
		bool FindNextFile([Out] FoundFileData% lpFindFileData);
		bool Close();
	};
	public ref class ListFileFinder {
	private:
		HANDLE _hFinder;
	public:
		ListFileFinder(HANDLE hFinder);
		bool FindNextFile([Out] FoundFileData% lpFindFileData);
		bool Close();
	};
	public ref class Common {
	public:
		static Common();
		static bool SetDataCompression(DWORD DataCompression);
		static bool SetArchiveMarkers(PSFILE_MARKERS pMarkers);
	};


}