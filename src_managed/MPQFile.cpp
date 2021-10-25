
#include <iostream>
#include "MPQArchive.h"
#include "MPQFile.h"
using namespace std;
using namespace System::ComponentModel;
void ManagedStormLib::MPQFile::Flush()
{
	FinishFile();
}

int ManagedStormLib::MPQFile::Read(cli::array<byte>^ buffer, int offset, int count)
{

	Seek(_pFile->dwFilePos + offset, SeekOrigin::Begin);
	pin_ptr<byte> lpBuffer = &buffer[0];
	unsigned long readBytes;
	bool result = SFileReadFile(_pFile, lpBuffer, count, &readBytes, nullptr);
	if (result || GetNativeLastError() == ERROR_HANDLE_EOF) {
		return readBytes;
	}
	else
		throw gcnew Win32Exception(GetNativeLastError());

}
long long ManagedStormLib::MPQFile::Seek(long long  offset, SeekOrigin origin)
{
	BigInteger bi;
	bi.value = offset;
	long filePosLow = SFileSetFilePointer(_pFile, bi.parts.lowValue, &bi.parts.highValue, (unsigned long)origin);
	if (filePosLow != -1) {
		return bi.value;
	}
	throw gcnew Win32Exception(GetNativeLastError());
}
void ManagedStormLib::MPQFile::SetLength(long long  value)
{
	_pFile->dwDataSize = value;
	_pFile->pFileEntry->dwFileSize = value;
	//throw gcnew System::NotSupportedException();
}
void ManagedStormLib::MPQFile::Write(cli::array<byte>^ buffer, int offset, int count)
{
	//if (_pFile->dwDataSize < _pFile->dwFilePos + offset + count)
	SetLength(max(_pFile->dwDataSize, _pFile->dwFilePos + offset + count));
	Seek(_pFile->dwFilePos + offset, SeekOrigin::Begin);
	pin_ptr<byte> lpBuffer = &buffer[0];
	//unsigned long readBytes;
	if (!SFileWriteFile(_pFile, lpBuffer, count, _pFile->dwCompression0))
		throw gcnew Win32Exception(GetNativeLastError());
}
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
	unsigned long previousDataSize = _pFile->dwDataSize;
	SetLength(buffer->Length);
	if (!SFileWriteFile(_pFile, pvData, buffer->Length, (unsigned long)dwCompression)) {
		//failed to write
		SetLength(previousDataSize);
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

void ManagedStormLib::MPQFile::ReadFile([Out] cli::array<byte>^% Buffer, unsigned long ToRead, [Out] unsigned long% readBytes, Nullable<NativeOverlapped> Overlapped)
{
	Buffer = gcnew cli::array<byte>(ToRead);
	readBytes = Read(Buffer, 0, ToRead);
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
