#pragma once
#include "StormLib.h"
using namespace System;
using namespace System::Diagnostics;
using namespace System::Globalization;
namespace ManagedStormLib {
	[Flags]
	public enum class MPQFlags : unsigned long {
		// Flags for TMPQArchive::dwFlags. Used internally
		READ_ONLY = 0x00000001, // If set, the MPQ has been open for read-only access
		CHANGED = 0x00000002, // If set, the MPQ tables have been changed
		MALFORMED = 0x00000004, // Malformed data structure detected (W3M map protectors)
		HASH_TABLE_CUT = 0x00000008, // The hash table goes beyond EOF
		BLOCK_TABLE_CUT = 0x00000010, // The hash table goes beyond EOF
		CHECK_SECTOR_CRC = 0x00000020, // Checking sector CRC when reading files
		SAVING_TABLES = 0x00000040, // If set, we are saving MPQ internal files and MPQ tables
		PATCH = 0x00000080, // If set, this MPQ is a patch archive
		WAR3_MAP = 0x00000100, // If set, this MPQ is a map for Warcraft III
		LISTFILE_NONE = 0x00000200, // Set when no (listfile) was found in InvalidateInternalFiles
		LISTFILE_NEW = 0x00000400, // Set when (listfile) invalidated by InvalidateInternalFiles
		LISTFILE_FORCE = 0x00000800, // Save updated listfile on exit
		ATTRIBUTES_NONE = 0x00001000, // Set when no (attributes) was found in InvalidateInternalFiles
		ATTRIBUTES_NEW = 0x00002000, // Set when (attributes) invalidated by InvalidateInternalFiles
		SIGNATURE_NONE = 0x00004000, // Set when no (signature) was found in InvalidateInternalFiles
		SIGNATURE_NEW = 0x00008000, // Set when (signature) invalidated by InvalidateInternalFiles
	};
	[Flags]
	public enum class MPQFileFlags : unsigned long {
		// Flags for SFileAddFile
		NONE = 0x00000000, // None
		IMPLODE = 0x00000100, // Implode method (By PKWARE Data Compression Library)
		COMPRESS = 0x00000200, // Compress methods (By multiple methods)
		ENCRYPTED = 0x00010000, // Indicates whether file is encrypted
		FIX_KEY = 0x00020000, // File decryption key has to be fixed
		PATCH_FILE = 0x00100000, // The file is a patch file. Raw file data begin with TPatchInfo structure
		SINGLE_UNIT = 0x01000000, // File is stored as a single unit, rather than split into sectors (Thx, Quantam)
		DELETE_MARKER = 0x02000000, // File is a deletion marker. Used in MPQ patches, indicating that the file no longer exists.
		SECTOR_CRC = 0x04000000, // File has checksums for each sector.
		// Ignored if file is not compressed or imploded.
		SIGNATURE = 0x10000000, // Present on STANDARD.SNP\(signature). The only occurence ever observed
		EXISTS = 0x80000000, // Set if file exists, reset when the file was deleted
		REPLACEEXISTING = 0x80000000, // Replace when the file exist (SFileAddFile)

		COMPRESS_MASK = 0x0000FF00, // Mask for a file being compressed

		DEFAULT_INTERNAL = 0xFFFFFFFF, // Use default flags for internal files
	};
	[Flags]
	public enum class MPQAttributeFlags : unsigned long {
		// Flags for MPQ attributes
		NONE = 0x00000000, // None
		CRC32 = 0x00000001, // The "(attributes)" contains CRC32 for each file
		FILETIME = 0x00000002, // The "(attributes)" contains file time for each file
		MD5 = 0x00000004, // The "(attributes)" contains MD5 for each file
		PATCH_BIT = 0x00000008, // The "(attributes)" contains a patch bit for each file
		ALL = 0x0000000F, // Summary mask
	};
	[Flags]
	public enum class CreateArchiveFlags : unsigned long {
		// Flags for SFileCreateArchive
		LISTFILE = 0x00100000, // Also add the (listfile) file
		ATTRIBUTES = 0x00200000, // Also add the (attributes) file
		SIGNATURE = 0x00400000, // Also add the (signature) file
		ARCHIVE_V1 = 0x00000000, // Creates archive of version 1 (size up to 4GB)
		ARCHIVE_V2 = 0x01000000, // Creates archive of version 2 (larger than 4 GB)
		ARCHIVE_V3 = 0x02000000, // Creates archive of version 3
		ARCHIVE_V4 = 0x03000000, // Creates archive of version 4
		ARCHIVE_VMASK = 0x0F000000, // Mask for archive version
	};
	[Flags]
	public enum class AddFileFlags : unsigned long {
		IMPLODE = 0x00000100, // Implode method (By PKWARE Data Compression Library)
		COMPRESS = 0x00000200, // Compress methods (By multiple methods)
		ENCRYPTED = 0x00010000, // Indicates whether file is encrypted
		FIX_KEY = 0x00020000, // File decryption key has to be fixed
		PATCH_FILE = 0x00100000, // The file is a patch file. Raw file data begin with TPatchInfo structure
		SINGLE_UNIT = 0x01000000, // File is stored as a single unit, rather than split into sectors (Thx, Quantam)
		DELETE_MARKER = 0x02000000, // File is a deletion marker. Used in MPQ patches, indicating that the file no longer exists.
		SECTOR_CRC = 0x04000000, // File has checksums for each sector.
		// Ignored if file is not compressed or imploded.
		SIGNATURE = 0x10000000, // Present on STANDARD.SNP\(signature). The only occurence ever observed
		EXISTS = 0x80000000, // Set if file exists, reset when the file was deleted
		REPLACEEXISTING = 0x80000000, // Replace when the file exist (SFileAddFile)
	};
	[Flags]
	public enum class MPQAttributes : unsigned long {
		// Flags for MPQ attributes
		NONE = 0x0, //None
		CRC32 = 0x00000001, // The "(attributes)" contains CRC32 for each file
		FILETIME = 0x00000002, // The "(attributes)" contains file time for each file
		MD5 = 0x00000004, // The "(attributes)" contains MD5 for each file
		PATCH_BIT = 0x00000008, // The "(attributes)" contains a patch bit for each file
		ALL = 0x0000000F, // Summary mask
	};
	public enum class CompressionType : unsigned long {
		// Compression types for multiple compressions
		HUFFMANN = 0x01, // Huffmann compression (used on WAVE files only)
		ZLIB = 0x02, // ZLIB compression
		PKWARE = 0x08, // PKWARE DCL compression
		BZIP2 = 0x10, // BZIP2 compression (added in Warcraft III)
		SPARSE = 0x20, // Sparse compression (added in Starcraft 2)
		ADPCM_MONO = 0x40, // IMA ADPCM compression (mono)
		ADPCM_STEREO = 0x80, // IMA ADPCM compression (stereo)
		LZMA = 0x12, // LZMA compression. Added in Starcraft 2. This value is NOT a combination of flags.
		NEXT_SAME = 0xFFFFFFFF, // Same compression
	};
	public enum class ArchiveOpenMode : byte {
		Open,
		Create
	};
	public enum class AddWaveQualityFlags : unsigned long {
		// Constants for SFileAddWave
		HIGH = 0, // Best quality, the worst compression
		MEDIUM = 1, // Medium quality, medium compression
		LOW = 2, // Low quality, the best compression
	};
	public enum class VerifyRawDataType {
		// Flags for SFileVerifyRawData (for MPQs version 4.0 or higher)
		MPQ_HEADER = 0x0001, // Verify raw MPQ header
		HET_TABLE = 0x0002, // Verify raw data of the HET table
		BET_TABLE = 0x0003, // Verify raw data of the BET table
		HASH_TABLE = 0x0004, // Verify raw data of the hash table
		BLOCK_TABLE = 0x0005, // Verify raw data of the block table
		HIBLOCK_TABLE = 0x0006, // Verify raw data of the hi-block table
		FILE = 0x0007, // Verify raw data of a file
	};
	public enum class MPQOpenArchiveFlags : unsigned long {
		// Flags for SFileOpenArchive
		_BASE_PROVIDER_FILE = 0x00000000, // Base data source is a file
		_BASE_PROVIDER_MAP = 0x00000001, // Base data source is memory-mapped file
		_BASE_PROVIDER_HTTP = 0x00000002, // Base data source is a file on web server
		_BASE_PROVIDER_MASK = 0x0000000F, // Mask for base provider value

		_STREAM_PROVIDER_FLAT = 0x00000000, // Stream is linear with no offset mapping
		_STREAM_PROVIDER_PARTIAL = 0x00000010, // Stream is partial file (.part)
		_STREAM_PROVIDER_MPQE = 0x00000020, // Stream is an encrypted MPQ
		_STREAM_PROVIDER_BLOCK4 = 0x00000030, // 0x4000 per block, text MD5 after each block, max 0x2000 blocks per file
		_STREAM_PROVIDER_MASK = 0x000000F0, // Mask for stream provider value

		_STREAM_FLAG_READ_ONLY = 0x00000100, // Stream is read only
		_STREAM_FLAG_WRITE_SHARE = 0x00000200, // Allow write sharing when open for write
		_STREAM_FLAG_USE_BITMAP = 0x00000400, // If the file has a file bitmap, load it and use it
		_STREAM_OPTIONS_MASK = 0x0000FF00, // Mask for stream options

		_STREAM_PROVIDERS_MASK = 0x000000FF, // Mask to get stream providers
		_STREAM_FLAGS_MASK = 0x0000FFFF, // Mask for all stream flags (providers+options)

		NO_LISTFILE = 0x00010000, // Don't load the internal listfile
		NO_ATTRIBUTES = 0x00020000, // Don't open the attributes
		NO_HEADER_SEARCH = 0x00040000, // Don't search for the MPQ header past the begin of the file
		FORCE_MPQ_V1 = 0x00080000, // Always open the archive as MPQ v 1.00, ignore the "wFormatVersion" variable in the header
		CHECK_SECTOR_CRC = 0x00100000, // On files with MPQ_FILE_SECTOR_CRC, the CRC will be checked when reading file
		PATCH = 0x00200000, // This archive is a patch MPQ. Used internally.
		FORCE_LISTFILE = 0x00400000, // Force add listfile even if there is none at the moment of opening
		READ_ONLY = STREAM_FLAG_READ_ONLY,
	};
	public enum class InfoClass {
		// Info classes for archives
		FileMpqFileName, // Name of the archive file (TCHAR [])
		FileMpqStreamBitmap, // Array of bits, each bit means availability of one block (BYTE [])
		FileMpqUserDataOffset, // Offset of the user data header (ULONGLONG)
		FileMpqUserDataHeader, // Raw (unfixed) user data header (TMPQUserData)
		FileMpqUserData, // MPQ USer data, without the header (BYTE [])
		FileMpqHeaderOffset, // Offset of the MPQ header (ULONGLONG)
		FileMpqHeaderSize, // Fixed size of the MPQ header
		FileMpqHeader, // Raw (unfixed) archive header (TMPQHeader)
		FileMpqHetTableOffset, // Offset of the HET table, relative to MPQ header (ULONGLONG)
		FileMpqHetTableSize, // Compressed size of the HET table (ULONGLONG)
		FileMpqHetHeader, // HET table header (TMPQHetHeader)
		FileMpqHetTable, // HET table as pointer. Must be freed using FileFreeFileInfo
		FileMpqBetTableOffset, // Offset of the BET table, relative to MPQ header (ULONGLONG)
		FileMpqBetTableSize, // Compressed size of the BET table (ULONGLONG)
		FileMpqBetHeader, // BET table header, followed by the flags (TMPQBetHeader + DWORD[])
		FileMpqBetTable, // BET table as pointer. Must be freed using FileFreeFileInfo
		FileMpqHashTableOffset, // Hash table offset, relative to MPQ header (ULONGLONG)
		FileMpqHashTableSize64, // Compressed size of the hash table (ULONGLONG)
		FileMpqHashTableSize, // Size of the hash table, in entries (DWORD)
		FileMpqHashTable, // Raw (unfixed) hash table (TMPQBlock [])
		FileMpqBlockTableOffset, // Block table offset, relative to MPQ header (ULONGLONG)
		FileMpqBlockTableSize64, // Compressed size of the block table (ULONGLONG)
		FileMpqBlockTableSize, // Size of the block table, in entries (DWORD)
		FileMpqBlockTable, // Raw (unfixed) block table (TMPQBlock [])
		FileMpqHiBlockTableOffset, // Hi-block table offset, relative to MPQ header (ULONGLONG)
		FileMpqHiBlockTableSize64, // Compressed size of the hi-block table (ULONGLONG)
		FileMpqHiBlockTable, // The hi-block table (USHORT [])
		FileMpqSignatures, // Signatures present in the MPQ (DWORD)
		FileMpqStrongSignatureOffset, // Byte offset of the strong signature, relative to begin of the file (ULONGLONG)
		FileMpqStrongSignatureSize, // Size of the strong signature (DWORD)
		FileMpqStrongSignature, // The strong signature (BYTE [])
		FileMpqArchiveSize64, // Archive size from the header (ULONGLONG)
		FileMpqArchiveSize, // Archive size from the header (DWORD)
		FileMpqMaxFileCount, // Max number of files in the archive (DWORD)
		FileMpqFileTableSize, // Number of entries in the file table (DWORD)
		FileMpqSectorSize, // Sector size (DWORD)
		FileMpqNumberOfFiles, // Number of files (DWORD)
		FileMpqRawChunkSize, // Size of the raw data chunk for MD5
		FileMpqStreamFlags, // Stream flags (DWORD)
		FileMpqFlags, // Nonzero if the MPQ is read only (DWORD)

		// Info classes for files
		FileInfoPatchChain, // Chain of patches where the file is (TCHAR [])
		FileInfoFileEntry, // The file entry for the file (TFileEntry)
		FileInfoHashEntry, // Hash table entry for the file (TMPQHash)
		FileInfoHashIndex, // Index of the hash table entry (DWORD)
		FileInfoNameHash1, // The first name hash in the hash table (DWORD)
		FileInfoNameHash2, // The second name hash in the hash table (DWORD)
		FileInfoNameHash3, // 64-bit file name hash for the HET/BET tables (ULONGLONG)
		FileInfoLocale, // File locale (DWORD)
		FileInfoFileIndex, // Block index (DWORD)
		FileInfoByteOffset, // File position in the archive (ULONGLONG)
		FileInfoFileTime, // File time (ULONGLONG)
		FileInfoFileSize, // Size of the file (DWORD)
		FileInfoCompressedSize, // Compressed file size (DWORD)
		FileInfoFlags, // File flags from (DWORD)
		FileInfoEncryptionKey, // File encryption key
		FileInfoEncryptionKeyRaw, // Unfixed value of the file key
		FileInfoCRC32, // CRC32 of the file
	};
	public enum class MpqFileSearchScope : unsigned long {
		// Values for SFileOpenFile
		FromMPQ = 0x00000000, // Open the file from the MPQ archive
		CheckExists = 0xFFFFFFFC, // Only check whether the file exists
		BaseFile = 0xFFFFFFFD, // Reserved for StormLib internal use
		AnyLocale = 0xFFFFFFFE, // Reserved for StormLib internal use
		LocalFile = 0xFFFFFFFF, // Open a local file
	};
	public enum class SignatureType : unsigned long {
		// Signature types
		NONE = 0x0000, // The archive has no signature in it
		WEAK = 0x0001, // The archive has weak signature
		STRONG = 0x0002, // The archive has strong signature
	};
	// Structure for patch prefix
	public value class NamePrefix {
	public:
		unsigned int nLength; // Length of this patch prefix. Can be 0
		cli::array < char >^ szPatchPrefix; // Patch name prefix (variable length). If not empty, it always starts with backslash.

		static NamePrefix FromNativePointer(TMPQNamePrefix* native) {
			NamePrefix managed;
			if (native != nullptr) {
				managed.nLength = native->nLength;
				managed.szPatchPrefix = gcnew cli::array < char >(1);
				for (size_t i = 0; i < 1; i++) {
					managed.szPatchPrefix[i] = native->szPatchPrefix[i];
				}
			}
			return managed;
		}
	};
	public value class CreateMPQInfo {
	public:
		unsigned long cbSize; // Size of this structure, in bytes
		unsigned long dwMpqVersion; // Version of the MPQ to be created
		IntPtr pvUserData; // Reserved, must be NULL
		unsigned long cbUserData; // Reserved, must be 0
		unsigned long dwStreamFlags; // Stream flags for creating the MPQ
		unsigned long dwFileFlags1; // File flags for (listfile). Use MPQ_FILE_DEFAULT_INTERNAL to set default flags
		unsigned long dwFileFlags2; // File flags for (attributes). Use MPQ_FILE_DEFAULT_INTERNAL to set default flags
		unsigned long dwFileFlags3; // File flags for (signature). Use MPQ_FILE_DEFAULT_INTERNAL to set default flags
		unsigned long dwAttrFlags; // Flags for the (attributes) file. If 0, no attributes will be created
		unsigned long dwSectorSize; // Sector size for compressed files
		unsigned long dwRawChunkSize; // Size of raw data chunk
		unsigned long dwMaxFileCount; // File limit for the MPQ
		void FromNativeStructure(SFILE_CREATE_MPQ native) {
			cbSize = native.cbSize;
			dwMpqVersion = native.dwMpqVersion;
			pvUserData = IntPtr(native.pvUserData);
			cbUserData = native.cbUserData;
			dwStreamFlags = native.dwStreamFlags;
			dwFileFlags1 = native.dwFileFlags1;
			dwFileFlags2 = native.dwFileFlags2;
			dwFileFlags3 = native.dwFileFlags3;
			dwAttrFlags = native.dwAttrFlags;
			dwSectorSize = native.dwSectorSize;
			dwRawChunkSize = native.dwRawChunkSize;
			dwMaxFileCount = native.dwMaxFileCount;
		}
		SFILE_CREATE_MPQ ToNativeStructure() {
			SFILE_CREATE_MPQ native;
			native.cbSize = cbSize;
			native.dwMpqVersion = dwMpqVersion;
			native.pvUserData = pvUserData.ToPointer();
			native.cbUserData = cbUserData;
			native.dwStreamFlags = dwStreamFlags;
			native.dwFileFlags1 = dwFileFlags1;
			native.dwFileFlags2 = dwFileFlags2;
			native.dwFileFlags3 = dwFileFlags3;
			native.dwAttrFlags = dwAttrFlags;
			native.dwSectorSize = dwSectorSize;
			native.dwRawChunkSize = dwRawChunkSize;
			native.dwMaxFileCount = dwMaxFileCount;
			return native;
		}
	};
	// MPQ file header
	//
	// We have to make sure that the header is packed OK.
	// Reason: A 64-bit integer at the beginning of 3.0 part,
	// which is offset 0x2C
	public value class Header {
	public:
		// The ID_MPQ ('MPQ\x1A') signature
		unsigned long dwID;

		// Size of the archive header
		unsigned long dwHeaderSize;

		// 32-bit size of MPQ archive
		// This field is deprecated in the Burning Crusade MoPaQ format, and the size of the archive
		// is calculated as the size from the beginning of the archive to the end of the hash table,
		// block table, or hi-block table (whichever is largest).
		unsigned long dwArchiveSize;

		// 0 = Format 1 (up to The Burning Crusade)
		// 1 = Format 2 (The Burning Crusade and newer)
		// 2 = Format 3 (WoW - Cataclysm beta or newer)
		// 3 = Format 4 (WoW - Cataclysm beta or newer)
		unsigned short wFormatVersion;

		// Power of two exponent specifying the number of 512-byte disk sectors in each file sector
		// in the archive. The size of each file sector in the archive is 512 * 2 ^ wSectorSize.
		unsigned short wSectorSize;

		// Offset to the beginning of the hash table, relative to the beginning of the archive.
		unsigned long dwHashTablePos;

		// Offset to the beginning of the block table, relative to the beginning of the archive.
		unsigned long dwBlockTablePos;

		// Number of entries in the hash table. Must be a power of two, and must be less than 2^16 for
		// the original MoPaQ format, or less than 2^20 for the Burning Crusade format.
		unsigned long dwHashTableSize;

		// Number of entries in the block table
		unsigned long dwBlockTableSize;

		//-- MPQ HEADER v 2 -------------------------------------------

		// Offset to the beginning of array of 16-bit high parts of file offsets.
		unsigned long long HiBlockTablePos64;

		// High 16 bits of the hash table offset for large archives.
		unsigned short wHashTablePosHi;

		// High 16 bits of the block table offset for large archives.
		unsigned short wBlockTablePosHi;

		//-- MPQ HEADER v 3 -------------------------------------------

		// 64-bit version of the archive size
		unsigned long long ArchiveSize64;

		// 64-bit position of the BET table
		unsigned long long BetTablePos64;

		// 64-bit position of the HET table
		unsigned long long HetTablePos64;

		//-- MPQ HEADER v 4 -------------------------------------------

		// Compressed size of the hash table
		unsigned long long HashTableSize64;

		// Compressed size of the block table
		unsigned long long BlockTableSize64;

		// Compressed size of the hi-block table
		unsigned long long HiBlockTableSize64;

		// Compressed size of the HET block
		unsigned long long HetTableSize64;

		// Compressed size of the BET block
		unsigned long long BetTableSize64;

		// Size of raw data chunk to calculate MD5.
		// MD5 of each data chunk follows the raw file data.
		unsigned long dwRawChunkSize;

		// MD5 of MPQ tables
		cli::array < byte >^ MD5_BlockTable; // MD5 of the block table before decryption
		cli::array < byte >^ MD5_HashTable; // MD5 of the hash table before decryption
		cli::array < byte >^ MD5_HiBlockTable; // MD5 of the hi-block table
		cli::array < byte >^ MD5_BetTable; // MD5 of the BET table before decryption
		cli::array < byte >^ MD5_HetTable; // MD5 of the HET table before decryption
		cli::array < byte >^ MD5_MpqHeader; // MD5 of the MPQ header from signature to (including) MD5_HetTable
		static Header FromNativePointer(TMPQHeader* native) {
			Header managed;
			if (native == nullptr) {

				managed.dwID = native->dwID;
				managed.dwHeaderSize = native->dwHeaderSize;
				managed.dwArchiveSize = native->dwArchiveSize;
				managed.wFormatVersion = native->wFormatVersion;
				managed.wSectorSize = native->wSectorSize;
				managed.dwHashTablePos = native->dwHashTablePos;
				managed.dwBlockTablePos = native->dwBlockTablePos;
				managed.dwHashTableSize = native->dwHashTableSize;
				managed.dwBlockTableSize = native->dwBlockTableSize;
				if (managed.wFormatVersion == 0) return managed;
				//v2
				managed.HiBlockTablePos64 = native->HiBlockTablePos64;
				managed.wHashTablePosHi = native->wHashTablePosHi;
				managed.wBlockTablePosHi = native->wBlockTablePosHi;
				if (managed.wFormatVersion == 1) return managed;
				//v3
				managed.ArchiveSize64 = native->ArchiveSize64;
				managed.BetTablePos64 = native->BetTablePos64;
				managed.HetTablePos64 = native->HetTablePos64;
				if (managed.wFormatVersion == 2) return managed;
				//v4
				managed.HashTableSize64 = native->HashTableSize64;
				managed.BlockTableSize64 = native->BlockTableSize64;
				managed.HiBlockTableSize64 = native->HiBlockTableSize64;
				managed.HetTableSize64 = native->HetTableSize64;
				managed.BetTableSize64 = native->BetTableSize64;
				managed.dwRawChunkSize = native->dwRawChunkSize;
				managed.MD5_BlockTable = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				managed.MD5_HashTable = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				managed.MD5_HiBlockTable = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				managed.MD5_BetTable = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				managed.MD5_HetTable = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				managed.MD5_MpqHeader = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				for (size_t i = 0; i < MD5_DIGEST_SIZE; i++) {
					managed.MD5_BlockTable[i] = native->MD5_BlockTable[i];
					managed.MD5_HashTable[i] = native->MD5_HashTable[i];
					managed.MD5_HiBlockTable[i] = native->MD5_HiBlockTable[i];
					managed.MD5_BetTable[i] = native->MD5_BetTable[i];
					managed.MD5_HetTable[i] = native->MD5_HetTable[i];
					managed.MD5_MpqHeader[i] = native->MD5_MpqHeader[i];
				}
			}
			return managed;
		}
	};
	public value class MPQHash {
	public:
		// The hash of the file path, using method A.
		unsigned long dwName1;

		// The hash of the file path, using method B.
		unsigned long dwName2;
		// The language of the file. This is a Windows LANGID data type, and uses the same values.
		// 0 indicates the default language (American English), or that the file is language-neutral.
		unsigned short lcLocale;

		// The platform the file is used for. 0 indicates the default platform.
		// No other values have been observed.
		byte Platform;
		byte Reserved;
		// If the hash table entry is valid, this is the index into the block table of the file.
		// Otherwise, one of the following two values:
		//  - FFFFFFFFh: Hash table entry is empty, and has always been empty.
		//               Terminates searches for a given file.
		//  - FFFFFFFEh: Hash table entry is empty, but was valid at some point (a deleted file).
		//               Does not terminate searches for a given file.
		unsigned long dwBlockIndex;
		static MPQHash FromNativePointer(TMPQHash* native) {
			MPQHash managed;
			if (native != nullptr) {
				managed.dwName1 = native->dwName1;
				managed.dwName2 = native->dwName2;
				managed.lcLocale = native->lcLocale;
				managed.Platform = native->Platform;
				managed.Reserved = native->Reserved;
				managed.dwBlockIndex = native->dwBlockIndex;
			}
			return managed;
		}
	};
	public value class MPQBlock {
	public:
		// Offset of the beginning of the file, relative to the beginning of the archive.
		unsigned long dwFilePos;

		// Compressed file size
		unsigned long dwCSize;

		// Only valid if the block is a file; otherwise meaningless, and should be 0.
		// If the file is compressed, this is the size of the uncompressed file data.
		unsigned long dwFSize;

		// Flags for the file. See MPQ_FILE_XXXX constants
		MPQFileFlags dwFlags;
		unsigned long dwBlockIndex;
		static MPQBlock FromNativePointer(TMPQBlock* native) {
			MPQBlock managed;
			if (native != nullptr) {
				managed.dwFilePos = native->dwFilePos;
				managed.dwCSize = native->dwCSize;
				managed.dwFSize = native->dwFSize;
				managed.dwFlags = (MPQFileFlags)native->dwFlags;
			}
			return managed;
		}
	};
	public value class MPQBits {
	public:
		static const cli::array<unsigned short>^ SetBitsMask =
			gcnew cli::array<unsigned short> { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };
		unsigned long NumberOfBytes;
		unsigned long NumberOfBits;
		cli::array<byte>^ Elements;
	};
	public value class HetTable {
	public:
		MPQBits pBetIndexes; // Bit array of FileIndex values
		cli::array < byte >^ pNameHashes; // Array of NameHash1 values (NameHash1 = upper 8 bits of FileName hashe)
		unsigned long long AndMask64; // AND mask used for calculating file name hash
		unsigned long long OrMask64; // OR mask used for setting the highest bit of the file name hash

		unsigned long dwEntryCount; // Number of occupied entries in the HET table
		unsigned long dwTotalCount; // Number of entries in both NameHash and FileIndex table
		unsigned long dwNameHashBitSize; // Size of the name hash entry (in bits)
		unsigned long dwIndexSizeTotal; // Total size of one entry in pBetIndexes (in bits)
		unsigned long dwIndexSizeExtra; // Extra bits in the entry in pBetIndexes
		unsigned long dwIndexSize; // Effective size of one entry in pBetIndexes (in bits)
		static HetTable FromNativePointer(TMPQHetTable* native) {
			HetTable managed;
			if (native != nullptr) {

			}
			return managed;
		}
	};
	[DebuggerDisplay("{szFileName}")]
	public value class FileEntry {
	public:
		unsigned long long FileNameHash; // Jenkins hash of the file name. Only used when the MPQ has BET table.
		unsigned long long ByteOffset; // Position of the file content in the MPQ, relative to the MPQ header
		DateTime FileTime; // FileTime from the (attributes) file. 0 if not present.
		unsigned long dwFileSize; // Decompressed size of the file
		unsigned long dwCmpSize; // Compressed size of the file (i.e., size of the file data in the MPQ)
		MPQFileFlags dwFlags; // File flags (from block table)
		unsigned long dwCrc32; // CRC32 from (attributes) file. 0 if not present.
		cli::array < byte >^ md5; // File MD5 from the (attributes) file. 0 if not present.
		String^ szFileName; // File name. NULL if not known.
		static FileEntry FromNativePointer(TFileEntry* native) {
			FileEntry managed;
			if (native != nullptr) {
				managed.FileNameHash = native->FileNameHash;
				managed.ByteOffset = native->ByteOffset;
				if (native->FileTime == 0)
					managed.FileTime = DateTime::MinValue;
				else
					managed.FileTime = DateTime::FromFileTime(native->FileTime);
				managed.dwFileSize = native->dwFileSize;
				managed.dwCmpSize = native->dwCmpSize;
				managed.dwFlags = (MPQFileFlags)native->dwFlags;
				managed.dwCrc32 = native->dwCrc32;
				managed.md5 = gcnew cli::array < byte >(MD5_DIGEST_SIZE);
				for (size_t i = 0; i < MD5_DIGEST_SIZE; i++) {
					managed.md5[i] = native->md5[i];
				}
				managed.szFileName = gcnew String(native->szFileName);
			}
			return managed;
		}
	};
	public value class PatchInfo {
	public:
		unsigned long dwLength; // Length of patch info header, in bytes
		unsigned long dwFlags; // Flags. 0x80000000 = MD5 (?)
		unsigned long dwDataSize; // Uncompressed size of the patch file
		cli::array < byte >^ md5; // MD5 of the entire patch file after decompression
		static PatchInfo FromNativePointer(_TPatchInfo* native) {
			PatchInfo managed;
			if (native != nullptr) {
				managed.dwLength = native->dwLength;
				managed.dwFlags = native->dwFlags;
				managed.dwDataSize = native->dwDataSize;
				managed.md5 = gcnew cli::array < byte >(0x10);
				for (size_t i = 0; i < 0x10; i++) {
					managed.md5[i] = native->md5[i];
				}
			}
			return managed;
		}
	};
	public value class MPQUserData {
	public:
		// The ID_MPQ_USERDATA ('MPQ\x1B') signature
		unsigned long dwID;

		// Maximum size of the user data
		unsigned long cbUserDataSize;

		// Offset of the MPQ header, relative to the begin of this header
		unsigned long dwHeaderOffs;

		// Appears to be size of user data header (Starcraft II maps)
		unsigned long cbUserDataHeader;
		static MPQUserData FromNativePointer(TMPQUserData* native) {
			MPQUserData managed;
			if (native != nullptr) {
				managed.dwID = native->dwID;
				managed.cbUserDataSize = native->cbUserDataSize;
				managed.dwHeaderOffs = native->dwHeaderOffs;
				managed.cbUserDataHeader = native->cbUserDataHeader;
			}
			return managed;
		}
		static MPQUserData FromNativeStructure(TMPQUserData native) {
			return FromNativePointer(&native);
		}
	};
	public value class FoundFileData {
	public:
		String^ cFileName; // Full name of the found file
		String^ szPlainName; // Plain name of the found file
		unsigned long dwHashIndex; // Hash table index for the file (HAH_ENTRY_FREE if no hash table)
		unsigned long dwBlockIndex; // Block table index for the file
		unsigned long dwFileSize; // File size in bytes
		MPQFileFlags dwFileFlags; // MPQ file flags
		unsigned long dwCompSize; // Compressed file size
		unsigned long dwFileTimeLo; // Low 32-bits of the file time (0 if not present)
		unsigned long dwFileTimeHi; // High 32-bits of the file time (0 if not present)
		CultureInfo^ lcLocale; // Locale version

		static FoundFileData FromNativePointer(SFILE_FIND_DATA* native) {
			FoundFileData managed;
			if (native != nullptr) {
				managed.cFileName = gcnew String(native->cFileName);
				managed.szPlainName = gcnew String(native->szPlainName);
				managed.dwHashIndex = native->dwHashIndex;
				managed.dwBlockIndex = native->dwBlockIndex;
				managed.dwFileSize = native->dwFileSize;
				managed.dwFileFlags = (MPQFileFlags)native->dwFileFlags;
				managed.dwCompSize = native->dwCompSize;
				managed.dwFileTimeLo = native->dwFileTimeLo;
				managed.dwFileTimeHi = native->dwFileTimeHi;
			}
			return managed;
		}
	};
}
public union BigInteger {
	struct {
		long lowValue;
		long highValue;
	}
	parts;
	unsigned long long value;
};