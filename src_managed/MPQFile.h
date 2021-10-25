#pragma once
#include "ManagedStormLib.h"

using namespace System::IO;
using namespace System::ComponentModel;
namespace ManagedStormLib {
	public ref class MPQFile : public Stream {
	private: TMPQFile* _pFile;
		   MPQFile^ _hfPatch;
	internal: event Action < MPQFile^ >^ Closed;
	public:
		//Stream Overrides
		property bool CanRead {
			bool get() override {
				return true;
			}
		};
		property bool CanSeek {
			bool get() override {
				return true;
			}
		};
		property bool CanWrite {
			bool get() override {
				return _pFile->bIsWriteHandle;
			}
		};
		property long long Length {
			long long get() override {
				BigInteger bi;
				bi.parts.lowValue = SFileGetFileSize(_pFile, (unsigned long*)&bi.parts.highValue);
				if (bi.parts.lowValue != -1) {
					return bi.value;
				}
				throw gcnew Win32Exception(GetNativeLastError());
			}
		}
		property long long Position {
			long long get() override {
				return _pFile->dwFilePos;
			}
			void set(long long value) override {
				BigInteger bi;
				bi.value = value;
				SetFilePointer(bi.parts.lowValue, bi.parts.highValue, SeekOrigin::Begin);
			}
		};
		void Flush() override;
		int Read(cli::array < byte >^ buffer, int offset, int count) override;
		long long Seek(long long offset, SeekOrigin origin) override;
		void SetLength(long long value) override;
		void Write(cli::array < byte >^ buffer, int offset, int count) override;
		//Stream Overrides End

		//Native Properties
		//TFileStream* pStream;                      // File stream. Only used on local files
		property MPQHash pHashEntry {
			MPQHash get() {
				return MPQHash::FromNativePointer(_pFile->pHashEntry);
			}
		}; // Pointer to hash table entry, if the file was open using hash table
		property FileEntry pFileEntry {
			FileEntry get() {
				return FileEntry::FromNativePointer(_pFile->pFileEntry);
			}
		}; // File entry for the file
		property unsigned long long RawFilePos {
			unsigned long long get() {
				return _pFile->RawFilePos;
			}
		}; // Offset in MPQ archive (relative to file begin)
		property unsigned long long MpqFilePos {
			unsigned long long get() {
				return _pFile->MpqFilePos;
			}
		}; // Offset in MPQ archive (relative to MPQ header)
		property unsigned long dwHashIndex {
			unsigned long get() {
				return _pFile->dwHashIndex;
			}
		}; // Hash table index (0xFFFFFFFF if not used)
		property unsigned long dwFileKey {
			unsigned long get() {
				return _pFile->dwFileKey;
			}
		}; // Decryption key
		property unsigned long dwFilePos {
			unsigned long get() {
				return _pFile->dwFilePos;
			}
		}; // Current file position
		property unsigned long dwMagic {
			unsigned long get() {
				return _pFile->dwMagic;
			}
		}; // 'FILE'

		property MPQFile^ hfPatch {
			MPQFile^ get() {
				if (_pFile->hfPatch == nullptr)
					return nullptr;
				else {
					if (_hfPatch == nullptr) {
						_hfPatch = gcnew MPQFile(ParentArchive, _pFile->hfPatch);
					}
					return _hfPatch;
				}
			}
		}; // Pointer to opened patch file

		property PatchInfo pPatchInfo {
			PatchInfo get() {
				return PatchInfo::FromNativePointer(_pFile->pPatchInfo);
			}
		}; // Patch info block, preceding the sector table
		property cli::array < unsigned long >^ SectorOffsets {
			cli::array < unsigned long >^ get() {
				if (_pFile->SectorOffsets == nullptr) {
					return nullptr;
				}
				cli::array < unsigned long >^ val = gcnew cli::array < unsigned long >(_pFile->dwSectorCount);
				for (size_t i = 0; i < _pFile->dwSectorCount; i++) {
					val[i] = *(_pFile->SectorOffsets + i);
				}
				return val;
			}
		}; // Position of each file sector, relative to the begin of the file. Only for compressed files.
		property cli::array < unsigned long >^ SectorChksums {
			cli::array < unsigned long >^ get() {
				if (_pFile->SectorChksums == nullptr) {
					return nullptr;
				}
				cli::array < unsigned long >^ val = gcnew cli::array < unsigned long >(_pFile->dwSectorCount);
				for (size_t i = 0; i < _pFile->dwSectorCount; i++) {
					val[i] = *(_pFile->SectorChksums + i);
				}
				return val;
			}
		}; // Array of sector checksums (either ADLER32 or MD5) values for each file sector
		property cli::array < byte >^ pbFileData {
			cli::array < byte >^ get() {
				if (_pFile->pbFileData == nullptr) {
					return nullptr;
				}
				cli::array < byte >^ val = gcnew cli::array < byte >(_pFile->cbFileData);
				for (size_t i = 0; i < _pFile->cbFileData; i++) {
					val[i] = *(_pFile->pbFileData + i);
				}
				return val;
			}
		}; // Data of the file (single unit files, patched files)
		property unsigned long cbFileData {
			unsigned long get() {
				return _pFile->cbFileData;
			}
		}; // Size of file data
		property CompressionType dwCompression0 {
			CompressionType get() {
				return (CompressionType)_pFile->dwCompression0;
			}
		}; // Compression that will be used on the first file sector
		property unsigned long dwSectorCount {
			unsigned long get() {
				return _pFile->dwSectorCount;
			}
		}; // Number of sectors in the file
		property unsigned long dwPatchedFileSize {
			unsigned long get() {
				return _pFile->dwPatchedFileSize;
			}
		}; // Size of patched file. Used when saving patch file to the MPQ
		property unsigned long dwDataSize {
			unsigned long get() {
				return _pFile->dwDataSize;
			}
		}; // Size of data in the file (on patch files, this differs from file size in block table entry)

		property cli::array < byte >^ pbFileSector; // Last loaded file sector. For single unit files, entire file content
		property unsigned long dwSectorOffs {
			unsigned long get() {
				return _pFile->dwSectorOffs;
			}
		}; // File position of currently loaded file sector
		property unsigned long dwSectorSize {
			unsigned long get() {
				return _pFile->dwSectorSize;
			}
		}; // Size of the file sector. For single unit files, this is equal to the file size

		property cli::array < unsigned char >^ hctx {
			cli::array < unsigned char >^ get() {
				cli::array < unsigned char >^ val = gcnew cli::array < unsigned char >(HASH_STATE_SIZE);
				for (size_t i = 0; i < HASH_STATE_SIZE; i++) {
					val[i] = _pFile->hctx[i];
				}
				return val;
			}

		}; // Hash state for MD5. Used when saving file to MPQ, size = [HASH_STATE_SIZE]
		property unsigned long dwCrc32 {
			unsigned long get() {
				return _pFile->dwCrc32;
			}
		}; // CRC32 value, used when saving file to MPQ

		property unsigned long dwAddFileError {
			unsigned long get() {
				return _pFile->dwAddFileError;
			}
		}; // Result of the "Add File" operations

		property bool bLoadedSectorCRCs {
			bool get() {
				return _pFile->bLoadedSectorCRCs;
			}
		}; // If true, we already tried to load sector CRCs
		property bool bCheckSectorCRCs {
			bool get() {
				return _pFile->bCheckSectorCRCs;
			}
		}; // If true, then SFileReadFile will check sector CRCs when reading the file
		property bool bIsWriteHandle {
			bool get() {
				return _pFile->bIsWriteHandle;
			}
		}; // If true, this handle has been created by SFileCreateFile

		//Native properties
		MPQFile(MPQArchive^ parent, TMPQFile* hFile); ~MPQFile(); !MPQFile();
		MPQArchive^ ParentArchive;
		bool SetFileLocale(CultureInfo^ lcNewLocale);
		//-----------------------------------------------------------------------------
		// Support for adding files to the MPQ
		void WriteFile(
			cli::array < byte >^ pvData, CompressionType dwCompression);
		bool FinishFile();
		unsigned long GetFileSize();
		unsigned long SetFilePointer(long lFilePos, [Out] long% plFilePosHigh, SeekOrigin dwMoveMethod);
		void ReadFile([Out] cli::array < byte >^% Buffer, unsigned long ToRead, [Out] unsigned long% Read, Nullable<NativeOverlapped> Overlapped);
		bool CloseFile();
		// Retrieving info about a file in the archive
		void GetFileInfo(InfoClass InfoClass, [Out] FileInformation^% pvFileInfo);
	};
}