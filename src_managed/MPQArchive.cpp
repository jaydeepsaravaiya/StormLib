
#include <iostream>	

#include "MPQArchive.h"
#include "MPQFile.h"
using namespace System::ComponentModel;
void ManagedStormLib::MPQArchive::CloseOpenFiles()
{
	IsDisposing = true;	//Preventing OpenFiles entries being removed from collection by MPQFile Event "Closed"

	for each (MPQFile ^ file in OpenFiles)
	{
		delete file;
	}

	OpenFiles->Clear();
	delete OpenFiles;
}

void ManagedStormLib::MPQArchive::RegisterCallbacks()
{
	{
		FileAddedNativeDelegate^ fp = gcnew FileAddedNativeDelegate(this, &MPQArchive::NativeFileAddedCallbackListener);
		GCHandle gch = GCHandle::Alloc(fp);
		IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
		SFileSetAddFileCallback(_pArchive, (SFILE_ADDFILE_CALLBACK)ip.ToPointer(), NULL);
		GCHandles->Add(gch);
	}

	{
		CompactNativeDelegate^ fp = gcnew CompactNativeDelegate(this, &MPQArchive::NativeCompactCallbackListener);
		GCHandle gch = GCHandle::Alloc(fp);
		IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
		SFileSetCompactCallback(_pArchive, (SFILE_COMPACT_CALLBACK)ip.ToPointer(), NULL);
		GCHandles->Add(gch);
	}

	{
		FileDownloadNativeDelegate^ fp = gcnew FileDownloadNativeDelegate(this, &MPQArchive::NativeDownloadCallbackListener);
		GCHandle gch = GCHandle::Alloc(fp);
		IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
		SFileSetDownloadCallback(_pArchive, (SFILE_DOWNLOAD_CALLBACK)ip.ToPointer(), NULL);
		GCHandles->Add(gch);
	}
}

void ManagedStormLib::MPQArchive::InitiateManagedMpqArchive()
{
	OpenFiles = gcnew List < MPQFile^ >;
	GCHandles = gcnew List<GCHandle>;
	RegisterCallbacks();
}

ManagedStormLib::MPQArchive::MPQArchive(TMPQArchive* native)
{
	if (native == nullptr)
		throw gcnew ArgumentException("Argument native cannot be null pointer.");
	FileInformation^ info = gcnew FileInformation(native, InfoClass::FileMpqFileName);
	_fileName = (String^)info->info;
	_pArchive = native;
	InitiateManagedMpqArchive();
}



void ManagedStormLib::MPQArchive::NativeFileAddedCallbackListener(void* pvUserData, unsigned long dwBytesWritten, unsigned long dwTotalBytes, bool bFinalCall)
{
	return FileAdded::raise(MPQUserData::FromNativePointer((TMPQUserData*)pvUserData), dwBytesWritten, dwTotalBytes, bFinalCall);
}

void ManagedStormLib::MPQArchive::NativeCompactCallbackListener(void* pvUserData, unsigned long dwWorkType, unsigned long long BytesProcessed, unsigned long long TotalBytes)
{
	return Compacting::raise(MPQUserData::FromNativePointer((TMPQUserData*)pvUserData), dwWorkType, BytesProcessed, TotalBytes);
}

void ManagedStormLib::MPQArchive::NativeDownloadCallbackListener(void* pvUserData, unsigned long long ByteOffset, unsigned long dwTotalBytes)
{
	return Download::raise(MPQUserData::FromNativePointer((TMPQUserData*)pvUserData), ByteOffset, dwTotalBytes);
}

ManagedStormLib::MPQArchive::MPQArchive(String^ archiveName, [Optional]Nullable<MPQOpenArchiveFlags> flags)
{
	if (!flags.HasValue) {// No Optional argument in CLI/C++
		flags = MPQOpenArchiveFlags::_BASE_PROVIDER_FILE;
	}
	_fileName = archiveName;
	Console::WriteLine("Creating Archive " + _fileName);
	HANDLE hMpq;
	if (SFileOpenArchive(ConvertToUnicodeString(archiveName), 0, (unsigned long)flags.Value, &hMpq))
	{
		_pArchive = (TMPQArchive*)hMpq;
		InitiateManagedMpqArchive();
	}
	else
	{
		throw gcnew Win32Exception(GetNativeLastError());
	}
}

ManagedStormLib::MPQArchive::~MPQArchive()
{
	for each (GCHandle handle in GCHandles)
	{
		handle.Free();
	}

	GCHandles->Clear();
	delete GCHandles;

	this->!MPQArchive();
}

ManagedStormLib::MPQArchive::!MPQArchive()
{
	CloseArchive();
	if (haPatch != nullptr) {
		delete haPatch;
		haPatch = nullptr;
	}
}

CultureInfo^ ManagedStormLib::MPQArchive::GetLocale()
{
	if (SFileGetLocale() == 0)
		return nullptr;// It means Locale is Neutral, but as Windows doesnt have Neutral Culture, its imitated by null
	return gcnew CultureInfo(SFileGetLocale());
}

unsigned long ManagedStormLib::MPQArchive::SetLocale(CultureInfo^ lcNewLocale)
{
	if (lcNewLocale == nullptr) {
		return SFileSetLocale(0);// Check MPQArchive::GetLocale for info.
	}
	return SFileSetLocale(lcNewLocale->LCID);
}

void ManagedStormLib::MPQArchive::OpenArchive(String^ MpqName, unsigned long Priority, MPQOpenArchiveFlags Flags, [Out] MPQArchive^% Mpq)
{
	HANDLE hMpq;
	auto result = SFileOpenArchive(ConvertToUnicodeString(MpqName), Priority, (unsigned long)Flags, &hMpq);
	if (result)
	{
		Mpq = gcnew MPQArchive((TMPQArchive*)hMpq);
	}
	else
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::CreateArchive(String^ MpqName, CreateArchiveFlags CreateFlags, unsigned long MaxFileCount, MPQArchive^% Mpq)
{
	HANDLE hMpq;
	auto result = SFileCreateArchive(ConvertToUnicodeString(MpqName), (unsigned long)CreateFlags, MaxFileCount, &hMpq);
	if (result)
	{
		Mpq = gcnew MPQArchive((TMPQArchive*)hMpq);
	}
	else
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::CreateArchive2(String^ MpqName, CreateMPQInfo CreateInfo, MPQArchive^% Mpq)
{
	HANDLE hMpq;
	pin_ptr<SFILE_CREATE_MPQ> pCreateInfo = &CreateInfo.ToNativeStructure();
	auto result = SFileCreateArchive2(ConvertToUnicodeString(MpqName), pCreateInfo, &hMpq);
	if (result)
	{
		Mpq = gcnew MPQArchive((TMPQArchive*)hMpq);

	}
	else
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::FlushArchive()
{
	if (!SFileFlushArchive(_pArchive)) {
		throw gcnew Win32Exception(GetNativeLastError());
	}
}

void ManagedStormLib::MPQArchive::CloseArchive()
{
	if (_pArchive != nullptr) {
		CloseOpenFiles();
		if (haBase == nullptr)//Base archive will close our unmanaged handle. so dont bother, plus it will throw access violation if same handle is tried to be freed twice
			if (!SFileCloseArchive(_pArchive))
				throw gcnew Win32Exception(GetNativeLastError());
	}
	_pArchive = nullptr;
}

void ManagedStormLib::MPQArchive::CreateFile(String^ szArchivedName, DateTime FileTime, unsigned long dwFileSize, CultureInfo^ Locale, AddFileFlags dwFlags, [Out] MPQFile^% phFile)
{
	HANDLE hFile;
	unsigned long lcLocale;
	if (Locale == nullptr)
	{
		lcLocale = 0;
	}
	else
	{
		lcLocale = Locale->LCID;
	}

	if (SFileCreateFile(_pArchive, ConvertToAnsiString(szArchivedName), FileTime.ToFileTime(), dwFileSize, lcLocale, (unsigned long)dwFlags, &hFile))
	{
		phFile = gcnew MPQFile(this, (TMPQFile*)hFile);
		OpenFiles->Add(phFile);
		phFile->Closed += gcnew System::Action<ManagedStormLib::MPQFile^>(this, &ManagedStormLib::MPQArchive::OnClosed);
	}
	else
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::AddFileEx(String^ szFileName, String^ szArchivedName, AddFileFlags dwFlags, CompressionType dwCompression, [Optional][DefaultParameterValue(CompressionType::NEXT_SAME)] CompressionType dwCompressionNext)
{
	if (!SFileAddFileEx(_pArchive, ConvertToUnicodeString(szFileName), ConvertToAnsiString(szArchivedName), (unsigned long)dwFlags, (unsigned long)dwCompression, (unsigned long)dwCompressionNext))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::AddFile(String^ szFileName, String^ szArchivedName, AddFileFlags dwFlags)
{
	if (!SFileAddFile(_pArchive, ConvertToUnicodeString(szFileName), ConvertToAnsiString(szArchivedName), (unsigned long)dwFlags))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::AddWave(String^ szFileName, String^ szArchivedName, AddFileFlags dwFlags, AddWaveQualityFlags dwQuality)
{
	if (!SFileAddWave(_pArchive, ConvertToUnicodeString(szFileName), ConvertToAnsiString(szArchivedName), (unsigned long)dwFlags, (unsigned long)dwQuality))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::RemoveFile(String^ szFileName, MpqFileSearchScope dwSearchScope)
{
	if (!SFileRemoveFile(_pArchive, ConvertToAnsiString(szFileName), (unsigned long)dwSearchScope))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::RenameFile(String^ szOldFileName, String^ szNewFileName)
{
	if (!SFileRenameFile(_pArchive, ConvertToAnsiString(szOldFileName), ConvertToAnsiString(szNewFileName)))
		throw gcnew Win32Exception(GetNativeLastError());
}

unsigned long ManagedStormLib::MPQArchive::AddListFile(String^ szListFile)
{
	return SFileAddListFile(_pArchive, ConvertToUnicodeString(szListFile));
}

void ManagedStormLib::MPQArchive::CompactArchive(String^ szListFile, bool bReserved)
{
	if (!SFileCompactArchive(_pArchive, ConvertToUnicodeString(szListFile), bReserved))
		throw gcnew Win32Exception(GetNativeLastError());
}


unsigned long ManagedStormLib::MPQArchive::GetMaxFileCount()
{
	return SFileGetMaxFileCount(_pArchive);
}

void ManagedStormLib::MPQArchive::SetMaxFileCount(unsigned long MaxFileCount)
{
	if (!SFileSetMaxFileCount(_pArchive, MaxFileCount))
		throw gcnew Win32Exception(GetNativeLastError());
}

ManagedStormLib::MPQAttributeFlags ManagedStormLib::MPQArchive::GetAttributes()
{
	return (MPQAttributeFlags)SFileGetAttributes(_pArchive);
}
void ManagedStormLib::MPQArchive::SetAttributes(MPQAttributeFlags Flags)
{
	if (!SFileSetAttributes(_pArchive, (unsigned long)Flags))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::UpdateFileAttributes(String^ FileName)
{
	if (!SFileUpdateFileAttributes(_pArchive, ConvertToAnsiString(FileName)))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::OpenPatchArchive(String^ szPatchMpqName, String^ szPatchPathPrefix, MPQOpenArchiveFlags dwFlags)
{
	if (SFileOpenPatchArchive(_pArchive, ConvertToUnicodeString(szPatchMpqName), ConvertToAnsiString(szPatchPathPrefix), (unsigned long)dwFlags)) {
		haPatch = gcnew MPQArchive(_pArchive->haPatch);
		haPatch->haBase = this;
	}
	else
		throw gcnew Win32Exception(GetNativeLastError());
}

bool ManagedStormLib::MPQArchive::IsPatchedArchive()
{
	return SFileIsPatchedArchive(_pArchive);
}

bool ManagedStormLib::MPQArchive::HasFile(String^ FileName)
{
	return SFileHasFile(_pArchive, ConvertToAnsiString(FileName));
}

void ManagedStormLib::MPQArchive::OpenFileEx(String^ FileName, MpqFileSearchScope SearchScope, [Out] MPQFile^% File)
{
	HANDLE hFile;
	bool result = SFileOpenFileEx(_pArchive, ConvertToAnsiString(FileName), (unsigned long)SearchScope, &hFile);
	if (result)
	{
		File = gcnew MPQFile(this, (TMPQFile*)hFile);

		OpenFiles->Add(File);
		File->Closed += gcnew System::Action<ManagedStormLib::MPQFile^>(this, &ManagedStormLib::MPQArchive::OnClosed);
	}
	else
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::GetFileInfo(InfoClass InfoClass, [Out] FileInformation^% pvFileInfo)
{
	pvFileInfo = gcnew FileInformation(_pArchive, InfoClass);
}

void ManagedStormLib::MPQArchive::ExtractFile(String^ szToExtract, String^ szExtracted, MpqFileSearchScope dwSearchScope)
{
	if (!SFileExtractFile(_pArchive, ConvertToAnsiString(szToExtract), ConvertToUnicodeString(szExtracted), (unsigned long)dwSearchScope))
		throw gcnew Win32Exception(GetNativeLastError());
}

void ManagedStormLib::MPQArchive::GetFileChecksums(String^ szFileName, unsigned long% Crc32, cli::array<byte>^ MD5)
{
	pin_ptr<byte> pMD5 = &MD5[0];
	pin_ptr<unsigned long> pdwCrc32 = &Crc32;
	if (!SFileGetFileChecksums(_pArchive, ConvertToAnsiString(szFileName), pdwCrc32, (char*)pMD5))
		throw gcnew Win32Exception(GetNativeLastError());
}

unsigned long ManagedStormLib::MPQArchive::VerifyFile(String^ szFileName, unsigned long dwFlags)
{
	return SFileVerifyFile(_pArchive, ConvertToAnsiString(szFileName), dwFlags);
}

unsigned long ManagedStormLib::MPQArchive::VerifyRawData(VerifyRawDataType dwWhatToVerify, String^ szFileName)
{
	return SFileVerifyRawData(_pArchive, (unsigned long)dwWhatToVerify, ConvertToAnsiString(szFileName));
}

void ManagedStormLib::MPQArchive::SignArchive(SignatureType dwSignatureType)
{
	if (!SFileSignArchive(_pArchive, (unsigned long)dwSignatureType))
		throw gcnew Win32Exception(GetNativeLastError());
}

unsigned long ManagedStormLib::MPQArchive::VerifyArchive()
{
	return SFileVerifyArchive(_pArchive);
}

unsigned long ManagedStormLib::MPQArchive::EnumLocales(String^ szFileName, cli::array< CultureInfo^ >^% Locales, MpqFileSearchScope dwSearchScope)

{
	/*pin_ptr<LCID> pLocales = &Locales[0];*/
	cli::array<LCID>^ LCIDs = gcnew cli::array<LCID>(64);
	pin_ptr<LCID> pLocales = &LCIDs[0];
	unsigned long MaxLocales = 64;
	unsigned long result = SFileEnumLocales(_pArchive, ConvertToAnsiString(szFileName), pLocales, &MaxLocales, (unsigned long)dwSearchScope);
	if (result == 0) {
		Locales = gcnew cli::array<CultureInfo^>(MaxLocales);
		for (size_t i = 0; i < MaxLocales; i++)
		{
			if (LCIDs[i] == 0)
				Locales[i] = nullptr;
			else
				Locales[i] = (gcnew CultureInfo(LCIDs[i]));
		}
	}
	return result;
}

ManagedStormLib::FileFinder^ ManagedStormLib::MPQArchive::FindFirstFile(String^ szMask, [Out] FoundFileData% lpFindFileData, String^ szListFile)
{
	SFILE_FIND_DATA data;
	HANDLE hFinder = SFileFindFirstFile(_pArchive, ConvertToAnsiString(szMask), &data, ConvertToUnicodeString(szListFile));
	lpFindFileData = FoundFileData::FromNativePointer(&data);
	return gcnew FileFinder(hFinder);
}

ManagedStormLib::ListFileFinder^ ManagedStormLib::MPQArchive::ListFileFindFirstFile(String^ szListFile, String^ szMask, [Out] FoundFileData% lpFindFileData, [Optional][DefaultParameterValue(true)] bool FindInInternalListFile)
{
	SFILE_FIND_DATA data;
	HANDLE hFinder;
	if (FindInInternalListFile) {

		hFinder = SListFileFindFirstFile(_pArchive, nullptr, ConvertToAnsiString(szMask), &data);
	}
	else {
		hFinder = SListFileFindFirstFile(nullptr, ConvertToUnicodeString(szListFile), ConvertToAnsiString(szMask), &data);

	}
	lpFindFileData = FoundFileData::FromNativePointer(&data);
	return gcnew ListFileFinder(hFinder);
}
void ManagedStormLib::MPQArchive::OnClosed(ManagedStormLib::MPQFile^ obj)
{
	if (!IsDisposing)
		OpenFiles->Remove(obj);
}