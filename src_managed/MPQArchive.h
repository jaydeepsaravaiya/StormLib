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
		void PopulateFieldsFromNativeStructure(TMPQArchive* native);
		void CloseOpenFiles();

		void NativeFileAddedCallbackListener(void* pvUserData, unsigned long dwBytesWritten, unsigned long dwTotalBytes, bool bFinalCall);//Pointer for Unmanaged code to call when this event occurs
		void NativeCompactCallbackListener(void* pvUserData, unsigned long dwWorkType, unsigned long long BytesProcessed, unsigned long long TotalBytes);//Pointer for Unmanaged code to call when this event occurs
		void NativeDownloadCallbackListener(void* pvUserData, unsigned long long ByteOffset, unsigned long dwTotalBytes);
#pragma endregion
	public:
#pragma region Ctor

		//Can only open
		MPQArchive(String^ archiveName, [Optional][DefaultParameterValue(MPQOpenArchiveFlags::READ_ONLY)]MPQOpenArchiveFlags flags);
		~MPQArchive();
		!MPQArchive();
#pragma endregion
#pragma region Properties
		event FileAddedDelegate^ FileAdded; // Callback event when file gets added to created archive
		event CompactDelegate^ Compacting; // Callback event for compacting the archive
		event FileDownloadDelegate^ Download; // Callback event on download

		unsigned long long UserDataPos; // Position of user data (relative to the begin of the file)
		unsigned long long MpqPos; // MPQ header offset (relative to the begin of the file)
		unsigned long long FileSize; // Size of the file at the moment of file open
		unsigned long dwHETBlockSize;
		unsigned long dwBETBlockSize;
		unsigned long dwMaxFileCount; // Maximum number of files in the MPQ. Also total size of the file table.
		unsigned long dwFileTableSize; // Current size of the file table, e.g. index of the entry past the last occupied one
		unsigned long dwReservedFiles; // Number of entries reserved for internal MPQ files (listfile, attributes)
		unsigned long dwSectorSize; // Default size of one file sector
		unsigned long dwSubType; // See MPQ_SUBTYPE_XXX

		MPQArchive^ haPatch; // Pointer tWo patch archive, if any
		MPQArchive^ haBase; // Pointer to base ("previous version") archive, if any

		NamePrefix pPatchPrefix; // Patch prefix to precede names of patch files

		UserData pUserData; // MPQ user data (NULL if not present in the file)
		UserData pvAddFileUserData; // User data thats passed to the callback
		UserData pvCompactUserData; // User data thats passed to the callback
		UserData MpqUserData; // MPQ user data. Valid only when ID_MPQ_USERDATA has been found

		Header pHeader; // MPQ file header

		HetTable pHetTable; // HET table

		HashString^ pfnHashString;               // Hashing function that will convert the file name into hash


		cli::array < MPQHash >^ pHashTable; // Hash table
		cli::array < FileEntry >^ pFileTable; // File table
		cli::array < unsigned long >^ HeaderData; // Storage for MPQ header

		MPQFlags dwFlags; // See MPQ_FLAG_XXXXX

		MPQFileFlags dwFileFlags1; // Flags for (listfile)
		MPQFileFlags dwFileFlags2; // Flags for (attributes)
		MPQFileFlags dwFileFlags3; // Flags for (signature)
		MPQAttributeFlags dwAttrFlags; // Flags for the (attributes) file, see MPQ_ATTRIBUTE_XXX

		unsigned long long CompactBytesProcessed; // Amount of bytes that have been processed during a particular compact call
		unsigned long long CompactTotalBytes; // Total amount of bytes to be compacted
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
		unsigned long GetAttributes();
		void SetAttributes(MPQAttributeFlags Flags);
		void UpdateFileAttributes(String^ FileName);

#pragma endregion
#pragma region Patch Archives
		//-----------------------------------------------------------------------------
		// Functions for manipulation with patch archives

		void OpenPatchArchive(String^ szPatchMpqName, String^ szPatchPathPrefix, unsigned long dwFlags);
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