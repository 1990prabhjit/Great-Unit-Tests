// ZipArchive.cpp: implementation of the CZipArchive class.
//
////////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2000 Tadeusz Dracz.
//  For conditions of distribution and use, see copyright notice in ZipArchive.h
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipArchive.h"
#include <direct.h>
#include <stdlib.h> // for qsort
#include "windows.h"
#include <utility>

typedef std::pair<WORD, DWORD> dwPair;
#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
#endif
#define ENCR_HEADER_LEN 12
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TCHAR CZipArchive::m_gszCopyright[] = {_T("Zip archive creation and modification Copyright 2000 Tadeusz Dracz")};
CZipArchive::CZipArchive():
m_pCentralDir(new CZipCentralDir()),
m_centralDir(*m_pCentralDir),
m_pStorage(new CZipStorage()),
m_storage(*m_pStorage)
{
	m_bCloned=false;
	m_bDetectZlibMemoryLeaks = false; //true;

	m_centralDir.m_pStorage = m_pStorage;	

	m_info.m_stream.zalloc = (alloc_func)myalloc;
	m_info.m_stream.zfree = (free_func)myfree;
	m_iFileOpened = nothing;
	m_bFromStream = false;	
	//_ASSERTE(_CrtCheckMemory());
}
CZipArchive::CZipArchive(const CZipArchive& r):
m_pCentralDir(new CZipCentralDir(*r.m_pCentralDir)),
m_centralDir(*m_pCentralDir),
m_pStorage(new CZipStorage(*r.m_pStorage)),
m_storage(*m_pStorage)
{
	m_centralDir.m_pStorage = m_pStorage;	
	m_bDetectZlibMemoryLeaks = r.m_bDetectZlibMemoryLeaks;
	m_bFromStream = r.m_bFromStream;	
	m_iFileOpened = nothing ; //r.m_iFileOpened;
	for(int i=0;i<3;i++)
	{
		m_keys[i] = r.m_keys[i];
	}		
	m_pszPassword = r.m_pszPassword;
	m_info = r.m_info;
	m_bCloned=true;
}

CZipArchive::~CZipArchive()
{
	// 	Close(); // cannot be here: if an exception is thrown strange things can happen
	EmptyPtrList();
	if(m_pCentralDir != NULL)
	{
		delete m_pCentralDir; // Takes some time
	}
	if(m_pStorage != NULL)
	{
		delete m_pStorage;
	}	
}

void CZipArchive::Open(LPCTSTR szPathName, int iMode, int iVolumeSize)
{
	if (!IsClosed())
	{
		TRACE(_T("ZipArchive already opened.\n"));
		return;
	}
	m_pszPassword.Release();
	m_iFileOpened = nothing;
	m_storage.Open(szPathName, iMode, iVolumeSize);
	m_centralDir.Init();
	if ((iMode == open) ||(iMode == openReadOnly))
		m_centralDir.Read();

}

void CZipArchive::Open(CMemFile& mf, int iMode)
{
	if (!IsClosed())
	{
		TRACE(_T("ZipArchive already opened.\n"));
		return;
	}
	if (iMode != open && iMode != openReadOnly && iMode != create)
	{
		TRACE(_T("Mode not supported.\n"));
		return;
	}
	m_pszPassword.Release();
	m_iFileOpened = nothing;
	m_storage.Open(mf, iMode);
	m_centralDir.Init();
	if ((iMode == open) ||(iMode == openReadOnly))
		m_centralDir.Read();
}


bool CZipArchive::IsClosed(bool bArchive)
{
	return  bArchive ?(m_storage.GetCurrentDisk() == -1):(!m_storage.m_pFile || m_storage.m_pFile->m_hFile == CFile::hFileNull);
}


void CZipArchive::ThrowError(int err)
{
	AfxThrowZipException(err, IsClosed() ? _T("") : m_storage.m_pFile->GetFilePath());
}

bool CZipArchive::DeleteFile(WORD uIndex)
{
	if (m_storage.IsSpanMode())
	{
		TRACE(_T("You cannot delete files from the disk spannig archive.\n"));
		return false;
	}

	if (m_iFileOpened)
	{
		TRACE(_T("You cannot delete files if there is a file opened.\n"));
		return false;
	}

	if (!m_centralDir.IsValidIndex(uIndex))
		return false;

	m_info.Init();
	m_centralDir.RemoveFromDisk();
	DeleteInternal(uIndex);
	m_info.m_pBuffer.Release();
	return true;
}

int CZipArchive::GetNoEntries()
{
	return (int)m_centralDir.m_headers.GetSize();
}


bool CZipArchive::GetFileInfo(CZipFileHeader & fhInfo, WORD uIndex)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return false;
	}

	if (!m_centralDir.IsValidIndex(uIndex))
		return false;

	fhInfo = *(m_centralDir.m_headers[uIndex]);
	m_centralDir.ConvertFileName(true, false, &fhInfo);
	return true;
}

int CZipArchive::FindFile(CString szFileName)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return (int)-1;
	}
	// this is required for fast finding and is done only once
	if (!m_centralDir.m_bConvertAfterOpen)
	{
		TRACE(_T("Converting all filenames."));
		m_centralDir.ConvertAll();
	}
	if (!m_centralDir.m_bFindFastEnabled)
		EnableFindFast();
	int iResult = m_centralDir.FindFileNameIndex(szFileName);
	return iResult == -1 ? -1 : m_centralDir.m_findarray[iResult].m_uIndex;
}

bool CZipArchive::OpenFile(WORD uIndex)
{
	if (!m_centralDir.IsValidIndex(uIndex))
		return false;

	if (m_storage.IsSpanMode() == 1)
	{
		TRACE(_T("You cannot extract from the span in creation.\n"));
		return false;
	}


	if (m_iFileOpened)
	{
		TRACE(_T("A file already opened.\n"));
		return false;
	}

	m_info.Init();
	m_centralDir.OpenFile(uIndex);
	if (CurrentFile()->IsEncrypted())
	{

		if (m_pszPassword.GetSize() == 0)
		{
			TRACE(_T("Password not set for the encrypted file.\n"));
			return false;
		}
		CryptInitKeys();
		if (!CryptCheck())
			ThrowError(ZIP_BADPASSWORD); // invalid password

	}
	else if (m_pszPassword.GetSize() != 0)
	{
		TRACE(_T("Password set for a not encrypted file. Ignoring password.\n"));
	}

	WORD uMethod = CurrentFile()->m_uMethod;
	if ((uMethod != 0) &&(uMethod != Z_DEFLATED))
		ThrowError(ZIP_BADZIPFILE);

	if (uMethod == Z_DEFLATED)
	{
		m_info.m_stream.opaque =  m_bDetectZlibMemoryLeaks ? &m_list : 0;
		int err = inflateInit2(&m_info.m_stream, -MAX_WBITS);
		//			* windowBits is passed < 0 to tell that there is no zlib header.
		//          * Note that in this case inflate *requires* an extra "dummy" byte
		//          * after the compressed stream in order to complete decompression and
		//          * return Z_STREAM_END. 
		CheckForError(err);
	}
	m_info.m_uComprLeft = CurrentFile()->m_uComprSize;
	if (CurrentFile()->IsEncrypted())
		m_info.m_uComprLeft -= ENCR_HEADER_LEN;
	m_info.m_uUncomprLeft = CurrentFile()->m_uUncomprSize;
	m_info.m_uCrc32 = 0;
	m_info.m_stream.total_out = 0;
	m_info.m_stream.avail_in = 0;

	m_iFileOpened = extract;
	return true;
}


int CZipArchive::GetLocalExtraField(char *pBuf, int iSize)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return -1;
	}

	if (m_iFileOpened != extract)
	{
		TRACE(_T("A file must be opened to get the local extra field.\n"));
		return -1;
	}

	int size = m_centralDir.m_pLocalExtraField.GetSize();
	if (!pBuf|| !size)
		return size;

	if (iSize < size)
		size = iSize;

	memcpy(pBuf, m_centralDir.m_pLocalExtraField, size);
	return size;
}

void* CZipArchive::myalloc(void* opaque, UINT items, UINT size)
{
	void* p = new char[size * items];
	if (opaque)
	{
		CPtrList* list  = (CPtrList*) opaque;
		list->AddTail(p);
	}
	return p;
}

void CZipArchive::myfree(void* opaque, void* address)
{
	if (opaque)
	{
		CPtrList* list  = (CPtrList*) opaque;
		POSITION pos = list->Find(address);
		if (pos)
			list->RemoveAt(pos);
	}
	delete[] address;
}

void CZipArchive::CheckForError(int iErr)
{
	if ((iErr == Z_OK) ||(iErr == Z_NEED_DICT))
		return;

	ThrowError(iErr);
}

CZipFileHeader* CZipArchive::CurrentFile()
{
	ASSERT(m_centralDir.m_pOpenedFile);
	return m_centralDir.m_pOpenedFile;
}

DWORD CZipArchive::ReadFile(void *pBuf, DWORD iSize)
{
	if (m_iFileOpened != extract)
	{
		TRACE(_T("Current file must be opened.\n"));
		return 0;
	}

	if (!pBuf || !iSize)
		return 0;

	m_info.m_stream.next_out = (Bytef*)pBuf;
	m_info.m_stream.avail_out = iSize > m_info.m_uUncomprLeft 
		? m_info.m_uUncomprLeft : iSize;


	DWORD iRead = 0;

	// may happen when the file is 0 sized
	bool bForce = m_info.m_stream.avail_out == 0 && m_info.m_uComprLeft > 0;
	while (m_info.m_stream.avail_out > 0 || (bForce && m_info.m_uComprLeft > 0))
	{
		if ((m_info.m_stream.avail_in == 0) &&
			(m_info.m_uComprLeft > 0))
		{
			DWORD uToRead = m_info.m_pBuffer.GetSize();
			if (m_info.m_uComprLeft < uToRead)
				uToRead = m_info.m_uComprLeft;

			if (uToRead == 0)
				return 0;

			m_storage.Read(m_info.m_pBuffer, uToRead, false);
			CryptDecodeBuffer(uToRead);
			m_info.m_uComprLeft -= uToRead;

			m_info.m_stream.next_in = (unsigned char*)(char*)m_info.m_pBuffer;
			m_info.m_stream.avail_in = uToRead;
		}

		if (CurrentFile()->m_uMethod == 0)
		{
			DWORD uToCopy = m_info.m_stream.avail_out < m_info.m_stream.avail_in 
				? m_info.m_stream.avail_out : m_info.m_stream.avail_in;

			memcpy(m_info.m_stream.next_out, m_info.m_stream.next_in, uToCopy);

			m_info.m_uCrc32 = crc32(m_info.m_uCrc32, m_info.m_stream.next_out, uToCopy);

			m_info.m_uUncomprLeft -= uToCopy;
			m_info.m_stream.avail_in -= uToCopy;
			m_info.m_stream.avail_out -= uToCopy;
			m_info.m_stream.next_out += uToCopy;
			m_info.m_stream.next_in += uToCopy;
			m_info.m_stream.total_out += uToCopy;
			iRead += uToCopy;
		}
		else
		{
			DWORD uTotal = m_info.m_stream.total_out;
			Bytef* pOldBuf =  m_info.m_stream.next_out;
			int err = inflate(&m_info.m_stream, Z_SYNC_FLUSH);
			DWORD uToCopy = m_info.m_stream.total_out - uTotal;

			m_info.m_uCrc32 = crc32(m_info.m_uCrc32, pOldBuf, uToCopy);

			m_info.m_uUncomprLeft -= uToCopy;
			iRead += uToCopy;

			if (err == Z_STREAM_END)
				return iRead;

			CheckForError(err);
		}
	}

	return iRead;
}

void CZipArchive::Close(bool bAfterException)
{
	// if after an exception - the archive may be closed, but the file may be opened
	if (IsClosed() && (!bAfterException || IsClosed(false)))
	{
		TRACE(_T("ZipArchive is already closed.\n"));
		return;
	}

	if (!bAfterException)
	{
		if (m_iFileOpened == extract)
			CloseFile(NULL);

		if (m_iFileOpened == compress)
			CloseNewFile();

		// write central directory
		m_centralDir.Write();
	}
	else
	{
		m_info.m_pBuffer.Release();
		m_iFileOpened = nothing;
		EmptyPtrList();
	}

	m_centralDir.Clear();
	m_storage.Close(bAfterException);
}

void CZipArchive::SetSpanCallback(ZIPCALLBACKFUN pFunc, void* pData)
{
	m_storage.m_pZIPCALLBACKFUN = pFunc;
	m_storage.m_pCallbackData = pData;
}

void CZipArchive::SetAdvanced(int iWriteBuffer, int iExtractBuffer, int iSearchBuffer)
{
	if (!IsClosed())
	{
		TRACE(_T("Set this options before opening the archive.\n"));
		return;
	}

	m_storage.m_iWriteBufferSize = iWriteBuffer < 1024 ? 1024 : iWriteBuffer;
	m_centralDir.m_iBufferSize = iSearchBuffer < 1024 ? 1024 : iSearchBuffer;
	m_info.m_iBufferSize = iExtractBuffer < 1024 ? 1024 : iExtractBuffer;
}

int CZipArchive::CloseFile(CFile &file)
{
	CString temp = file.GetFilePath();
	file.Close();
	return CloseFile(temp);
}

int CZipArchive::CloseFile(LPCTSTR lpszFilePath, bool bAfterException)
{
	if (m_iFileOpened != extract)
	{
		TRACE(_T("No opened file.\n"));
		return false;
	}

	int iRet = 1;
	if (!bAfterException)
	{
		if (m_info.m_uUncomprLeft == 0)
		{
			if (m_info.m_uCrc32 != CurrentFile()->m_uCrc32)
				ThrowError(ZIP_BADCRC);
		}
		else
			iRet = -1;


		if (CurrentFile()->m_uMethod == Z_DEFLATED)
			inflateEnd(&m_info.m_stream);


		if (lpszFilePath)
		{
			try
			{
				CFileStatus fs;
				fs.m_ctime = fs.m_atime = CTime::GetCurrentTime();
				fs.m_attribute = 0;
				fs.m_mtime = CurrentFile()->GetTime();
				CFile::SetStatus(lpszFilePath, fs);
				if (SetFileAttributes(lpszFilePath, CurrentFile()->m_uExternalAttr) == 0)
					iRet = -2;
			}
			catch (CException* e)
			{
				e->Delete();
				return false;
			}
		}
	}
	m_centralDir.CloseFile();
	m_iFileOpened = nothing;
	m_info.m_pBuffer.Release();
	EmptyPtrList();
	return iRet;
}

bool CZipArchive::OpenNewFile(CZipFileHeader & header, int iLevel, LPCTSTR lpszFilePath)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return false;
	}

	if (m_iFileOpened)
	{
		TRACE(_T("A file already opened.\n"));
		return false;
	}

	if (m_storage.IsSpanMode() == -1)
	{
		TRACE(_T("You cannot add the files to the existing disk spannig archive.\n"));
		return false;
	}

	if (GetNoEntries() ==(WORD)USHRT_MAX)
	{
		TRACE(_T("Maximum file count inside archive reached.\n"));
		return false;
	}


	if (lpszFilePath)
	{
		bool bRet = true;
		CFileStatus fs;
		if(m_bFromStream)
		{
			CTime time = CTime::GetCurrentTime();
			header.SetTime(time);
		}
		else if (!CFile::GetStatus(lpszFilePath, fs))
			bRet = false;
		else
		{
			header.SetTime(fs.m_mtime);
			header.m_uExternalAttr = ::GetFileAttributes(lpszFilePath); // mfc bug: m_attribute is 1-byte
			if (header.m_uExternalAttr == -1)
				bRet = false;
		}

		if (!bRet)
			// do not continue - if the file was a directory then not recognizing it will cause 
			// serious errors 
			return false;
	}

	m_info.Init();
	m_centralDir.AddNewFile(header);

	CString szFileName = CurrentFile()->GetFileName();

	if (szFileName.IsEmpty())
	{
		szFileName.Format(_T("file%i"), GetNoEntries());;
		CurrentFile()->SetFileName(szFileName);
	}
	// this ensures the conversion will take place anyway (must take because we are going 
	// 	to write the local header in a moment
	m_centralDir.ConvertFileName(false, m_centralDir.m_bConvertAfterOpen);


	bool bIsDirectory = IsDirectory(CurrentFile()->m_uExternalAttr);
	bool bEncrypted = m_pszPassword.GetSize() != 0;
#ifdef _DEBUG
	if (bIsDirectory && bEncrypted)
		TRACE(_T("Warning! Encrypting a directory. Possible but pointless.\n\
				 Clear the password before adding a directory.\n"));
#endif	


	if (!CurrentFile()->PrepareData(iLevel, m_storage.IsSpanMode() == 1, bEncrypted))
		ThrowError(ZIP_TOOLONGFILENAME);

	CurrentFile()->WriteLocal(m_storage);
	// we have written the local header, but if we keep filenames not converted
	// in memory , we have to restore the non-converted value
	if (m_centralDir.m_bConvertAfterOpen)
		CurrentFile()->SetFileName(szFileName);
	if (bEncrypted)
	{
		CZipAutoBuffer buf(ENCR_HEADER_LEN);
		// use pseudo-crc since we don't know it yet
		CryptCryptHeader((long)header.m_uModTime << 16, buf);
		m_storage.Write(buf, ENCR_HEADER_LEN, false);
	}


	m_info.m_uComprLeft = 0;
	m_info.m_stream.avail_in = (uInt)0;
	m_info.m_stream.avail_out = (uInt)m_info.m_pBuffer.GetSize();
	m_info.m_stream.next_out = (unsigned char*)(char*)m_info.m_pBuffer;
	m_info.m_stream.total_in = 0;
	m_info.m_stream.total_out = 0;

	if (bIsDirectory && (CurrentFile()->m_uMethod != 0))
		CurrentFile()->m_uMethod = 0;

	if (CurrentFile()->m_uMethod == Z_DEFLATED)
	{
		m_info.m_stream.opaque = m_bDetectZlibMemoryLeaks ? &m_list : 0;

		int err = deflateInit2(&m_info.m_stream, iLevel,
			Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);

		CheckForError(err);
	}
	m_iFileOpened = compress;
	return true;
}

CString CZipArchive::GetFilePath(LPCTSTR lpszFilePath)
{
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	_tsplitpath(lpszFilePath, szDrive, szDir, NULL, NULL);
	return  CString(szDrive) + CString(szDir);
}

CString CZipArchive::GetFileExt(LPCTSTR lpszFilePath)
{
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(lpszFilePath, NULL, NULL, NULL, szExt);
	return CString(szExt);
}

CString CZipArchive::GetFileTitle(LPCTSTR lpszFilePath)
{
	TCHAR szFname[_MAX_FNAME];
	_tsplitpath(lpszFilePath, NULL, NULL, szFname, NULL);
	return  CString(szFname);
}

CString CZipArchive::GetFileDirAndName(LPCTSTR lpszFilePath)
{
	TCHAR szDir[_MAX_DIR];
	TCHAR szFname[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(lpszFilePath, NULL , szDir, szFname, szExt);
	CString Dir = szDir;
	Dir.TrimLeft(_T("\\"));
	return  Dir + szFname + szExt;
}


CString CZipArchive::GetFileName(LPCTSTR lpszFilePath)
{
	TCHAR szExt[_MAX_EXT];
	TCHAR szName[_MAX_FNAME];
	_tsplitpath(lpszFilePath, NULL, NULL, szName, szExt);
	return CString(szName) + CString(szExt);
}

bool CZipArchive::ForceDirectory(LPCTSTR lpDirectory)
{
	ASSERT(lpDirectory);
	CString szDirectory = lpDirectory;
	szDirectory.TrimRight(_T("\\"));
	if ((GetFilePath(szDirectory) == szDirectory) ||
		(FileExists(szDirectory) == -1))
		return true;
	if (!ForceDirectory(GetFilePath(szDirectory)))
		return false;
	if (!CreateDirectory(szDirectory, NULL))
		return false;
	return true;
}

int CZipArchive::FileExists(LPCTSTR lpszName)
{
	CFileFind fileFind;
	if (!fileFind.FindFile(lpszName))
	{
		if (DirectoryExists(lpszName))  // if root ex. "C:\"
			return -1;
		return 0;
	}
	fileFind.FindNextFile();
	return fileFind.IsDirectory() ? -1 : 1;
}

bool CZipArchive::DirectoryExists(LPCTSTR lpszDir)
{
	TCHAR curPath[_MAX_PATH];   /* Get the current working directory: */
	if (!_tgetcwd(curPath, _MAX_PATH))
		return false;
	if (_tchdir(lpszDir))	// retruns 0 if error
		return false;
	_tchdir(curPath);
	return true;
}

bool CZipArchive::IsFileDirectory(WORD uIndex)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return false;
	}

	if (!m_centralDir.IsValidIndex(uIndex))
		return false;

	return IsDirectory(m_centralDir.m_headers[uIndex]->m_uExternalAttr);
}
bool CZipArchive::ExtractFile(WORD uIndex,
							  LPCTSTR lpszPath,             
							  bool bFullPath,              
							  LPCTSTR lpszNewName,          
							  ZIPCALLBACKFUN pCallback,     
							  void* pUserData,              
							  DWORD nBufSize)
{
	if (!nBufSize && !lpszPath)
		return false;

	CZipFileHeader header;
	GetFileInfo(header, uIndex); // to ensure that slash and oem conversions take place
	CString szFile = lpszPath, szFileName = lpszNewName ? lpszNewName : header.GetFileName();
	szFile.TrimRight(_T("\\"));
	szFile += _T("\\") + (bFullPath ? GetFileDirAndName(szFileName)
		: GetFileName(szFileName)); // just in case in the archive there are file names with drives

	if (IsFileDirectory(uIndex))
	{
		ForceDirectory(szFile);
		SetFileAttributes(szFile, header.m_uExternalAttr);
		return true;
	}
	else
	{
		if (!OpenFile(uIndex))
			return false;

		ForceDirectory(GetFilePath(szFile));
		CFile f(szFile, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite);
		DWORD iRead, iFileLength = pCallback ? header.GetSize() : 0, iSoFar = 0;
		CZipAutoBuffer buf(nBufSize);
		do
		{
			iRead = ReadFile(buf, buf.GetSize());
			if (iRead)
			{	
				f.Write(buf, iRead);
				iSoFar += iRead;
				if (pCallback)
					if (!pCallback(iFileLength, iSoFar, pUserData))
						break;
			}
		}
		while (iRead == buf.GetSize());
		return CloseFile(f) == 1;
	}	
}

void CZipArchive::SetExtraField(char *pBuf, WORD iSize)
{
	if (m_iFileOpened != compress)
	{
		TRACE(_T("A new file must be opened.\n"));
		return;
	}
	if (!pBuf || !iSize)
		return;

	CurrentFile()->m_pExtraField.Allocate(iSize);
	memcpy(CurrentFile()->m_pExtraField, pBuf, iSize);
}

bool CZipArchive::WriteNewFile(void *pBuf, DWORD iSize)
{
	if (m_iFileOpened != compress)
	{
		TRACE(_T("A new file must be opened.\n"));
		return false;
	}


	m_info.m_stream.next_in = (unsigned char*)pBuf;
	m_info.m_stream.avail_in = iSize;
	CurrentFile()->m_uCrc32 = crc32(CurrentFile()->m_uCrc32, (unsigned char*)pBuf, iSize);


	while (m_info.m_stream.avail_in > 0)
	{
		if (m_info.m_stream.avail_out == 0)
		{
			CryptEncodeBuffer();
			m_storage.Write(m_info.m_pBuffer, m_info.m_uComprLeft, false);
			m_info.m_uComprLeft = 0;
			m_info.m_stream.avail_out = m_info.m_pBuffer.GetSize();
			m_info.m_stream.next_out = (unsigned char*)(char*)m_info.m_pBuffer;
		}

		if (CurrentFile()->m_uMethod == Z_DEFLATED)
		{
			DWORD uTotal = m_info.m_stream.total_out;
			int err = deflate(&m_info.m_stream,  Z_NO_FLUSH);
			CheckForError(err);
			m_info.m_uComprLeft += m_info.m_stream.total_out - uTotal;
		}
		else
		{
			DWORD uToCopy = (m_info.m_stream.avail_in < m_info.m_stream.avail_out) 
				? m_info.m_stream.avail_in : m_info.m_stream.avail_out;

			memcpy(m_info.m_stream.next_out, m_info.m_stream.next_in, uToCopy);

			m_info.m_stream.avail_in -= uToCopy;
			m_info.m_stream.avail_out -= uToCopy;
			m_info.m_stream.next_in += uToCopy;
			m_info.m_stream.next_out += uToCopy;
			m_info.m_stream.total_in += uToCopy;
			m_info.m_stream.total_out += uToCopy;
			m_info.m_uComprLeft += uToCopy;
		}
	}

	return true;
}

bool CZipArchive::CloseNewFile()
{
	if (m_iFileOpened != compress)
	{
		TRACE(_T("A new file must be opened.\n"));
		return false;
	}

	m_info.m_stream.avail_in = 0;

	int err = Z_OK;
	if (CurrentFile()->m_uMethod == Z_DEFLATED)
		while (err == Z_OK)
		{
			if (m_info.m_stream.avail_out == 0)
			{
				CryptEncodeBuffer();
				m_storage.Write(m_info.m_pBuffer, m_info.m_uComprLeft, false);
				m_info.m_uComprLeft = 0;
				m_info.m_stream.avail_out = m_info.m_pBuffer.GetSize();
				m_info.m_stream.next_out = (unsigned char*)(char*)m_info.m_pBuffer;
			}
			DWORD uTotal = m_info.m_stream.total_out;
			err = deflate(&m_info.m_stream,  Z_FINISH);
			m_info.m_uComprLeft += m_info.m_stream.total_out - uTotal;
		}

		if (err == Z_STREAM_END)
			err = Z_OK;
		CheckForError(err);

		if (m_info.m_uComprLeft > 0)
		{
			CryptEncodeBuffer();
			m_storage.Write(m_info.m_pBuffer, m_info.m_uComprLeft, false);
		}

		if (CurrentFile()->m_uMethod == Z_DEFLATED)
		{
			err = deflateEnd(&m_info.m_stream);
			CheckForError(err);
		}


		// it may be increased by the encrypted header size
		CurrentFile()->m_uComprSize += m_info.m_stream.total_out;
		CurrentFile()->m_uUncomprSize = m_info.m_stream.total_in;

		m_centralDir.CloseNewFile();
		m_iFileOpened = nothing;
		m_info.m_pBuffer.Release();
		return true;
}

void CZipArchive::DeleteFiles(CStringArray &aNames, bool bCaseSensitive)
{
	CWordArray indexes;

	for (WORD i = 0; i < GetNoEntries(); i++)
	{
		CZipFileHeader fh;
		GetFileInfo(fh, i);
		CString szFileName = fh.GetFileName();
		for (int j = 0; j < aNames.GetSize(); j++)
		{
			bool bEqual = (bCaseSensitive ? aNames[j].Collate(szFileName)
				: aNames[j].CollateNoCase(szFileName)) == 0;
			if (bEqual)
			{
				indexes.Add(i);
				break;
			}
		}
	}

	DeleteFiles(indexes);
}


void CZipArchive::DeleteFiles(CWordArray &aIndexes)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return;
	}

	if (m_storage.IsSpanMode())
	{
		TRACE(_T("You cannot delete files from the disk spannig archive.\n"));
		return;
	}

	if (m_iFileOpened)
	{
		TRACE(_T("You cannot delete files if there is a file opened.\n"));
		return;
	}

	// sorting the index table using qsort 
	int uSize = (int)aIndexes.GetSize();
	if (!uSize)
		return;

	m_centralDir.RemoveFromDisk();

	m_info.Init();
	DeleteInternalList(aIndexes);
	m_info.m_pBuffer.Release();
}

DWORD CZipArchive::RemovePackedFile(DWORD uStartOffset, DWORD uEndOffset)
{
	uStartOffset += m_centralDir.m_uBytesBeforeZip;
	uEndOffset += m_centralDir.m_uBytesBeforeZip;
	DWORD BytesToCopy = (DWORD)m_storage.m_pFile->GetLength() - uEndOffset;
	DWORD uTotalToWrite = BytesToCopy;

	char* buf = (char*)m_info.m_pBuffer;
	if (BytesToCopy > m_info.m_pBuffer.GetSize()) 
		BytesToCopy = m_info.m_pBuffer.GetSize();



	DWORD TotalWritten = 0;
	DWORD size_read;

	do
	{
		m_storage.m_pFile->Seek(uEndOffset + TotalWritten, CFile::begin);
		size_read = m_storage.m_pFile->Read(buf, BytesToCopy);
		if (size_read > 0)
		{
			m_storage.m_pFile->Seek(uStartOffset + TotalWritten, CFile::begin);
			m_storage.m_pFile->Write(buf, size_read);
		}
		TotalWritten += size_read;
	}
	while (uTotalToWrite != TotalWritten);
	//	while (size_read == BytesToCopy);
	//	if (uTotalToWrite != TotalWritten)
	//		ThrowError(CZipException::generic);
	DWORD uRemoved = (uEndOffset - uStartOffset);
	m_storage.m_pFile->SetLength(m_storage.m_pFile->GetLength() - uRemoved);
	return uRemoved;
}


DWORD CZipArchive::DeleteInternalList(CWordArray& uIndexs)
{
	//LARGE_INTEGER* arrLI = new LARGE_INTEGER[uIndexs.GetSize()];
	dwPair* pPairs = new dwPair[uIndexs.GetSize()];
	for(int num=0;num<uIndexs.GetSize();num++)
	{
		pPairs[num].second = m_centralDir.m_headers[uIndexs[num]]->m_uOffset;
		pPairs[num].first = uIndexs[num];
		ASSERT(pPairs[num].second != -1);
	}

	// Sort the deleted headers by their offsets
	qsort(pPairs, uIndexs.GetSize(), sizeof(dwPair), ComparePairs);
	for(int num=0;num<uIndexs.GetSize();num++)
	{
		uIndexs[num] = pPairs[num].first;
	}

	DWORD uTotalOffSet=0;
	DWORD uStartOffsetArray,uEndOffsetArray,uNextStartOffsetArray;
	for(int num=0;num<uIndexs.GetSize();num++)
	{
		// This is much harder than to do 
		//		DeleteInternal(uIndexs.GetAt(num)), isn't it? Oh well...

		CZipFileHeader* pfh = m_centralDir.m_headers[uIndexs.GetAt(num)];
		uStartOffsetArray = pfh->m_uOffset;
		uEndOffsetArray = uStartOffsetArray + pfh->GetLocalSize(); //  m_centralDir.m_headers[uIndexs.GetAt(num)]->m_uOffset;

		if(num == uIndexs.GetSize()-1)
			uNextStartOffsetArray = ((DWORD)m_storage.m_pFile->GetLength());
		else
			uNextStartOffsetArray=m_centralDir.m_headers[uIndexs.GetAt(num+1)]->m_uOffset;

		DWORD BytesToCopy = uNextStartOffsetArray - uEndOffsetArray;
		DWORD uTotalToWrite = BytesToCopy;

		char* buf = (char*)m_info.m_pBuffer;
		if (BytesToCopy > m_info.m_pBuffer.GetSize()) 
			BytesToCopy = m_info.m_pBuffer.GetSize();

		DWORD uStartOffset = uStartOffsetArray+m_centralDir.m_uBytesBeforeZip;
		DWORD uEndOffset = uEndOffsetArray+m_centralDir.m_uBytesBeforeZip;

		DWORD TotalWritten = 0;
		DWORD size_read;

		// Moving chunk of size <BytesToCopy> from <uEndOffset> to <uStartOffset-uTotalOffSet>
		do
		{
			m_storage.m_pFile->Seek(uEndOffset + TotalWritten, CFile::begin);
			size_read = m_storage.m_pFile->Read(buf, BytesToCopy);
			if (size_read > 0)
			{
				m_storage.m_pFile->Seek(uStartOffset-uTotalOffSet + TotalWritten, CFile::begin);
				m_storage.m_pFile->Write(buf, size_read);
			}
			TotalWritten += size_read;
		}
		while (uTotalToWrite > TotalWritten);
		ASSERT(uTotalToWrite == TotalWritten);
		uTotalOffSet += uEndOffset-uStartOffset;

		// Moving the offsets of relevant headers back <uTotalOffSet> bytes
		if(uTotalOffSet != 0)
		{
			DWORD headersToShift = (DWORD)m_centralDir.m_headers.GetSize();
			for(INT_PTR i=0; i < m_centralDir.m_headers.GetSize();i++) 
			{
				// printf("Entry #%d (%s) Position: %d\n", i, m_centralDir.m_headers[i]->GetFileName(), m_centralDir.m_headers[i]->m_uOffset);
				if(m_centralDir.m_headers[i]->m_uOffset > uStartOffset && m_centralDir.m_headers[i]->m_uOffset < uEndOffset+TotalWritten)
				{
					m_centralDir.m_headers[i]->m_uOffset -= uTotalOffSet;
					printf("New position: %d\n", m_centralDir.m_headers[i]->m_uOffset);
				}
			}
		}
	}

	for(INT_PTR count=uIndexs.GetSize()-1;count>=0;count--)
		m_centralDir.RemoveFile(uIndexs.GetAt(count));



	m_storage.m_pFile->SetLength(m_storage.m_pFile->GetLength() - uTotalOffSet);
	return uTotalOffSet;
}



void CZipArchive::DeleteInternal(WORD uIndex)
{
	CZipFileHeader* pfh = m_centralDir.m_headers[uIndex];
	DWORD uOtherOffsetChanged = 0;

	uOtherOffsetChanged = RemovePackedFile(pfh->m_uOffset, pfh->m_uOffset + pfh->GetLocalSize());

	// teraz uaktualnij offsety w pozosta�ych pozycjach central dir 
	// (update offsets in file headers in the central dir)
	if (uOtherOffsetChanged)
		for (int i = 0; i < GetNoEntries(); i++)
			if(m_centralDir.m_headers[i]->m_uOffset > pfh->m_uOffset)
				m_centralDir.m_headers[i]->m_uOffset -= uOtherOffsetChanged;

	m_centralDir.RemoveFile(uIndex);

}




bool CZipArchive::IsDriveRemovable(LPCTSTR lpszFilePath)
{
	return GetDriveType(GetDrive(lpszFilePath)) == DRIVE_REMOVABLE;
}

CString CZipArchive::GetDrive(LPCTSTR lpszFilePath)
{
	TCHAR szDrive[_MAX_DRIVE];
	_tsplitpath(lpszFilePath, szDrive, NULL, NULL, NULL);
	return szDrive;
}


bool CZipArchive::AddNewFile(LPCTSTR lpszFilePath,  
							 int iLevel,          
							 bool bFullPath,      
							 ZIPCALLBACKFUN pCallback,
							 void* pUserData,         
							 unsigned long nBufSize)
{
	return AddNewFile(lpszFilePath,lpszFilePath,iLevel,bFullPath,pCallback,pUserData,nBufSize);
}

bool CZipArchive::AddNewFile(LPCTSTR lpszFilePath,  
							 LPCTSTR lpszSubFilePath,
							 int iLevel,          
							 bool bFullPath,      
							 ZIPCALLBACKFUN pCallback,
							 void* pUserData,         
							 unsigned long nBufSize)
{
	if (!nBufSize)
		return false;

	CZipFileHeader header;
	header.SetFileName(bFullPath ? GetFileDirAndName(lpszSubFilePath) : GetFileName(lpszSubFilePath));
	if (header.GetFileNameSize() == 0)
		return false;
	if (!OpenNewFile(header, iLevel, lpszFilePath))
		return false;

	if (!IsDirectory(header.m_uExternalAttr))
	{
		CFile f;
		CFileException* e = new CFileException;
		BOOL bRet = f.Open(lpszFilePath, CFile::modeRead | CFile::shareDenyWrite, e);
		e->Delete();
		if (!bRet)
			return false;

		DWORD iRead, iFileLength = pCallback ? (DWORD)f.GetLength() : 0, iSoFar = 0;
		CZipAutoBuffer buf(nBufSize);
		do
		{
			iRead = f.Read(buf, nBufSize);
			if (iRead)
			{
				WriteNewFile(buf, iRead);
				iSoFar += iRead;
				if (pCallback)
					if (!pCallback(iFileLength, iSoFar, pUserData))
						break;
			}
		}
		while (iRead == buf.GetSize());
	}
	CloseNewFile();
	return true;
}

int CZipArchive::GetSpanMode()
{
	return m_storage.m_iSpanMode * m_storage.IsSpanMode();
}

CString CZipArchive::GetArchivePath()
{
	return m_storage.m_pFile->GetFilePath();
}

CString CZipArchive::GetGlobalComment()
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return _T("");
	}
	CString temp;	
	if (SingleToWide(m_centralDir.m_pszComment, temp) != -1)
		return temp;
	else 
		return _T("");
}

bool CZipArchive::SetGlobalComment(const CString &szComment)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return false;
	}
	if (m_storage.IsSpanMode() == -1)
	{
		TRACE(_T("You cannot modify the global comment of the existing disk spanning archive.\n"));
		return false;
	}

	WideToSingle(szComment, m_centralDir.m_pszComment);
	m_centralDir.RemoveFromDisk();
	return true;
}

bool CZipArchive::IsDirectory(DWORD uAttr)
{
	return (uAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

int CZipArchive::GetCurrentDisk()
{
	return m_storage.GetCurrentDisk() + 1;
}

bool CZipArchive::SetFileComment(WORD uIndex, CString szComment)
{
	if (IsClosed())
	{
		TRACE(_T("ZipArchive is closed.\n"));
		return false;
	}
	if (m_storage.IsSpanMode() == -1)
	{
		TRACE(_T("You cannot modify the file comment in the existing disk spanning archive.\n"));
		return false;
	}

	if (!m_centralDir.IsValidIndex(uIndex))
		return false;
	m_centralDir.m_headers[uIndex]->SetComment(szComment);
	m_centralDir.RemoveFromDisk();
	return true;
}

int CZipArchive::ComparePairs(const void *pArg1, const void *pArg2)
{
	dwPair p1 = *(dwPair*)pArg1;
	dwPair p2 = *(dwPair*)pArg2;
	return p1.second == p2.second ? 0 :(p1.second < p2.second ? - 1 : 1);
}

void CZipArchive::CryptInitKeys()
{
	ASSERT(m_pszPassword.GetSize());
	m_keys[0] = 305419896L;
	m_keys[1] = 591751049L;
	m_keys[2] = 878082192L;
	for (DWORD i = 0; i < m_pszPassword.GetSize(); i++)
		CryptUpdateKeys(m_pszPassword[i]);
}

void CZipArchive::CryptUpdateKeys(char c)
{

	m_keys[0] = CryptCRC32(m_keys[0], c);
	m_keys[1] += m_keys[0] & 0xff;
	m_keys[1] = m_keys[1] * 134775813L + 1;
	c = char(m_keys[1] >> 24);
	m_keys[2] = CryptCRC32(m_keys[2], c);
}

bool CZipArchive::CryptCheck()
{
	CZipAutoBuffer buf(ENCR_HEADER_LEN);
	m_storage.Read(buf, ENCR_HEADER_LEN, false);
	BYTE b = 0;
	for (int i = 0; i < ENCR_HEADER_LEN; i++)
	{
		b = buf[i]; // only temporary
		CryptDecode((char&)b);
	}
	// check the last byte
	if (CurrentFile()->IsDataDescr()) // Data descriptor present
		return BYTE(CurrentFile()->m_uModTime >> 8) == b;
	else
		return BYTE(CurrentFile()->m_uCrc32 >> 24) == b;
}

char CZipArchive::CryptDecryptByte()
{
	int temp = (m_keys[2] & 0xffff) | 2;
	return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

void CZipArchive::CryptDecode(char &c)
{
	c ^= CryptDecryptByte();
	CryptUpdateKeys(c);
}

bool CZipArchive::SetPassword(LPCTSTR lpszPassword)
{
	if (m_iFileOpened != nothing)
	{
		TRACE(_T("You cannot change the password when the file is opened\n"));
		return false; // it's important not to change the password when the file inside archive is opened
	}
	if (IsClosed())
	{
		TRACE(_T("Setting the password for a closed archive has no effect.\n"));
	}
	if (lpszPassword)
	{
		int iLen = WideToSingle(lpszPassword, m_pszPassword);
		if (iLen == -1)
			return false;
		for (size_t i = 0; (int)i < iLen; i++)
			if (m_pszPassword[i] > 127)
			{
				m_pszPassword.Release();
				TRACE(_T("The password contains forbidden characters. Password cleared.\n"));
				return false;
			}
	}
	else
		m_pszPassword.Release();
	return true;
}

DWORD CZipArchive::CryptCRC32(DWORD l, char c)
{
	const DWORD *CRC_TABLE = get_crc_table();
	return CRC_TABLE[(l ^ c) & 0xff] ^ (l >> 8);
}

void CZipArchive::CryptCryptHeader(long iCrc, CZipAutoBuffer &buf)
{
	CryptInitKeys();
	srand(UINT(GetTickCount()*time(NULL)));
	// genereate pseudo-random sequence
	char c;
	for (int i = 0; i < ENCR_HEADER_LEN - 2; i++)
	{
		int t1 = rand();
		c = (char)(t1 >> 6);
		if (!c)
			c = (char)t1;
		CryptEncode(c);
		buf[i] = c;

	}
	c = (char)((iCrc >> 16) & 0xff);
	CryptEncode(c);
	buf[ENCR_HEADER_LEN - 2] = c;
	c = (char)((iCrc >> 24) & 0xff);
	CryptEncode(c);
	buf[ENCR_HEADER_LEN - 1] = c;
}

void CZipArchive::CryptEncode(char &c)
{
	char t = CryptDecryptByte();
	CryptUpdateKeys(c);
	c ^= t;
}

void CZipArchive::CryptEncodeBuffer()
{
	if (CurrentFile()->IsEncrypted())
		for (DWORD i = 0; i < m_info.m_uComprLeft; i++)
			CryptEncode(m_info.m_pBuffer[i]);
}

void CZipArchive::CloseFileAfterTestFailed()
{
	if (m_iFileOpened != extract)
	{
		TRACE(_T("No file opened.\n"));
		return;
	}
	m_info.m_pBuffer.Release();
	m_centralDir.Clear(false);
	m_iFileOpened = nothing;
}

bool CZipArchive::TestFile(WORD uIndex, ZIPCALLBACKFUN pCallback, void* pUserData, DWORD uBufSize)
{
	if (!uBufSize)
		return false;
	CZipFileHeader* pHeader = m_centralDir.m_headers[uIndex];
	if (IsFileDirectory(uIndex))
	{

		// we do not test whether the password for the encrypted directory
		// is correct, since it seems to be senseless (anyway password 
		// encrypted directories should be avoided - it adds 12 bytes)
		DWORD iSize = pHeader->m_uComprSize;
		if ((iSize != 0 || iSize != pHeader->m_uUncomprSize)
			// different treating compressed directories
			&& !(pHeader->IsEncrypted() && iSize == 12 && !pHeader->m_uUncomprSize))
			ThrowError(CZipException::dirWithSize);
		return true;
	}
	else
	{
		try
		{
			if (!OpenFile(uIndex))
				return false;
			CZipAutoBuffer buf(uBufSize);
			DWORD iRead, iSoFar = 0;
			do
			{	
				iRead = ReadFile(buf, buf.GetSize());
				iSoFar += iRead;
				if (pCallback)
					if (!pCallback(pHeader->m_uUncomprSize, iSoFar, pUserData))
						break;
			}
			while (iRead == buf.GetSize());
			CloseFile();
		}
		catch(CException*)
		{
			CloseFileAfterTestFailed();
			throw;
		}
	}
	return true;

}

int CZipArchive::WideToSingle(LPCTSTR lpWide, CZipAutoBuffer &szSingle)
{
	size_t wideLen = _tcslen(lpWide);
	if (wideLen == 0)
	{
		szSingle.Release();
		return 0;
	}

#ifdef _UNICODE	
	// iLen does not include terminating character
	int iLen = WideCharToMultiByte(CP_ACP,0, lpWide, wideLen, szSingle, 
		0, NULL, NULL);
	if (iLen > 0)
	{
		szSingle.Allocate(iLen, true);
		iLen = WideCharToMultiByte(CP_ACP,0, lpWide , wideLen, szSingle, 
			iLen, NULL, NULL);
		ASSERT(iLen != 0);
	}
	else // here it means error
	{
		szSingle.Release();
		iLen --;
	}
	return iLen;

#else // if not UNICODE just copy
	// 	iLen does not include the NULL character
	szSingle.Allocate((DWORD)wideLen);
	memcpy(szSingle, lpWide, wideLen);
	return (int)wideLen;
#endif

}

int CZipArchive::SingleToWide(CZipAutoBuffer &szSingle, CString& szWide)
{
	int singleLen = szSingle.GetSize();
#ifdef _UNICODE	
	// iLen doesn't include terminating character
	int iLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSingle, singleLen, NULL, 0);
	if (iLen > 0)
	{
		iLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSingle, singleLen, 
			szWide.GetBuffer(iLen) , iLen);
		szWide.ReleaseBuffer(iLen);
		ASSERT(iLen != 0);
	}
	else
	{
		szWide.Empty();
		iLen --;
	}
	return iLen;

#else // if not UNICODE just copy
	// 	iLen does not include the NULL character
	memcpy(szWide.GetBuffer(singleLen), szSingle, singleLen);
	szWide.ReleaseBuffer(singleLen);
	return singleLen;
#endif
}

const DWORD* CZipArchive::GetCRCTable()
{
	return get_crc_table();
}

void CZipArchive::CryptDecodeBuffer(DWORD uCount)
{
	if (CurrentFile()->IsEncrypted())
		for (DWORD i = 0; i < uCount; i++)
			CryptDecode(m_info.m_pBuffer[i]);
}

void CZipArchive::EmptyPtrList()
{
	if (m_list.GetCount())
	{
		// if some memory hasn't been freed due to an error in zlib, so free it now
		POSITION pos = m_list.GetHeadPosition();
		while (pos)
			delete[] m_list.GetNext(pos);
	}

}

void CZipArchive::EnableFindFast(bool bEnable)
{
	if (IsClosed())
	{
		TRACE(_T("Set it after opening the archive"));
		return;
	}

	if (m_centralDir.m_bFindFastEnabled == bEnable)
		return;
	m_centralDir.m_bFindFastEnabled = bEnable;
	if (bEnable)
		m_centralDir.BuildFindFastArray();
	else
		m_centralDir.m_findarray.RemoveAll();
}

bool CZipArchive::WriteFile(CString filePath, void *pBuf, int length, int iLevel)
{

	m_bFromStream = true;
	CZipFileHeader header;
	header.SetFileName(GetFileDirAndName(filePath));
	if (header.GetFileNameSize() == 0)
		return false;

	if (!OpenNewFile(header, iLevel, filePath))
		return false;

	WriteNewFile(pBuf, length);

	CloseNewFile();
	m_bFromStream = false;
	return true;

}

int CZipArchive::GetFilesNum()
{
	EnableFindFast();
	return (int)m_centralDir.m_findarray.GetSize();
}

bool CZipArchive::IsOptimized()
{
	return m_pCentralDir->IsOptimized();
}

void CZipArchive::Optimize()
{
	if(!IsOptimized())
		m_centralDir.Optimize();
	FindDuplicates();
}

bool CZipArchive::getXML(streamVector* strmVec, IStream** pStream)
{
	OLVXML::IXMLDOMDocumentPtr retXml;
	streamVector::iterator vecIter, start, end;
	LARGE_INTEGER   nStreamPos;
	start = strmVec->begin();
	end = strmVec->end();
	HRESULT hr;
	hr = retXml.CreateInstance(OLVXML::CLSID_FreeThreadedDOMDocument60);
	retXml->async = VARIANT_FALSE;
	retXml->validateOnParse = VARIANT_FALSE;
	retXml->resolveExternals = VARIANT_FALSE;
	// Try concatenate fragments

	for ( vecIter = strmVec->begin( ); vecIter != strmVec->end( ); vecIter++ )
	{
		STATSTG sStreamStatus;
		ULARGE_INTEGER bytes2Copy, bytesRed, bytesWritten;

		hr = (*vecIter)->Stat(&sStreamStatus,  STATFLAG_NONAME &&  STATFLAG_NOOPEN);
		if (FAILED(hr))
			throw new _com_error(hr);

		bytes2Copy = sStreamStatus.cbSize;
		// Seek to the beginning of the stream
		memset(&nStreamPos, 0, sizeof(LARGE_INTEGER));

		hr = (*vecIter)->Seek(nStreamPos, STREAM_SEEK_SET, NULL);
		if (FAILED(hr))
			throw new _com_error(hr);

		hr = (*vecIter)->CopyTo(*pStream, bytes2Copy, &bytesRed, &bytesWritten);
		if (FAILED(hr))
			throw new _com_error(hr);

		ASSERT((bytes2Copy.HighPart == bytesRed.HighPart) &&
			(bytesRed.HighPart== bytesWritten.HighPart) &&
			(bytes2Copy.LowPart == bytesRed.LowPart) &&
			(bytesRed.LowPart == bytesWritten.LowPart));
	}
	memset(&nStreamPos, 0, sizeof(LARGE_INTEGER));

	hr = (*pStream)->Seek(nStreamPos, STREAM_SEEK_SET, NULL);
	if (FAILED(hr))
		throw new _com_error(hr);

	VARIANT_BOOL bLoaded = retXml->load(*pStream);
	if (VARIANT_TRUE == bLoaded)
	{
		return true;
	}// unsuccessful try permutations

	// Try all permutation until valid XML regained
	while (std::next_permutation(start, end))
	{
		(*pStream)->Release();
		(*pStream) = NULL;
		CreateStreamOnHGlobal(NULL, true, pStream);
		for ( vecIter = strmVec->begin( ); vecIter != strmVec->end( ); vecIter++ )
		{
			STATSTG sStreamStatus;
			ULARGE_INTEGER bytes2Copy, bytesRed, bytesWritten;

			hr = (*vecIter)->Stat(&sStreamStatus, STATFLAG_NONAME && STATFLAG_NOOPEN);
			if (FAILED(hr))
				throw new _com_error(hr);

			bytes2Copy = sStreamStatus.cbSize;
			// Seek to the beginning of the stream
			memset(&nStreamPos, 0, sizeof(LARGE_INTEGER));

			hr = (*vecIter)->Seek(nStreamPos, STREAM_SEEK_SET, NULL);
			if (FAILED(hr))
				throw new _com_error(hr);

			hr = (*vecIter)->CopyTo(*pStream, bytes2Copy, &bytesRed, &bytesWritten);
			if (FAILED(hr))
				throw new _com_error(hr);

			ASSERT((bytes2Copy.HighPart == bytesRed.HighPart) &&
				(bytesRed.HighPart== bytesWritten.HighPart) &&
				(bytes2Copy.LowPart == bytesRed.LowPart) &&
				(bytesRed.LowPart == bytesWritten.LowPart));
		}

		hr = (*pStream)->Seek(nStreamPos, STREAM_SEEK_SET, NULL);
		if (FAILED(hr))
			throw new _com_error(hr);

		VARIANT_BOOL bLoaded = retXml->load(*pStream);
		if (VARIANT_TRUE == bLoaded)
		{
			return true;
		}
	}
	//nothing found, release allocated resources
	return false;
}
bool CZipArchive::FixSplitFiles(storage oDuplicates)
{
	byte* binStream = NULL;
	CComPtr<IStream> pStream = NULL;
	IStream* pFinalStream = NULL;
	HRESULT hr = E_FAIL;
	streamVector strmVec;
	streamVector::iterator vecIter;
	try
	{
		storage :: const_iterator tIter = oDuplicates.begin();
		CString sFileName;
		int nCompressionLevel = -1;

		while( tIter != oDuplicates.end())
		{
			CreateStreamOnHGlobal(NULL, true, &pStream.p);
			CZipFileHeader	fh;
			WORD iFileIdx = (WORD)tIter->first;
			DWORD iLength = 0;

			sFileName = tIter->second;

			GetFileInfo(fh,iFileIdx);
			iLength = fh.m_uUncomprSize;
			if(!OpenFile(iFileIdx))
			{
				throw new _com_error(E_FAIL);
			}
			nCompressionLevel = fh.GetCompressionLevel();
			binStream = new byte[iLength];
			ReadFile(binStream,iLength);
			CloseFile();
			ULONG nBytesRed = 0;

			hr = pStream->Write(binStream, iLength, &nBytesRed);
			if (FAILED(hr))
				throw new _com_error(hr);

			strmVec.push_back(pStream);
			ASSERT(nBytesRed == iLength);
			if (NULL != binStream)
			{
				delete binStream;
				binStream = NULL;
			}
			tIter++;
		}
		CreateStreamOnHGlobal(NULL, true, &pFinalStream);

		if (!getXML(&strmVec, &pFinalStream))
		{
			// TODO: add logging
			throw new _com_error(E_FAIL);
		}
		STATSTG sStreamStatus;

		hr = pFinalStream->Stat(&sStreamStatus,  STATFLAG_NONAME &&  STATFLAG_NOOPEN);
		if (FAILED(hr))
			throw new _com_error(hr);

		ULONG iBytesWritten = 0;
		ULONG iFileSize = (ULONG)sStreamStatus.cbSize.LowPart;
		binStream = new byte[iFileSize];

		// Seek to the beginning of the stream
		LARGE_INTEGER   nStreamPos;
		memset(&nStreamPos, 0, sizeof(LARGE_INTEGER));

		hr = pFinalStream->Seek(nStreamPos, STREAM_SEEK_SET, NULL);
		if (FAILED(hr))
			throw new _com_error(hr);

		hr = pFinalStream->Read(binStream,iFileSize,&iBytesWritten);
		if (FAILED(hr))
			throw new _com_error(hr);

		ASSERT(iBytesWritten == iFileSize);
		// So far new fragments succesfully gathered, proceed with delete
		int nFile2Delete = FindFile(sFileName);
		while( nFile2Delete != -1)
		{
			DeleteFile((WORD)nFile2Delete);
			nFile2Delete = FindFile(sFileName);
		}
		WriteFile(sFileName, binStream, iBytesWritten, nCompressionLevel);
		strmVec.clear();
		if (NULL != binStream)
		{
			delete binStream;
			binStream = NULL;
		}
		if (NULL != pFinalStream)
		{
			pFinalStream->Release();
			pFinalStream = NULL;
		}
	}
	catch (...)
	{
		// TODO: Add error handling and reporting
		CloseFile();
		// Release resources
		if (NULL != binStream)
		{
			delete binStream;
			binStream = NULL;
		}
		if (NULL != pFinalStream)
		{
			pFinalStream->Release();
			pFinalStream = NULL;
		}
		strmVec.clear();
		return false;
	}

	// Release resources
	CloseFile();
	if (NULL != binStream)
	{
		delete binStream;
		binStream = NULL;
	}
	if (NULL != pFinalStream)
	{
		pFinalStream->Release();
		pFinalStream = NULL;
	}
	strmVec.clear();
	return true;
}

void CZipArchive::FindDuplicates()
{
	INT_PTR nCount = m_centralDir.m_findarray.GetCount();

	for (INT_PTR i = 0; i < nCount; i++)
	{
		CZipFindFast oDirEntry;
		oDirEntry = m_centralDir.m_findarray.ElementAt(i);
		std::map <int, CString> oDupMap;
		std::pair <int, CString> s_Pair;

		for (INT_PTR j = i+1; j < nCount; j++)
		{
			CZipFindFast oYetAnotherEntry;
			oYetAnotherEntry = m_centralDir.m_findarray.ElementAt(j);
			if (oDirEntry.m_pHeader->GetFileName().CompareNoCase(oYetAnotherEntry.m_pHeader->GetFileName()) == 0)
			{
				s_Pair.second = oYetAnotherEntry.m_pHeader->GetFileName();
				s_Pair.first = oYetAnotherEntry.m_uIndex;
				oDupMap.insert(s_Pair);
			}
		}
		if (oDupMap.size() > 0)
		{
			s_Pair.second = oDirEntry.m_pHeader->GetFileName();
			s_Pair.first = oDirEntry.m_uIndex;
			oDupMap.insert(s_Pair);
			FixSplitFiles(oDupMap);
			// indecies changed, iterate all from beginning
			//i = 0; // dont sure, just update the number of entries
			nCount = m_centralDir.m_findarray.GetCount();
		}
		oDupMap.clear();
	}
}