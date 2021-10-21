#pragma once
#include "ManagedStormLib.h"
using namespace System::IO;
namespace ManagedStormLib {
	public ref class MPQFile {
	private:
		TMPQFile* _pFile;
	internal:
		event Action<MPQFile^>^ Closed;
	public:
		MPQFile(MPQArchive^ parent, TMPQFile* hFile);
		~MPQFile();
		!MPQFile();
		MPQArchive^ ParentArchive;
		bool SetFileLocale(CultureInfo^ lcNewLocale);
		//-----------------------------------------------------------------------------
		// Support for adding files to the MPQ
		void WriteFile(
			cli::array < byte >^ pvData, CompressionType dwCompression);
		bool FinishFile();
		unsigned long GetFileSize();
		unsigned long SetFilePointer(long lFilePos, [Out]long% plFilePosHigh, SeekOrigin dwMoveMethod);
		bool ReadFile([Out] cli::array < byte >^% Buffer, unsigned long ToRead, [Out] unsigned long% Read, NativeOverlapped Overlapped);
		bool CloseFile();
		// Retrieving info about a file in the archive
		void GetFileInfo(InfoClass InfoClass, [Out] FileInformation^% pvFileInfo);
	};
}