#pragma once

#include "ManagedStormLib.h"
using namespace System;
using namespace System::Collections::Generic;
namespace ManagedStormLib {
	public ref class MPQArchive {
#pragma region Private
	private:
		String^ _fileName;
		TMPQArchive* _pArchive;
		bool IsDisposing = false;
		List<MPQFile^>^ OpenFiles;
		List<GCHandle>^ GCHandles;
		MPQArchive(TMPQArchive* native);

		void InitiateManagedMpqArchive();
		void RegisterCallbacks();
		void CloseOpenFiles();

		void NativeFileAddedCallbackListener(void* pvUserData, unsigned long dwBytesWritten, unsigned long dwTotalBytes, bool bFinalCall);//Pointer for Unmanaged code to call when this event occurs
		void NativeCompactCallbackListener(void* pvUserData, unsigned long dwWorkType, unsigned long long BytesProcessed, unsigned long long TotalBytes);//Pointer for Unmanaged code to call when this event occurs
		void NativeDownloadCallbackListener(void* pvUserData, unsigned long long ByteOffset, unsigned long dwTotalBytes);

		//property backing fields
		MPQArchive^ _haPatch;
		MPQArchive^ _haBase;
		//property backing fields end
#pragma endregion
	public:
#pragma region Ctor

		//Can only open
		MPQArchive(String^ archiveName, [Optional]Nullable<MPQOpenArchiveFlags> flags);
		~MPQArchive();
		!MPQArchive();
#pragma endregion
#pragma region Properties
		event FileAddedDelegate^ FileAdded; // Callback event when file gets added to created archive
		event CompactDelegate^ Compacting; // Callback event for compacting the archive
		event FileDownloadDelegate^ Download; // Callback event on download
		property unsigned long long UserDataPos {unsigned long long get() { return _pArchive->UserDataPos; }}; // Position of user data (relative to the begin of the file)
		property unsigned long long MpqPos {unsigned long long get() { return _pArchive->MpqPos; }}; // MPQ header offset (relative to the begin of the file)
		property unsigned long long FileSize {unsigned long long get() { return _pArchive->FileSize; }}; // Size of the file at the moment of file open
		property unsigned long dwHETBlockSize {unsigned long get() { return _pArchive->dwHETBlockSize; }};
		property unsigned long dwBETBlockSize {unsigned long get() { return _pArchive->dwBETBlockSize; }};
		property unsigned long dwMaxFileCount {unsigned long get() { return _pArchive->dwMaxFileCount; }}; // Maximum number of files in the MPQ. Also total size of the file table.
		property unsigned long dwFileTableSize {unsigned long get() { return _pArchive->dwFileTableSize; }}; // Current size of the file table, e.g. index of the entry past the last occupied one
		property unsigned long dwReservedFiles {unsigned long get() { return _pArchive->dwReservedFiles; }}; // Number of entries reserved for internal MPQ files (listfile, attributes)
		property unsigned long dwSectorSize {unsigned long get() { return _pArchive->dwSectorSize; }}; // Default size of one file sector
		property unsigned long dwSubType {unsigned long get() { return _pArchive->dwSubType; }}; // See MPQ_SUBTYPE_XXX

		property MPQArchive^ haPatch /*{MPQArchive^ get() {
			bool shouldCreate = false;
			if (_pArchive->haPatch == nullptr)
			{
				if (_haPatch != nullptr) {
					delete _haPatch;
					_haPatch = nullptr;
				}
			}
			else {
				if (_haPatch != nullptr) {
					if (_haPatch->_pArchive != _pArchive->haPatch) {
						delete _haPatch;
						shouldCreate = true;
					}
				}
				shouldCreate = true;
			}
			_haPatch = shouldCreate ? gcnew MPQArchive(_pArchive->haPatch) : _haPatch;
			return _haPatch;
		}
		}*/; // Pointer tWo patch archive, if any
		property MPQArchive^ haBase /*{MPQArchive^ get() {
			bool shouldCreate = false;
			if (_pArchive->haBase == nullptr)
			{
				if (_haBase != nullptr) {
					delete _haBase;
					_haBase = nullptr;
				}
			}
			else {
				if (_haBase != nullptr) {
					if (_haBase->_pArchive != _pArchive->haBase) {
						delete _haBase;
						shouldCreate = true;
					}
				}
				shouldCreate = true;
			}
			_haBase = shouldCreate ? gcnew MPQArchive(_pArchive->haBase) : _haBase;
			return _haBase;
		}}*/; // Pointer to base ("previous version") archive, if any

		property NamePrefix pPatchPrefix {NamePrefix get() { return NamePrefix::FromNativePointer(_pArchive->pPatchPrefix); }}; // Patch prefix to precede names of patch files

		property MPQUserData pUserData {MPQUserData get() { return MPQUserData::FromNativePointer(_pArchive->pUserData); }}; // MPQ user data (NULL if not present in the file)
		property MPQUserData pvAddFileUserData {MPQUserData get() {
			return MPQUserData::FromNativePointer((TMPQUserData*)_pArchive->pvAddFileUserData);
		}}; // User data thats passed to the callback
		property MPQUserData pvCompactUserData {MPQUserData get() { return MPQUserData::FromNativePointer((TMPQUserData*)_pArchive->pvCompactUserData); }}; // User data thats passed to the callback
		property MPQUserData UserData {MPQUserData get() { return MPQUserData::FromNativeStructure(_pArchive->UserData); }}; // MPQ user data. Valid only when ID_MPQ_USERDATA has been found

		property Header pHeader {Header get() { return Header::FromNativePointer(_pArchive->pHeader); }}; // MPQ file header

		property HetTable pHetTable {HetTable get() { return HetTable::FromNativePointer(_pArchive->pHetTable); }}; // HET table

		property HashString^ pfnHashString {HashString^ get() {
			return (HashString^)Marshal::GetDelegateForFunctionPointer(IntPtr(_pArchive->pfnHashString), HashString::typeid);
		} };               // Hashing function that will convert the file name into hash

		property cli::array < MPQHash >^ pHashTable {
			cli::array<MPQHash>^ get() {
				cli::array<MPQHash>^ value = gcnew cli::array<MPQHash>(pHeader.dwHashTableSize);
				for (size_t i = 0; i < pHeader.dwHashTableSize; i++)
				{
					value[i] = MPQHash::FromNativePointer((TMPQHash*)_pArchive->pHashTable + i);
				}
				return value;
			}
		}; // Hash table
		property cli::array < FileEntry >^ pFileTable {
			cli::array < FileEntry >^ get() {

				cli::array<FileEntry>^ value = gcnew cli::array<FileEntry>(pHeader.dwBlockTableSize);
				for (size_t i = 0; i < pHeader.dwBlockTableSize; i++)
				{
					value[i] = FileEntry::FromNativePointer((TFileEntry*)_pArchive->pFileTable + i);
				}
				return value;
			}
		}; // File table
		property cli::array < unsigned long >^ HeaderData {
			cli::array < unsigned long >^ get() {
				cli::array < unsigned long >^ value = gcnew cli::array < unsigned long >(MPQ_HEADER_DWORDS);
				for (size_t i = 0; i < MPQ_HEADER_DWORDS; i++)
				{
					value[i] = _pArchive->HeaderData[i];
				}
				return value;
			}
		}; // Storage for MPQ header

		property MPQFlags dwFlags { MPQFlags get() {
			return (MPQFlags)_pArchive->dwFlags;
		} }; // See MPQ_FLAG_XXXXX

		property MPQFileFlags dwFileFlags1 { MPQFileFlags get() {
			return (MPQFileFlags)_pArchive->dwFileFlags1;
		} }; // Flags for (listfile)
		property MPQFileFlags dwFileFlags2 { MPQFileFlags get() {
			return (MPQFileFlags)_pArchive->dwFileFlags2;
		} }; // Flags for (attributes)
		property MPQFileFlags dwFileFlags3 { MPQFileFlags get() {
			return (MPQFileFlags)_pArchive->dwFileFlags3;
		} }; // Flags for (signature)
		property MPQAttributeFlags dwAttrFlags { MPQAttributeFlags get() {
			return (MPQAttributeFlags)_pArchive->dwAttrFlags;
		} }; // Flags for the (attributes) file, see MPQ_ATTRIBUTE_XXX

		property unsigned long long CompactBytesProcessed { unsigned long long get() { return _pArchive->CompactBytesProcessed; } }; // Amount of bytes that have been processed during a particular compact call
		property unsigned long long CompactTotalBytes { unsigned long long get() { return _pArchive->CompactTotalBytes; } }; // Total amount of bytes to be compacted

#pragma endregion
#pragma region Functions for Archive Manipulation (Open/Create/Update)
		//-----------------------------------------------------------------------------
		// Functions for archive manipulation
		static void OpenArchive(String^ MpqName, unsigned long Priority, MPQOpenArchiveFlags Flags, [Out] MPQArchive^% Mpq);
		static void CreateArchive(String^ MpqName, CreateArchiveFlags CreateFlags, unsigned long MaxFileCount, [Out] MPQArchive^% Mpq);
		static void CreateArchive2(String^ MpqName, CreateMPQInfo pCreateInfo, [Out] MPQArchive^% Mpq);

		void FlushArchive();
		void CloseArchive();

#pragma endregion
#pragma region Not Implemented due to already covered with other mechanism
		//bool SetAddFileCallback( SFILE_ADDFILE_CALLBACK AddFileCB, void* pvUserData); // Covered with Event
		//bool SetCompactCallback( SFILE_COMPACT_CALLBACK CompactCB, void* pvUserData); // Covered with Event
		//bool SetDownloadCallback( SFILE_DOWNLOAD_CALLBACK DownloadCB, void* pvUserData); //Covered with Download event
#pragma endregion
#pragma region File Addition and Modifying
		//-----------------------------------------------------------------------------
		// Support for adding files to the MPQ

		void CreateFile(String^ szArchivedName, DateTime FileTime, unsigned long dwFileSize, CultureInfo^ lcLocale, AddFileFlags dwFlags, [Out] MPQFile^% phFile);
		void AddFileEx(String^ szFileName, String^ szArchivedName, AddFileFlags dwFlags, CompressionType dwCompression, [Optional][DefaultParameterValue(CompressionType::NEXT_SAME)] CompressionType dwCompressionNext);
		void AddFile(String^ szFileName, String^ szArchivedName, AddFileFlags dwFlags);
		void AddWave(String^ szFileName, String^ szArchivedName, AddFileFlags dwFlags, AddWaveQualityFlags dwQuality);
		void RemoveFile(String^ szFileName, MpqFileSearchScope dwSearchScope);
		void RenameFile(String^ szOldFileName, String^ szNewFileName);

		// Adds another listfile into MPQ. The currently added listfile(s) remain,
		// so you can use this API to combining more listfiles.
		// Note that this function is internally called by SFileFindFirstFile
		unsigned long AddListFile(String^ szListFilePath);
#pragma endregion
#pragma region Utility
		// Archive compacting
		void CompactArchive(String^ szListFile, bool bReserved);
		// Changing the maximum file count
		unsigned long GetMaxFileCount();
		void SetMaxFileCount(unsigned long MaxFileCount);

		// Changing (attributes) file
		MPQAttributeFlags GetAttributes();
		void SetAttributes(MPQAttributeFlags Flags);
		void UpdateFileAttributes(String^ FileName);

#pragma endregion
#pragma region Patch Archives
		//-----------------------------------------------------------------------------
		// Functions for manipulation with patch archives

		void OpenPatchArchive(String^ szPatchMpqName, String^ szPatchPathPrefix, MPQOpenArchiveFlags dwFlags);
		bool IsPatchedArchive();

#pragma endregion
#pragma region File Manipulation
		//-----------------------------------------------------------------------------
		// Functions for file manipulation

		// Reading from MPQ file
		bool HasFile(String^ FileName);
		void OpenFileEx(String^ FileName, MpqFileSearchScope SearchScope, [Out] MPQFile^% File);
		void GetFileInfo(InfoClass InfoClass, [Out] FileInformation^% pvFileInfo);
		// High-level extract function
		void ExtractFile(
			String^ szToExtract,
			String^ szExtracted, MpqFileSearchScope dwSearchScope);
#pragma endregion
#pragma region Verification
		//-----------------------------------------------------------------------------
		// Functions for file and archive verification

		// Generates file CRC32
		void GetFileChecksums(
			String^ szFileName, [Out] unsigned long% pdwCrc32, [Out] cli::array<byte>^ pMD5);

		// Verifies file against its checksums stored in (attributes) attributes (depending on dwFlags).
		// For dwFlags, use one or more of MPQ_ATTRIBUTE_MD5
		unsigned long VerifyFile(
			String^ szFileName, unsigned long dwFlags);

		// Verifies raw data of the archive. Only works for MPQs version 4 or newer
		unsigned long VerifyRawData(VerifyRawDataType dwWhatToVerify, String^ szFileName);

		// Verifies the signature, if present
		void SignArchive(SignatureType dwSignatureType);
		unsigned long VerifyArchive();
#pragma endregion
#pragma region Locale
		// Locale support

		// Call before SFileOpenFileEx
		CultureInfo^ GetLocale();
		unsigned long SetLocale(CultureInfo^ lcNewLocale);

		unsigned long EnumLocales(
			String^ szFileName, [Out] cli::array < CultureInfo^ >^% plcLocales, MpqFileSearchScope dwSearchScope);
#pragma endregion
#pragma region File Searching

		// Functions for file searching

		FileFinder^ FindFirstFile(
			String^ szMask, [Out] FoundFileData% lpFindFileData,
			String^ szListFile);
		ListFileFinder^ ListFileFindFirstFile(
			String^ szListFile,
			String^ szMask, [Out] FoundFileData% lpFindFileData, [Optional][DefaultParameterValue(true)] bool FindInInternalListFile);
#pragma endregion
#pragma region Managed Code Specific
		//Callback when Opened File from this archive gets closed
		void OnClosed(ManagedStormLib::MPQFile^ obj);

#pragma endregion
	};
}