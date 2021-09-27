/*****************************************************************************/
/* TNameList.cpp                          Copyright (c) Ladislav Zezula 2021 */
/*---------------------------------------------------------------------------*/
/* Helper class for reporting StormLib tests                                 */
/* This file should be included directly from StormTest.cpp using #include   */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 18.08.21  1.00  Lad  The first version of TNameList.cpp                   */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// Global functions / defines

// Definition of the path separator
#ifdef STORMLIB_WINDOWS
static LPCTSTR g_szPathSeparator = _T("\\");
static const TCHAR PATH_SEPARATOR = _T('\\');       // Path separator for Windows platforms
#else
static LPCSTR g_szPathSeparator = "/";
static const TCHAR PATH_SEPARATOR = '/';            // Path separator for Non-Windows platforms
#endif

// This must be the directory where our test MPQs are stored.
static TCHAR szMpqDirectory[MAX_PATH+1];
size_t cchMpqDirectory = 0;

static const char * IntToHexChar = "0123456789abcdef";

static const BYTE CharToByte[0x80] = 
{
//   00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0xFF
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x10
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x20
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x30
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x40
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x50
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x60
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  // 0x70
};

template <typename XCHAR>
DWORD BinaryToString(XCHAR * szBuffer, size_t cchBuffer, LPCVOID pvBinary, size_t cbBinary)
{
    const unsigned char * pbBinary = (const unsigned char *)pvBinary;

    // The size of the string must be enough to hold the binary + EOS
    if(cchBuffer < ((cbBinary * 2) + 1))
        return ERROR_INSUFFICIENT_BUFFER;

    // Convert the string to the array of MD5
    // Copy the blob data as text
    for(size_t i = 0; i < cbBinary; i++)
    {
        *szBuffer++ = IntToHexChar[pbBinary[0] >> 0x04];
        *szBuffer++ = IntToHexChar[pbBinary[0] & 0x0F];
        pbBinary++;
    }

    // Terminate the string
    *szBuffer = 0;
    return ERROR_SUCCESS;
}

template <typename XCHAR>
DWORD StringToBinary(const XCHAR * szString, LPVOID pvBinary, size_t cbBinary, size_t * PtrBinary = NULL)
{
    LPBYTE pbBinary = (LPBYTE)pvBinary;
    LPBYTE pbBinaryEnd = pbBinary + cbBinary;
    LPBYTE pbSaveBinary = pbBinary;

    // Verify parameter
    if(szString != NULL && szString[0] != 0)
    {
        // Work as long as we have at least 2 characters ready
        while(szString[0] != 0 && szString[1] != 0)
        {
            // Convert both to unsigned char to get rid of negative indexes produced by szString[x]
            BYTE StringByte0 = (BYTE)szString[0];
            BYTE StringByte1 = (BYTE)szString[1];

            // Each character must be within the range of 0x80
            if(StringByte0 > 0x80 || StringByte1 > 0x80)
                return ERROR_INVALID_PARAMETER;
            if(CharToByte[StringByte0] == 0xFF || CharToByte[StringByte1] == 0xFF)
                return ERROR_INVALID_PARAMETER;

            // Overflow check
            if(pbBinary >= pbBinaryEnd)
                return ERROR_INSUFFICIENT_BUFFER;

            *pbBinary++ = (CharToByte[StringByte0] << 0x04) | CharToByte[StringByte1];
            szString += 2;
        }

        // Odd number of chars?
        if(szString[0] != 0 && szString[1] == 0)
            return ERROR_INVALID_PARAMETER;
    }

    // Give the length
    if(PtrBinary != NULL)
        PtrBinary[0] = pbBinary - pbSaveBinary;
    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// TCharHelper, helping with conversion to TCHAR

class TCharHelper
{
    public:

    TCharHelper(const char * szString)
    {
        m_szString = NULL;
        m_nLength = 0;

        if(szString != NULL)
        {
            m_nLength = strlen(szString);
            
            if((m_szString = new TCHAR[m_nLength + 1]) != NULL)
            {
                StringCopy(m_szString, m_nLength + 1, szString);
            }
        }
    }

    ~TCharHelper()
    {
        delete [] m_szString;
    }

    // Returns length of the string
    size_t Length() const
    {
        return m_nLength;
    }

    // Returns LPCTSTR or NULL
    operator const TCHAR * () const
    {
        return m_szString;
    }

    protected:

    TCHAR * m_szString;
    size_t m_nLength;
};

//-----------------------------------------------------------------------------
// TPathHelper, helping with conversion to full paths

class TPathHelper
{
    public:

    TPathHelper(const wchar_t * szPlainName, const TCHAR * szFolderName1 = NULL, const TCHAR * szFolderName2 = NULL)
    {
        InitPath(szPlainName, szFolderName1, szFolderName2);
    }

    TPathHelper(const char * szPlainName, const TCHAR * szFolderName1 = NULL, const TCHAR * szFolderName2 = NULL)
    {
        TCharHelper plainName(szPlainName);

        InitPath(plainName, szFolderName1, szFolderName2);
    }

    ~TPathHelper()
    {
        delete [] m_szPath;
    }

    // Returns LPCTSTR or NULL
    operator const TCHAR * ()
    {
        return m_szPath;
    }

    protected:

    void InitPath(const wchar_t * szPlainName, const wchar_t * szFolderName1, const wchar_t * szFolderName2)
    {
        // By default, it's NULL
        m_szPath = NULL;

        // If there is an actual name, do the conversion
        if(szPlainName != NULL)
        {
            // Calculate length of the full file name
            m_nLength = CalculateLength(szPlainName, szFolderName1, szFolderName2, NULL);

            // Allocate buffer
            if((m_szPath = new TCHAR[m_nLength + 1]) != NULL)
            {
                m_szPath[0] = 0;
                AppendFolders(szPlainName, szFolderName1, szFolderName2, NULL);
            }
        }
    }


    size_t CalculateLength(const wchar_t * szPlainName, ...)
    {
        LPCTSTR szFolder;
        va_list argList;
        size_t nLength = 0;

        // Append all parts
        va_start(argList, szPlainName);
        while((szFolder = va_arg(argList, LPCTSTR)) != NULL)
        {
            if(szFolder[0])
            {
                nLength = nLength + _tcslen(szFolder) + 1;
            }
        }
        va_end(argList);
        
        // Append the length of the plain name
        assert(szPlainName && szPlainName[0]);
        return nLength + wcslen(szPlainName);
    }

    void AppendFolders(const wchar_t * szPlainName, ...)
    {
        LPCTSTR szFolder;
        va_list argList;

        // Append all parts
        va_start(argList, szPlainName);
        while((szFolder = va_arg(argList, LPCTSTR)) != NULL)
        {
            if(szFolder[0])
            {
                StringCat(m_szPath, m_nLength + 1, szFolder);
                StringCat(m_szPath, m_nLength + 1, g_szPathSeparator);
            }
        }
        va_end(argList);

        // Append file name and exit
        assert(szPlainName && szPlainName[0]);
        StringCat(m_szPath, m_nLength + 1, szPlainName);
    }

    TCHAR * m_szPath;
    size_t m_nLength;
};

//-----------------------------------------------------------------------------
// TNameList, helping with maintaining file name lists

class TNameList
{
    public:

    TNameList();
    ~TNameList();

    bool InsertName(const char * szName);
    bool CompareHash(const char * szWantedHash);
    void Sort();

    char * operator[](size_t nIndex) const;
    char * GetString(size_t nIndex) const;
    size_t GetCount() const;

    protected:

    char * NewStr(const char * szName);
    static int __cdecl CompareTwoNames(const void * elem1, const void * elem2);

    char ** m_names;
    size_t m_nNames;
    size_t m_nNamesMax;
};

//-----------------------------------------------------------------------------
// Methods

TNameList::TNameList()
{
    m_nNamesMax = 0x10;
    m_nNames = 0;
    m_names = (char **)STORM_ALLOC(char *, m_nNamesMax);
}

TNameList::~TNameList()
{
    if(m_names != NULL)
    {
        for(size_t i = 0; i < m_nNames; i++)
            delete [] m_names[i];
        delete [] m_names;
    }
}

bool TNameList::InsertName(const char * szName)
{
    char ** m_newNames;

    if(m_names != NULL)
    {
        // Make sure we have enough space
        if(m_nNames >= m_nNamesMax)
        {
            m_nNamesMax = m_nNamesMax * 2;
            if((m_newNames = (char **)STORM_REALLOC(char *, m_names, m_nNamesMax)) == NULL)
                return false;
            m_names = m_newNames;
        }

        // Insert the string
        if((m_names[m_nNames++] = NewStr(szName)) != NULL)
        {
            return true;
        }
    }
    return false;
}

bool TNameList::CompareHash(const char * szHashExpected)
{
    hash_state md5_state;
    unsigned char md5_digest[MD5_DIGEST_SIZE];
    char szHashComputed[MD5_STRING_SIZE];

    // Perform MD5 hashing
    md5_init(&md5_state);
    for(size_t i = 0; i < m_nNames; i++)
    {
        size_t nLength = strlen(m_names[i]);
        md5_process(&md5_state, (unsigned char *)m_names[i], (unsigned long)(nLength + 1));
    }
    md5_done(&md5_state, md5_digest);

    // Convert to string
    BinaryToString(szHashComputed, _countof(szHashComputed), md5_digest, sizeof(md5_digest));

    // Compare the hashes
    if(_stricmp(szHashExpected, szHashComputed))
    {
        printf("Name hash mismatch (expected: %s, computed: %s)\n", szHashExpected, szHashComputed);
        return false;
    }

    return true;
}

void TNameList::Sort()
{
    if(m_names != NULL)
    {
        qsort(m_names, m_nNames, sizeof(char *), CompareTwoNames);
    }
}

char * TNameList::operator[](size_t nIndex) const
{
    return GetString(nIndex);
}

char * TNameList::GetString(size_t nIndex) const
{
    if(nIndex >= m_nNames)
        return NULL;
    return m_names[nIndex];
}

size_t TNameList::GetCount() const
{
    return m_nNames;
}

int __cdecl TNameList::CompareTwoNames(const void * elem1, const void * elem2)
{
    char * szName1 = *(char **)elem1;
    char * szName2 = *(char **)elem2;

    return strcmp(szName1, szName2);
}

char * TNameList::NewStr(const char * szName)
{
    char * szNewString = NULL;
    size_t nLength;

    if(szName != NULL)
    {
        nLength = strlen(szName);
        szNewString = new char[nLength + 1];
        if(szNewString != NULL)
        {
            StringCopy(szNewString, nLength + 1, szName);
            szNewString[nLength] = 0;
            _strlwr(szNewString);
        }
    }

    return szNewString;
}

/*
    TNameList nameList;

    nameList.InsertName("(attributes)");
    nameList.InsertName("war3map.w3e");
    nameList.InsertName("war3map.wpm");
    nameList.InsertName("(listfile)");
    nameList.InsertName("user32.dll");
    nameList.InsertName("war3map.shd");
    nameList.InsertName("config.sys");
    nameList.InsertName("Ladislav");
    nameList.InsertName("kernel32.dll");
    nameList.InsertName("ayayayay");
    nameList.InsertName("war3mapUnits.doo");
    nameList.InsertName("autoexec.bat");
    nameList.InsertName("betadine");
    nameList.InsertName("war3map.wts");
    nameList.InsertName("huhulinator");
    nameList.InsertName("advapi32.doo");
    nameList.InsertName("advapi32.dll");
    nameList.InsertName("advapi32.ocx");
    nameList.Sort();

    for(size_t i = 0; i < nameList.getCount(); i++)
    {
        printf("%s\n", nameList[i]);
    }
*/
