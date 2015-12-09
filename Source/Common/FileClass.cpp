#include "stdafx.h"

#if defined(_MSC_VER)
#include <crtdbg.h>
#else
#define _ASSERTE(expr)          ((void)0)
#endif

CFile::CFile() :
#ifdef _WIN32
m_hFile(INVALID_HANDLE_VALUE),
#else
m_hFile(NULL),
#endif
m_bCloseOnDelete(false)
{
}

CFile::CFile(void * hFile) :
    m_hFile(hFile),
    m_bCloseOnDelete(true)
{
    if (hFile == 0)
    {
        _ASSERTE(hFile != 0);
    }
}

CFile::CFile(const char * lpszFileName, uint32_t nOpenFlags) :
#ifdef _WIN32
    m_hFile(INVALID_HANDLE_VALUE),
#else
    m_hFile(NULL),
#endif
	m_bCloseOnDelete(true)
{
    Open(lpszFileName, nOpenFlags);
}

	CFile::~CFile()
{
#ifdef _WIN32
    if (m_hFile != INVALID_HANDLE_VALUE && m_bCloseOnDelete)
#else
    if (m_hFile != NULL && m_bCloseOnDelete)
#endif
	{
        Close();
    }
}

bool CFile::Open(const char * lpszFileName, uint32_t nOpenFlags)
{
    if (!Close())
    {
        return false;
    }

    if (lpszFileName == NULL || strlen(lpszFileName) == 0)
    {
        return false;
    }

    m_bCloseOnDelete = true;
#ifdef _WIN32
	m_hFile = INVALID_HANDLE_VALUE;

    ULONG dwAccess = 0;
    switch (nOpenFlags & 3)
    {
    case modeRead:
        dwAccess = GENERIC_READ;
        break;
    case modeWrite:
        dwAccess = GENERIC_WRITE;
        break;
    case modeReadWrite:
        dwAccess = GENERIC_READ|GENERIC_WRITE;
        break;
    default:
        _ASSERTE(false);
    }

    // map share mode
    ULONG dwShareMode = 0;

    dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    if ((nOpenFlags & shareDenyWrite) == shareDenyWrite) { dwShareMode &= ~FILE_SHARE_WRITE; }
    if ((nOpenFlags & shareDenyRead) == shareDenyRead)   { dwShareMode &= ~FILE_SHARE_READ; }
    if ((nOpenFlags & shareExclusive) == shareExclusive) { dwShareMode = 0; }

    // map modeNoInherit flag
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = (nOpenFlags & modeNoInherit) == 0;

    // map creation flags
    ULONG dwCreateFlag = OPEN_EXISTING;
    if (nOpenFlags & modeCreate)
    {
        dwCreateFlag = ((nOpenFlags & modeNoTruncate) != 0) ? OPEN_ALWAYS : CREATE_ALWAYS;
    }

    // attempt file creation
    HANDLE hFile = ::CreateFile(lpszFileName, dwAccess, dwShareMode, &sa, dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    { //#define ERROR_PATH_NOT_FOUND             3L
        //ULONG err = GetLastError();
        return false;
    }
    m_hFile = hFile;
#else
    m_hFile = NULL; //need to use fopen
#endif
	m_bCloseOnDelete = true;

    return true;
}

bool CFile::Close()
{
    bool bError = true;
#ifdef _WIN32
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        bError = !::CloseHandle(m_hFile);
    }
    m_hFile = INVALID_HANDLE_VALUE;
#else
	fclose((FILE *)m_hFile);
	m_hFile = NULL;
#endif
	m_bCloseOnDelete = false;
    return bError;
}

uint32_t CFile::SeekToEnd ( void )
{
    return Seek(0, CFile::end);
}

void CFile::SeekToBegin ( void )
{
    Seek(0, CFile::begin);
}

bool CFile::IsOpen( void ) const
{
#ifdef _WIN32
    return m_hFile != INVALID_HANDLE_VALUE;
#else
    return m_hFile != NULL;
#endif
}

bool CFile::Flush()
{
#ifdef _WIN32
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        return true;
    }

    return ::FlushFileBuffers(m_hFile) != 0;
#else
	return false;
#endif
}

bool CFile::Write(const void* lpBuf, uint32_t nCount)
{
    if (nCount == 0)
    {
        return true;     // avoid Win32 "null-write" option
    }

#ifdef _WIN32
    ULONG nWritten = 0;
    if (!::WriteFile(m_hFile, lpBuf, nCount, &nWritten, NULL))
    {
        return false;
    }

    if (nWritten != nCount)
    {
        // Win32s will not return an error all the time (usually DISK_FULL)
        return false;
    }
#else
	if (fwrite(lpBuf,1,nCount,(FILE *)m_hFile) != nCount)
	{
		return false;
	}
#endif
	return true;
}

uint32_t CFile::Read(void* lpBuf, uint32_t nCount)
{
    if (nCount == 0)
    {
        return 0;   // avoid Win32 "null-read"
    }

#ifdef _WIN32
    DWORD dwRead = 0;
    if (!::ReadFile(m_hFile, lpBuf, nCount, &dwRead, NULL))
    {
        return 0;
    }
    return (uint32_t)dwRead;
#else
	return 0;
#endif
}

long CFile::Seek(long lOff, SeekPosition nFrom)
{
#ifdef _WIN32
    ULONG dwNew = ::SetFilePointer(m_hFile, lOff, NULL, (ULONG)nFrom);
    if (dwNew  == (ULONG)-1)
    {
        return -1;
    }
    return dwNew;
#else
	return -1;
#endif
}

uint32_t CFile::GetPosition() const
{
#ifdef _WIN32
    return ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
#else
	return -1;
#endif
}

bool CFile::SetLength(uint32_t dwNewLen)
{
#ifdef _WIN32
    Seek((LONG)dwNewLen, begin);

    return ::SetEndOfFile(m_hFile) != 0;
#else
	return false;
#endif
}

uint32_t CFile::GetLength() const
{
#ifdef _WIN32
    return GetFileSize(m_hFile,0);
#else
	return 0;
#endif
}

bool CFile::SetEndOfFile()
{
#ifdef _WIN32
    return ::SetEndOfFile(m_hFile) != 0;
#else
	return false;
#endif
}