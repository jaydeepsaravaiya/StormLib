
#include <iostream>
#include "MPQArchive.h"
#include "MPQFile.h"
using namespace std;
using namespace System::ComponentModel;
ManagedStormLib::MPQFile::MPQFile(MPQArchive^ parent, TMPQFile* hFile)
{
	ParentArchive = parent;
	_pFile = hFile;
}

ManagedStormLib::MPQFile::~MPQFile()
{
	std::cout << "Disposing File" << std::endl;
	Closed::raise(this);
	this->!MPQFile();
}

ManagedStormLib::MPQFile::!MPQFile()
{
	std::cout << "Finalizing File" << std::endl;
	if (_pFile != nullptr)
	{
		SFileCloseFile(_pFile);
		_pFile = nullptr;
	}
}

bool ManagedStormLib::MPQFile::SetFileLocale(CultureInfo^ NewLocale)
{
	unsigned long lcNewLocale;
	if (NewLocale == nullptr)
	{
		lcNewLocale = 0;
	}
	else
	{
		lcNewLocale = NewLocale->LCID;
	}

	return SFileSetFileLocale(_pFile, lcNewLocale);
}

void ManagedStormLib::MPQFile::WriteFile(cli::array<byte>^ buffer, CompressionType dwCompression)
{
	pin_ptr<byte> pvData = &buffer[0];
	_pFile->dwDataSize += buffer->Length;
	_pFile->pFileEntry->dwFileSize += buffer->Length;
	if (!SFileWriteFile(_pFile, pvData, buffer->Length, (unsigned long)dwCompression)) {

		//failed to write
		_pFile->pFileEntry->dwFileSize -= buffer->Length;
		_pFile->dwDataSize -= buffer->Length;
		throw gcnew Win32Exception(GetNativeLastError());
	}
}

bool ManagedStormLib::MPQFile::FinishFile()
{
	return SFileFinishFile(_pFile);
}

unsigned long ManagedStormLib::MPQFile::GetFileSize()
{
	return SFileGetFileSize(_pFile, NULL);
}

unsigned long ManagedStormLib::MPQFile::SetFilePointer(long lFilePos, [Out]long% FilePosHigh, SeekOrigin dwMoveMethod)
{
	pin_ptr<long> plFilePosHigh = &FilePosHigh;
	return SFileSetFilePointer(_pFile, lFilePos, plFilePosHigh, (unsigned long)dwMoveMethod);
}

bool ManagedStormLib::MPQFile::ReadFile([Out] cli::array<byte>^% Buffer, unsigned long ToRead, [Out] unsigned long% Read, NativeOverlapped Overlapped)
{
	Buffer = gcnew cli::array<byte>(_pFile->dwDataSize);
	pin_ptr<byte> lpBuffer = &Buffer[0];
	pin_ptr<NativeOverlapped> lpOverlapped = &Overlapped;
	pin_ptr < unsigned long > pdwRead = &Read;
	return SFileReadFile(_pFile, lpBuffer, ToRead, pdwRead, (LPOVERLAPPED)lpOverlapped);
}

bool ManagedStormLib::MPQFile::CloseFile()
{
	if (SFileCloseFile(_pFile))
	{
		_pFile = nullptr;
		return true;
	}

	return false;
}

void ManagedStormLib::MPQFile::GetFileInfo(InfoClass InfoClass, [Out]FileInformation^% pvFileInfo)
{
	pvFileInfo = gcnew FileInformation(_pFile, InfoClass);
}
