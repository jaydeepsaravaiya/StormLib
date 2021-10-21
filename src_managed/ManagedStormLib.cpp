
#include "ManagedStormLib.h"
#include <iostream>

using namespace System::ComponentModel;
#pragma unmanaged
DWORD GetNativeLastError() {
	return GetLastError();
}
#pragma managed


using namespace System::Runtime::InteropServices;

const TCHAR* ManagedStormLib::ConvertToUnicodeString(String^ managedString)
{
	return (const TCHAR*)Marshal::StringToHGlobalUni(managedString).ToPointer();
}

const char* ManagedStormLib::ConvertToAnsiString(String^ managedString)
{
	return (const char*)Marshal::StringToHGlobalAnsi(managedString).ToPointer();
}

static ManagedStormLib::Common::Common()
{
	throw gcnew System::NotImplementedException();
}

bool ManagedStormLib::Common::SetDataCompression(DWORD DataCompression)
{
	return false;
}

bool ManagedStormLib::Common::SetArchiveMarkers(PSFILE_MARKERS pMarkers)
{
	return false;
}



ManagedStormLib::FileInformation::FileInformation(HANDLE _parent, InfoClass _class)
{
	unsigned long lengthNeeded;
	SFileGetFileInfo(_parent, (SFileInfoClass)_class, nullptr, 0, &lengthNeeded);// To figure out required number of bytes 
	//void* pInfo = malloc(lengthNeeded);
	_pInfo = Marshal::AllocHGlobal(lengthNeeded).ToPointer();
	if (SFileGetFileInfo(_parent, (SFileInfoClass)_class, _pInfo, lengthNeeded, nullptr)) {//not required anymore as we already know size now
		switch (_class)
		{
		case ManagedStormLib::InfoClass::FileMpqFileName://String
		case ManagedStormLib::InfoClass::FileInfoPatchChain:
		{

			info = gcnew  String((const TCHAR*)_pInfo);
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqStreamBitmap:
		case ManagedStormLib::InfoClass::FileMpqStrongSignature://Byte[]
		{
			cli::array<byte>^ byte_array = gcnew  cli::array<byte>(lengthNeeded);
			for (size_t i = 0; i < lengthNeeded; i++)
			{
				byte_array[i] = *((byte*)_pInfo + i);
			}
			info = byte_array;
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqUserDataOffset://ULONGLONG
		case ManagedStormLib::InfoClass::FileMpqHeaderOffset:
		case ManagedStormLib::InfoClass::FileMpqHetTableOffset:
		case ManagedStormLib::InfoClass::FileMpqHetTableSize:
		case ManagedStormLib::InfoClass::FileMpqBetTableOffset:
		case ManagedStormLib::InfoClass::FileMpqBetTableSize:
		case ManagedStormLib::InfoClass::FileMpqHashTableOffset:
		case ManagedStormLib::InfoClass::FileMpqHashTableSize64:
		case ManagedStormLib::InfoClass::FileMpqBlockTableOffset:
		case ManagedStormLib::InfoClass::FileMpqBlockTableSize64:
		case ManagedStormLib::InfoClass::FileMpqHiBlockTableOffset:
		case ManagedStormLib::InfoClass::FileMpqHiBlockTableSize64:
		case ManagedStormLib::InfoClass::FileMpqStrongSignatureOffset:
		case ManagedStormLib::InfoClass::FileMpqArchiveSize64:
		case ManagedStormLib::InfoClass::FileInfoNameHash3:
		case ManagedStormLib::InfoClass::FileInfoByteOffset:
		case ManagedStormLib::InfoClass::FileInfoFileTime:
		{
			info = *(unsigned long long*)_pInfo;
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqUserDataHeader://UserData
		{
			info = UserData::FromNativePointer((TMPQUserData*)_pInfo);
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqUserData://Userdata without Header, byte[]
		{

		}
		break;
		case ManagedStormLib::InfoClass::FileMpqHeaderSize://DWORD
		case ManagedStormLib::InfoClass::FileMpqHashTableSize:
		case ManagedStormLib::InfoClass::FileMpqBlockTableSize:
		case ManagedStormLib::InfoClass::FileMpqSignatures:
		case ManagedStormLib::InfoClass::FileMpqStrongSignatureSize:
		case ManagedStormLib::InfoClass::FileMpqArchiveSize:
		case ManagedStormLib::InfoClass::FileMpqMaxFileCount:
		case ManagedStormLib::InfoClass::FileMpqFileTableSize:
		case ManagedStormLib::InfoClass::FileMpqSectorSize:
		case ManagedStormLib::InfoClass::FileMpqNumberOfFiles:
		case ManagedStormLib::InfoClass::FileMpqRawChunkSize:
		case ManagedStormLib::InfoClass::FileMpqStreamFlags:
		case ManagedStormLib::InfoClass::FileMpqFlags:
		case ManagedStormLib::InfoClass::FileInfoHashIndex:
		case ManagedStormLib::InfoClass::FileInfoNameHash1:
		case ManagedStormLib::InfoClass::FileInfoNameHash2:
		case ManagedStormLib::InfoClass::FileInfoLocale:
		case ManagedStormLib::InfoClass::FileInfoFileIndex:
		case ManagedStormLib::InfoClass::FileInfoFileSize:
		case ManagedStormLib::InfoClass::FileInfoCompressedSize:
		case ManagedStormLib::InfoClass::FileInfoFlags:
		case ManagedStormLib::InfoClass::FileInfoEncryptionKey:
		case ManagedStormLib::InfoClass::FileInfoEncryptionKeyRaw:
		case ManagedStormLib::InfoClass::FileInfoCRC32:
		{
			info = *(unsigned long*)_pInfo;
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqHeader://Header
		{
			info = Header::FromNativePointer((TMPQHeader*)_pInfo);
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqHetHeader://MPQHetHeader
			break;
		case ManagedStormLib::InfoClass::FileMpqHetTable://HetTable[]
			break;
		case ManagedStormLib::InfoClass::FileMpqBetHeader://MPQBetHeader+Dword[BetHeaderFlagCount]
			break;
		case ManagedStormLib::InfoClass::FileMpqBetTable://BetTable[]
			break;
		case ManagedStormLib::InfoClass::FileMpqHashTable://TMPQHash[]
		{
			unsigned long array_len = lengthNeeded / sizeof(TMPQHash);
			cli::array<MPQHash>^ hash_array = gcnew  cli::array<MPQHash>(array_len);
			for (size_t i = 0; i < array_len; i++)
			{
				hash_array[i] = MPQHash::FromNativePointer((TMPQHash*)_pInfo + i);
			}
			info = hash_array;
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqBlockTable://TMPQBlock[]
		{
			unsigned long array_len = lengthNeeded / sizeof(TMPQBlock);
			cli::array<MPQBlock>^ block_array = gcnew  cli::array<MPQBlock>(array_len);
			for (size_t i = 0; i < array_len; i++)
			{
				block_array[i] = MPQBlock::FromNativePointer((TMPQBlock*)_pInfo + i);
			}
			info = block_array;
		}
		break;
		case ManagedStormLib::InfoClass::FileMpqHiBlockTable://USHORT[]
		{
			unsigned long array_len = lengthNeeded / sizeof(unsigned short);
			cli::array<unsigned short>^ short_array = gcnew  cli::array<unsigned short>(array_len);
			for (size_t i = 0; i < array_len; i++)
			{
				short_array[i] = *((unsigned short*)_pInfo + i);
			}
			info = short_array;
		}
		break;
		case ManagedStormLib::InfoClass::FileInfoFileEntry://TFileEntry
		{
			info = FileEntry::FromNativePointer((TFileEntry*)_pInfo);
		}
		break;
		case ManagedStormLib::InfoClass::FileInfoHashEntry://TMPQHash
		{
			info = MPQHash::FromNativePointer((TMPQHash*)_pInfo);
		}
		break;
		default:
			break;
		}
	}
	else {
		throw gcnew Win32Exception(GetNativeLastError());
	}

}

ManagedStormLib::FileInformation::~FileInformation()
{
	this->!FileInformation();
}

ManagedStormLib::FileInformation::!FileInformation()
{
	FreeFileInfo();
}

void ManagedStormLib::FileInformation::FreeFileInfo()
{
	if (_pInfo == nullptr)
		return;
	if (!SFileFreeFileInfo(_pInfo, (SFileInfoClass)Class)) { // has no effect if FileInfo is not Freeable
		throw gcnew Win32Exception(GetNativeLastError());
	}
	_pInfo = nullptr;
}

ManagedStormLib::FileFinder::FileFinder(HANDLE hFinder)
{
	_hFinder = hFinder;
}

bool ManagedStormLib::FileFinder::FindNextFile(FoundFileData% lpFindFileData)
{
	SFILE_FIND_DATA pData;
	bool result = SFileFindNextFile(_hFinder, &pData);
	lpFindFileData = FoundFileData::FromNativePointer(&pData);
	return result;
}

bool ManagedStormLib::FileFinder::Close()
{
	return SFileFindClose(_hFinder);
}

ManagedStormLib::ListFileFinder::ListFileFinder(HANDLE hFinder)
{
	_hFinder = hFinder;
}

bool ManagedStormLib::ListFileFinder::FindNextFile(FoundFileData% lpFindFileData)
{
	SFILE_FIND_DATA pData;
	bool result = SListFileFindNextFile(_hFinder, &pData);
	lpFindFileData = FoundFileData::FromNativePointer(&pData);
	return result;
}

bool ManagedStormLib::ListFileFinder::Close()
{
	return SListFileFindClose(_hFinder);
}
