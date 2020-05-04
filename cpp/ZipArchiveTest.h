#if !defined(AFX_ZIPARCHIVE_H__A7F528A6_1872_4071_BE66_D56CC2DDE0E6__INCLUDED_)
#define AFX_ZIPARCHIVE_H__A7F528A6_1872_4071_BE66_D56CC2DDE0E6__INCLUDED_

//#include "ZipStorage.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>

//#include "ZipException.h"
//#include "ZipCentralDir.h"	// Added by ClassView
//#include "ZipInternalInfo.h"	// Added by ClassView

typedef unsigned char Bytef;


class IStream_test
{
public:

	class MockIStream_test
	{
	public:
		MockIStream_test()
		{
			pMockIStream_test = this;
		}

		virtual ~MockIStream_test()
		{
			pMockIStream_test = NULL;
		}

		static MockIStream_test* pMockIStream_test;

		MOCK_METHOD3(Seek, HRESULT(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition));
		MOCK_METHOD0(Release, void());
		MOCK_METHOD2(Stat, HRESULT(STATSTG* pstatstg, DWORD grfStatFlag));
		MOCK_METHOD4(CopyTo, HRESULT(IStream_test* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten));
		MOCK_METHOD3(Write, HRESULT(byte* buf, DWORD Len, ULONG* pByteRead));
		MOCK_METHOD3(Read, HRESULT(byte* buf, DWORD Len, ULONG* pByteRead));
	};

	IStream_test() {}
	virtual ~IStream_test() {}
	HRESULT Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		return MockIStream_test::pMockIStream_test->Seek(dlibMove, dwOrigin, plibNewPosition);
	}
	void Release()
	{
		// MockIStream_test::pMockIStream_test->Release();
	}
	HRESULT Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		return MockIStream_test::pMockIStream_test->Stat(pstatstg, grfStatFlag);
	}
	HRESULT CopyTo(IStream_test* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		return MockIStream_test::pMockIStream_test->CopyTo(pstm, cb, pcbRead, pcbWritten);
	}
	HRESULT Write(byte* buf, DWORD Len, ULONG* pByteRead)
	{
		return MockIStream_test::pMockIStream_test->Write(buf, Len, pByteRead);
	}
	HRESULT Read(byte* buf, DWORD Len, ULONG* pByteRead)
	{
		return MockIStream_test::pMockIStream_test->Read(buf, Len, pByteRead);
	}
	void AddRef() {}
	int val = 0;
	bool operator<(const IStream_test &b) const { return val < b.val; }
};

#define IStream IStream_test

class CFile_Test
{
public:
	static HANDLE hFileNull;
	HANDLE m_hFile;

	enum OpenFlags {
		modeRead = (int)0x00000,
		modeWrite = (int)0x00001,
		modeReadWrite = (int)0x00002,
		shareCompat = (int)0x00000,
		shareExclusive = (int)0x00010,
		shareDenyWrite = (int)0x00020,
		shareDenyRead = (int)0x00030,
		shareDenyNone = (int)0x00040,
		modeNoInherit = (int)0x00080,
#ifdef _UNICODE
		typeUnicode = (int)0x00400, // used in derived classes (e.g. CStdioFile) only
#endif
		modeCreate = (int)0x01000,
		modeNoTruncate = (int)0x02000,
		typeText = (int)0x04000, // used in derived classes (e.g. CStdioFile) only
		typeBinary = (int)0x08000, // used in derived classes (e.g. CStdioFile) only
		osNoBuffer = (int)0x10000,
		osWriteThrough = (int)0x20000,
		osRandomAccess = (int)0x40000,
		osSequentialScan = (int)0x80000,
	};

	enum Attribute {
		normal = 0x00,                // note: not same as FILE_ATTRIBUTE_NORMAL
		readOnly = FILE_ATTRIBUTE_READONLY,
		hidden = FILE_ATTRIBUTE_HIDDEN,
		system = FILE_ATTRIBUTE_SYSTEM,
		volume = 0x08,
		directory = FILE_ATTRIBUTE_DIRECTORY,
		archive = FILE_ATTRIBUTE_ARCHIVE,
		device = FILE_ATTRIBUTE_DEVICE,
		temporary = FILE_ATTRIBUTE_TEMPORARY,
		sparse = FILE_ATTRIBUTE_SPARSE_FILE,
		reparsePt = FILE_ATTRIBUTE_REPARSE_POINT,
		compressed = FILE_ATTRIBUTE_COMPRESSED,
		offline = FILE_ATTRIBUTE_OFFLINE,
		notIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
		encrypted = FILE_ATTRIBUTE_ENCRYPTED
	};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	class MockCFile_Test
	{
	public:
		MockCFile_Test()
		{
			pMockCFile_Test = this;
		}

		virtual ~MockCFile_Test()
		{
			pMockCFile_Test = NULL;
		}

		static MockCFile_Test* pMockCFile_Test;

		MOCK_METHOD0(GetLength, int());
		MOCK_METHOD3(Open, BOOL(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError));
		MOCK_METHOD2(Read, UINT(void* lpBuf, UINT nCount));
		MOCK_METHOD2(Write, void(const void* lpBuf, UINT nCount));
		MOCK_METHOD3(GetStatus, BOOL(LPCTSTR lpszFileName, CFileStatus& rStatus, CAtlTransactionManager* pTM));
		MOCK_METHOD3(SetStatus, void(LPCTSTR lpszFileName, const CFileStatus& status, CAtlTransactionManager* pTM));
		MOCK_METHOD0(GetFilePath, CString());
		MOCK_METHOD0(Close, void());
	};

	CFile_Test() {}
	CFile_Test(CString path, int) {}
	virtual ~CFile_Test() {}
	int GetLength()
	{
		return MockCFile_Test::pMockCFile_Test->GetLength();
	}
	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL)
	{
		return MockCFile_Test::pMockCFile_Test->Open(lpszFileName, nOpenFlags, pError);
	}
	UINT Read(void* lpBuf, UINT nCount)
	{
		return MockCFile_Test::pMockCFile_Test->Read(lpBuf, nCount);
	}
	void Write(const void* lpBuf, UINT nCount)
	{
		// MockCFile_Test::pMockCFile_Test->Write(lpBuf, nCount);
	}
	static BOOL GetStatus(LPCTSTR lpszFileName, CFileStatus& rStatus, CAtlTransactionManager* pTM = NULL)
	{
		return MockCFile_Test::pMockCFile_Test->GetStatus(lpszFileName, rStatus, pTM);
	}
	static void SetStatus(LPCTSTR lpszFileName, const CFileStatus& status, CAtlTransactionManager* pTM = NULL)
	{
		//return MockCFile_Test::pMockCFile_Test->SetStatus(lpszFileName, status, pTM);
	}
	CString GetFilePath()
	{
		return MockCFile_Test::pMockCFile_Test->GetFilePath();
	}
	void Close()
	{
		// MockCFile_Test::pMockCFile_Test->Close();
	}

};

HANDLE CFile_Test::hFileNull = NULL;
#define CFile CFile_Test

class CZipException : public CException
{
public:
	enum
	{
		noError,		// no error
		generic,		// unknown error
		streamEnd,			// zlib library errors
		needDict,			//
		errNo,				//
		streamError,		//
		dataError,			//
		memError,			//	
		bufError,			//
		versionError,		// 
		badZipFile,		// damaged or not a zip file
		badCrc,			// crc mismatched
		noCallback,		// no callback function set
		aborted,		// disk change callback function returned false
		nonRemovable,	// the disk selected for pkSpan archive is non removable
		tooManyVolumes,	// limit of the maximum volumes reached (999)
		tooLongFileName,	// the file name of the file added to the archive is too long
		badPassword,	// incorrect password set for the file being decrypted
		cdirNotFound,		///< the central directory was not found in the archive
		dirWithSize		// during testing: found the directory with the size greater than 0
	};
};

class CZipAutoBuffer
{
public:

	operator char*()
	{
		return m_pBuffer;
	}

	char* m_pBuffer;

	class MockCZipAutoBuffer
	{
	public:
		MockCZipAutoBuffer()
		{
			pMockCZipAutoBuffer = this;
		}

		virtual ~MockCZipAutoBuffer()
		{
			pMockCZipAutoBuffer = NULL;
		}

		static MockCZipAutoBuffer* pMockCZipAutoBuffer;

		MOCK_CONST_METHOD0(GetBuffer, const char*());
		MOCK_METHOD2(Allocate, char*(DWORD iSize, bool bZeroMemory));
		MOCK_METHOD0(Release, void());
		MOCK_CONST_METHOD0(GetSize, DWORD());
	};

	CZipAutoBuffer() {}
	CZipAutoBuffer(DWORD iSize, bool bZeroMemory = false)
	{
		m_pBuffer = new char[iSize];
		memset(m_pBuffer, 0, iSize);
	}
	virtual ~CZipAutoBuffer() {}
	const char* GetBuffer() const
	{
		return MockCZipAutoBuffer::pMockCZipAutoBuffer->GetBuffer();
	}
	char* Allocate(DWORD iSize, bool bZeroMemory = false)
	{
		return MockCZipAutoBuffer::pMockCZipAutoBuffer->Allocate(iSize, bZeroMemory);
	}
	void Release()
	{
		// MockCZipAutoBuffer::pMockCZipAutoBuffer->Release();
	}
	DWORD GetSize() const
	{
		return MockCZipAutoBuffer::pMockCZipAutoBuffer->GetSize();
	}
};

typedef bool(*ZIPCALLBACKFUN)(int iNumber, int iCode, void* pData);

class CZipBigFile : public CFile
{
public:

	class MockCZipBigFile
	{
	public:
		MockCZipBigFile(bool bToBeCloned = false)
		{
			pMockCZipBigFile = this;
		}

		virtual ~MockCZipBigFile()
		{
			pMockCZipBigFile = NULL;
		}

		static MockCZipBigFile* pMockCZipBigFile;

		MOCK_METHOD2(Seek, ULONGLONG(LONGLONG dOff, UINT nFrom));
		MOCK_METHOD1(SetLength, void(ULONGLONG dwNewLen));
		MOCK_METHOD2(Read, UINT(void* lpBuf, UINT nCount));
		MOCK_METHOD2(Write, void(const void* lpBuf, UINT nCount));
		MOCK_METHOD3(Open, BOOL(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError));
		MOCK_METHOD0(Close, void());
	};

	CZipBigFile() {}
	virtual ~CZipBigFile() {}
	ULONGLONG Seek(LONGLONG dOff, UINT nFrom)
	{
		return MockCZipBigFile::pMockCZipBigFile->Seek(dOff, nFrom);
	}
	void SetLength(ULONGLONG dwNewLen)
	{
		//return MockCZipBigFile::pMockCZipBigFile->SetLength(dwNewLen);
	}
	UINT Read(void* lpBuf, UINT nCount)
	{
		return MockCZipBigFile::pMockCZipBigFile->Read(lpBuf, nCount);
	}
	void Write(const void* lpBuf, UINT nCount)
	{
		//return MockCZipBigFile::pMockCZipBigFile->Write(lpBuf, nCount);
	}
	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL)
	{
		return MockCZipBigFile::pMockCZipBigFile->Open(lpszFileName, nOpenFlags, pError);
	}
	void Close()
	{
		//return MockCZipBigFile::pMockCZipBigFile->Close();
	}
};

class CZipStorage
{
public:
	int m_iWriteBufferSize;
	void* m_pCallbackData;
	CZipBigFile* m_pFile = NULL;
	int m_iSpanMode;
	ZIPCALLBACKFUN m_pZIPCALLBACKFUN;

	enum
	{
		noSpan,
		pkzipSpan,
		tdSpan,
		suggestedAuto,
		suggestedTd
	};

	class MockCZipStorage
	{
	public:
		MockCZipStorage()
		{
			pMockCZipStorage = this;
		}

		virtual ~MockCZipStorage()
		{
			pMockCZipStorage = NULL;
		}

		static MockCZipStorage* pMockCZipStorage;

		MOCK_METHOD2(Open, void(CMemFile& mf, int iMode));
		MOCK_METHOD1(Close, void(bool bAfterException));
		MOCK_METHOD0(GetCurrentDisk, int());
		MOCK_METHOD0(IsSpanMode, int());
		MOCK_METHOD3(Open, void(LPCTSTR szPathName, int iMode, int iVolumeSize));
		MOCK_METHOD3(Write, void(void* pBuf, DWORD iSize, bool bAtOnce));
		MOCK_METHOD3(Read, DWORD(void* pBuf, DWORD iSize, bool bAtOnce));
		MOCK_METHOD3(OpenFile, bool(LPCTSTR lpszName, UINT uFlags, bool bThrow));
		MOCK_METHOD1(ThrowError, void(int err));
	};

	CZipStorage()
	{
		m_pFile = new CZipBigFile();
	}
	virtual ~CZipStorage()
	{
		if (NULL != m_pFile)
		{
			delete m_pFile;
			m_pFile = NULL;
		}
	}
	void Open(CMemFile& mf, int iMode)
	{
		// MockCZipStorage::pMockCZipStorage->Open(mf, iMode);
	}
	void Close(bool bAfterException)
	{
		// MockCZipStorage::pMockCZipStorage->Close(bAfterException);
	}
	int GetCurrentDisk()
	{
		return MockCZipStorage::pMockCZipStorage->GetCurrentDisk();
	}
	int IsSpanMode()
	{
		return MockCZipStorage::pMockCZipStorage->IsSpanMode();
	}
	void Open(LPCTSTR szPathName, int iMode, int iVolumeSize)
	{
		// MockCZipStorage::pMockCZipStorage->Open(szPathName, iMode, iVolumeSize);
	}
	void Write(void *pBuf, DWORD iSize, bool bAtOnce)
	{
		// MockCZipStorage::pMockCZipStorage->Write(pBuf, iSize, bAtOnce);
	}
	DWORD Read(void* pBuf, DWORD iSize, bool bAtOnce)
	{
		return MockCZipStorage::pMockCZipStorage->Read(pBuf, iSize, bAtOnce);
	}
	bool OpenFile(LPCTSTR lpszName, UINT uFlags, bool bThrow = true)
	{
		return MockCZipStorage::pMockCZipStorage->OpenFile(lpszName, uFlags, bThrow);
	}
	void ThrowError(int err)
	{
		// MockCZipStorage::pMockCZipStorage->ThrowError(err);
	}
};

class CZipFileHeader
{
public:
	WORD m_uMethod;
	WORD m_uModTime;
	DWORD m_uCrc32;
	DWORD m_uComprSize;
	DWORD m_uUncomprSize = 0;
	DWORD m_uExternalAttr;
	DWORD m_uOffset;
	CZipAutoBuffer m_pExtraField;
	CZipAutoBuffer m_pszComment;

	class MockCZipFileHeader
	{
	public:
		MockCZipFileHeader()
		{
			pMockCZipFileHeader = this;
		}

		virtual ~MockCZipFileHeader()
		{
			pMockCZipFileHeader = NULL;
		}

		static MockCZipFileHeader* pMockCZipFileHeader;

		MOCK_METHOD0(GetCompressionLevel, int());
		MOCK_METHOD0(GetFileNameSize, WORD());
		MOCK_METHOD0(GetFileName, CString());
		MOCK_METHOD1(SetFileName, bool(LPCTSTR lpszFileName));
		MOCK_METHOD1(SetComment, bool(LPCTSTR lpszComment));
		MOCK_METHOD0(IsDataDescr, bool());
		MOCK_METHOD0(IsEncrypted, bool());
		MOCK_METHOD0(GetTime, CTime());
		MOCK_METHOD1(SetTime, void(const CTime& time));
		MOCK_METHOD0(GetSize, DWORD());
		MOCK_METHOD0(GetLocalSize, DWORD());
		MOCK_METHOD3(PrepareData, bool(int iLevel, bool bExtraHeader, bool bEncrypted));
		MOCK_METHOD1(WriteLocal, void(CZipStorage& storage));
		MOCK_METHOD1(Read, bool(CZipStorage* pStorage));
		MOCK_METHOD1(Write, DWORD(CZipStorage* pStorage));
	};

	CZipFileHeader() {}
	virtual ~CZipFileHeader() {}
	int GetCompressionLevel()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->GetCompressionLevel();
	}
	WORD GetFileNameSize()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->GetFileNameSize();
	}
	CString GetFileName()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->GetFileName();
	}
	bool SetFileName(LPCTSTR lpszFileName)
	{
		return MockCZipFileHeader::pMockCZipFileHeader->SetFileName(lpszFileName);
	}
	bool SetComment(LPCTSTR lpszComment)
	{
		return MockCZipFileHeader::pMockCZipFileHeader->SetComment(lpszComment);
	}
	bool IsDataDescr()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->IsDataDescr();
	}
	bool IsEncrypted()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->IsEncrypted();
	}
	CTime GetTime()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->GetTime();
	}
	void SetTime(const CTime& time)
	{
		// MockCZipFileHeader::pMockCZipFileHeader->SetTime(time);
	}
	DWORD GetSize()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->GetSize();
	}
	DWORD GetLocalSize()
	{
		return MockCZipFileHeader::pMockCZipFileHeader->GetLocalSize();
	}
	bool PrepareData(int iLevel, bool bExtraHeader, bool bEncrypted)
	{
		return MockCZipFileHeader::pMockCZipFileHeader->PrepareData(iLevel, bExtraHeader, bEncrypted);
	}
	void WriteLocal(CZipStorage& storage)
	{
		// MockCZipFileHeader::pMockCZipFileHeader->WriteLocal(storage);
	}
	bool Read(CZipStorage *pStorage)
	{
		return MockCZipFileHeader::pMockCZipFileHeader->Read(pStorage);
	}
	DWORD Write(CZipStorage *pStorage)
	{
		return MockCZipFileHeader::pMockCZipFileHeader->Write(pStorage);
	}
};

struct CZipFindFast
{
	CZipFindFast()
	{
		m_uIndex = 0;
		m_pHeader = NULL;
	}

	CZipFindFast(const CZipFindFast& r) :
		m_pHeader(r.m_pHeader),
		m_uIndex(r.m_uIndex)
	{}

	CZipFindFast(CZipFileHeader* pHeader, WORD uIndex) :
		m_pHeader(pHeader),
		m_uIndex(uIndex)
	{}

	/**
	We extract a name from it.
	*/
	CZipFileHeader* m_pHeader;

	/**
	The index in the central directory of the \e m_pHeader.
	*/
	WORD m_uIndex;

	CZipFindFast& operator=(const CZipFindFast& r)
	{
		if (this == &r)
			return *this;
		m_pHeader = r.m_pHeader;
		m_uIndex = r.m_uIndex;
		return *this;
	}

	CZipFindFast operator=(const CZipFindFast* r)
	{
		if (this == r)
			return *this;
		m_pHeader = r->m_pHeader;
		m_uIndex = r->m_uIndex;
		return *this;
	}
};

class CZipFindFastArray : public CArray<CZipFindFast, CZipFindFast>
{
public:

	class MockCZipFindFastArray
	{
	public:
		MockCZipFindFastArray()
		{
			pMockCZipFindFastArray = this;
		}

		virtual ~MockCZipFindFastArray()
		{
			pMockCZipFindFastArray = NULL;
		}

		static MockCZipFindFastArray* pMockCZipFindFastArray;

		MOCK_METHOD1(Find, int(LPCTSTR lpszFileName));
	};

	CZipFindFastArray() {}
	virtual ~CZipFindFastArray() {}
	int Find(LPCTSTR lpszFileName)
	{
		return MockCZipFindFastArray::pMockCZipFindFastArray->Find(lpszFileName);
	}
};

class CZipCentralDir
{
public:
	CZipStorage* m_pStorage = NULL;
	DWORD m_uOffset;
	CZipAutoBuffer m_pszComment;
	bool m_bFindFastEnabled;
	CZipFileHeader* m_pOpenedFile;
	DWORD m_uBytesBeforeZip;
	int m_iBufferSize;
	CTypedPtrArray<CPtrArray, CZipFileHeader*> m_headers;
	CZipAutoBuffer m_pLocalExtraField;
	CZipFindFastArray m_findarray;
	bool m_bConvertAfterOpen;

	class MockCZipCentralDir
	{
	public:
		MockCZipCentralDir()
		{
			pMockCZipCentralDir = this;
		}

		virtual ~MockCZipCentralDir()
		{
			pMockCZipCentralDir = NULL;
		}

		static MockCZipCentralDir* pMockCZipCentralDir;

		MOCK_METHOD1(RemoveFile, void(WORD uIndex));
		MOCK_METHOD1(Clear, void(bool bEverything));
		MOCK_METHOD0(CloseFile, void());
		MOCK_METHOD1(OpenFile, void(WORD uIndex));
		MOCK_METHOD1(IsValidIndex, bool(WORD uIndex));
		MOCK_METHOD0(Read, void());
		MOCK_METHOD0(Init, void());
		MOCK_METHOD0(CloseNewFile, void());
		MOCK_METHOD0(Write, void());
		MOCK_METHOD1(AddNewFile, void(CZipFileHeader & header));
		MOCK_METHOD0(RemoveFromDisk, void());
		MOCK_METHOD1(GetSize, DWORD(bool bWhole));
		MOCK_METHOD1(FindFileNameIndex, int(LPCTSTR lpszFileName));
		MOCK_METHOD0(BuildFindFastArray, void());
		MOCK_METHOD3(ConvertFileName, void(bool bFromZip, bool bAfterOpen, CZipFileHeader* pHeader));
		MOCK_METHOD0(ConvertAll, void());
		MOCK_METHOD1(ThrowError, void(int err));
		MOCK_METHOD0(IsOptimized, bool());
		MOCK_METHOD0(Optimize, void());
	};

	CZipCentralDir() {}
	CZipCentralDir(const CZipCentralDir& r) {}
	virtual ~CZipCentralDir() {}
	void RemoveFile(WORD uIndex)
	{
		// MockCZipCentralDir::pMockCZipCentralDir->RemoveFile(uIndex);
	}
	void Clear(bool bEverything = true)
	{
		// MockCZipCentralDir::pMockCZipCentralDir->Clear(bEverything);
	}
	void CloseFile()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->CloseFile();
	}
	void OpenFile(WORD uIndex)
	{
		// MockCZipCentralDir::pMockCZipCentralDir->OpenFile(uIndex);
	}
	bool IsValidIndex(WORD uIndex)
	{
		return MockCZipCentralDir::pMockCZipCentralDir->IsValidIndex(uIndex);
	}
	void Read()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->Read();
	}
	void Init()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->Init();
	}
	void CloseNewFile()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->CloseNewFile();
	}
	void Write()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->Write();
	}
	void AddNewFile(CZipFileHeader & header)
	{
		// MockCZipCentralDir::pMockCZipCentralDir->AddNewFile(header);
	}
	void RemoveFromDisk()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->RemoveFromDisk();
	}
	DWORD GetSize(bool bWhole = false)
	{
		return MockCZipCentralDir::pMockCZipCentralDir->GetSize(bWhole);
	}
	int FindFileNameIndex(LPCTSTR lpszFileName)
	{
		return MockCZipCentralDir::pMockCZipCentralDir->FindFileNameIndex(lpszFileName);
	}
	void BuildFindFastArray()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->BuildFindFastArray();
	}
	void ConvertFileName(bool bFromZip, bool bAfterOpen, CZipFileHeader* pHeader = NULL)
	{
		// MockCZipCentralDir::pMockCZipCentralDir->ConvertFileName(bFromZip, bAfterOpen, pHeader);
	}
	void ConvertAll()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->ConvertAll();
	}
	void ThrowError(int err)
	{
		// MockCZipCentralDir::pMockCZipCentralDir->ThrowError(err);
	}
	bool IsOptimized()
	{
		return MockCZipCentralDir::pMockCZipCentralDir->IsOptimized();
	}
	void Optimize()
	{
		// MockCZipCentralDir::pMockCZipCentralDir->Optimize();
	}
};

typedef void*(*alloc_func)(int iNumber, int iCode, void* pData);
typedef void(*free_func)(void* opaque, void* address);

class z_stream
{
public:
	int total_out = 0;
	int total_in = 0;
	DWORD avail_in = 0;
	DWORD avail_out = 0;
	unsigned char* next_in = NULL;
	unsigned char* next_out = NULL;

	alloc_func zalloc;
	free_func zfree;

	CPtrList* opaque;

	//unsigned char* total_in = NULL;
	//unsigned char* total_out = NULL;
};

class CZipInternalInfo
{
public:
	DWORD m_iBufferSize = 0;
	z_stream m_stream;
	DWORD m_uUncomprLeft = 0;
	DWORD m_uComprLeft = 0;
	DWORD m_uCrc32 = 0;
	CZipAutoBuffer m_pBuffer;
	void Init() {}
	CZipInternalInfo& operator=(const CZipInternalInfo& info)
	{
		return *this = info;
	}
};

namespace OLVXML
{
	enum DOM {
		CLSID_FreeThreadedDOMDocument60 = 1
	};

	class IXMLDOMDocument
	{
	public:
		VARIANT_BOOL async;
		VARIANT_BOOL validateOnParse;
		VARIANT_BOOL resolveExternals;

		class MockIXMLDOMDocument
		{
		public:
			MockIXMLDOMDocument()
			{
				pMockIXMLDOMDocument = this;
			}

			virtual ~MockIXMLDOMDocument()
			{
				pMockIXMLDOMDocument = NULL;
			}

			static MockIXMLDOMDocument* pMockIXMLDOMDocument;

			MOCK_METHOD1(load, short(IStream* file));
		};

		IXMLDOMDocument() {}
		virtual ~IXMLDOMDocument() {}

		short load(IStream* file)
		{
			return MockIXMLDOMDocument::pMockIXMLDOMDocument->load(file);
		}
	};

	class IXMLDOMDocumentPtr {
	public:
		static IXMLDOMDocument* m_pIXMLDOMDocument;

		IXMLDOMDocumentPtr(void *ptr = NULL) {}
		int CreateInstance(int doc) { return 0; }
		void Release() {}
		IXMLDOMDocument& operator*() const { return *m_pIXMLDOMDocument; }
		IXMLDOMDocument* operator->() const { return m_pIXMLDOMDocument; }
		operator bool() { return m_pIXMLDOMDocument != NULL; }
	};

	IXMLDOMDocument* IXMLDOMDocumentPtr::m_pIXMLDOMDocument = NULL;
};


class CZipArchive
{
public:
	virtual int GetFilesNum();
	virtual bool WriteFile(CString filePath, void* pBuf, int length, int iLevel = -1);
	static int SingleToWide(CZipAutoBuffer &szSingle, CString& szWide);
	static int WideToSingle(LPCTSTR lpWide, CZipAutoBuffer &szSingle);

	// Function name	: TestFile
	// Description	    : Test the file with the given index
	//		the function throws CException*, but performs all the necessary cleanup
	//		before, so that the next file can be tested after catchig the exception
	//		and examining it for the reason of error.
	// Return type		: bool 
	//		return false if the incorrect action has been taken by 
	//		the user or the programmer (it is when OpenFile(WORD nIndex) or 
	//		GetFileInfo(CZipFileHeader & fhInfo, WORD uIndex) return false or uBufSize
	//		is 0
	// Argument         : WORD uIndex
	//		index of the file to test
	// 	Argument         : ZIPCALLBACKFUN pCallback = NULL
	//		See the description at AddNewFile
	// 	Argument         : void* pUserData = NULL
	// Argument         : DWORD uBufSize = 65535
	//		the size of the buffer used during extraction
	virtual bool TestFile(WORD uIndex, ZIPCALLBACKFUN pCallback = NULL, void* pUserData = NULL, DWORD uBufSize = 65535);

	// Function name	: CloseFileAfterTestFailed
	// Description	    : Perform the necessary cleanup after the exception 
	//		while testing the archive so that next files in the archive can be tested
	// Return type		: void 
	virtual void CloseFileAfterTestFailed();


	// Function name	: SetPassword
	// Description	    : set the password for the file that is going to be opened or created
	//						use this function BEFORE opening or adding a file					
	// Return type		: bool 
	//						return false if the password contains ASCII characters
	//						with values 128 or higher or the file inside archive is
	//						opened		
	// Argument         : LPCTSTR lpszPassword = NULL
	//						set it to NULL to clear password
	virtual bool SetPassword(LPCTSTR lpszPassword = NULL);

	// Function name	: SetAdvanced
	// Description	    :  set advanced options
	// Return type		: void 
	// Argument         : int iWriteBuffer = 65535
	//		buffer used during write operation to the disk, the bigger, the better;
	//		it is pointless, however, to set it bigger than the size of the volume;
	//		the optimal size is the size of the volume
	// Argument         : int iExtractBuffer = 16384
	//		set the size of the buffer used in extracting and compressing files
	//		default 16384, must not be < 1024
	//		set it before opening the archive
	// Argument         : int iSearchBuffer = 32768
	//		set the size of the buffer used in searching for the central dir
	//		useful with big archives, default 32768, must not be < 1024
	virtual void SetAdvanced(int iWriteBuffer = 65535, int iExtractBuffer = 16384, int iSearchBuffer = 32768);

	// Function name	: SetSpanCallback
	// Description	    : set callback function used during operations on a
	//		pkzip compatible disk spanning archive to change disks; 
	//		set it usualy before opening the archive for reading
	// Return type		: void 
	// Argument         : ZIPCALLBACKFUN pFunc
	//		for the description of callback function see CZipStorage.h
	// Argument         : void* pData
	//		user data to be passed to the callback function as the last parameter
	virtual void SetSpanCallback(ZIPCALLBACKFUN pFunc, void* pData = NULL);

	//	archive open modes	
	enum { open, openReadOnly, create, createSpan };

	//		Disk spanning archive modes:
	//		pkzip compatible mode (pkSpan):
	//			- only on removeble devices
	//			- autodetect the size of the volume
	//			- write a label to the disk
	//			- there is a need to set the span callback function
	//		TD mode (tdSpan):
	//			- may be created on non removable devices
	//			- uses user-defined volume size
	//			- no need to set the span callback function

	// Function name	: Open
	// Description	    : Open a zip archive
	// Return type		: void
	// Argument         : LPCTSTR szPathName
	//		Path to the archive
	// Argument         : int iMode= CZipArchive::open;
	//		open mode flags:
	//		open			open an existing archive
	//		openReadOnly	open an existing archive as read only file 
	//			(this mode is intended to use in self extract code)
	//			if you try to add or delete a file in this mode, an exception will be thrown
	//		create			create a new archive
	//		createSpan		create a disk spanning archive
	// Argument         : int iVolumeSize = 0
	//		if iMode == createSpan and iVolumeSize <= 0 then create disk spanning 
	//			archive in pkzip mode
	//		if iMode == createSpan and iVolumeSize > 0 then create disk spanning 
	//			archive in TD mode
	//		if iMode == open and the exisitng archive is a spanned archive
	//			the pkSpan mode is assumed if the archive is on a removable device
	//			or tdSpan otherwise;
	//			if you want to open tdSpan archive on a removable device, set iVolumeSize
	//			to a value different from 0	
	//		if iMode == create this argument doesn't matter
	virtual void Open(LPCTSTR szPathName, int iMode = open, int iVolumeSize = 0);

	// Function name	: Open
	// Description	    : Open or create an archive in CMemFile
	// Return type		: void 
	// Argument         : CMemFile& mf
	// Argument         : int iMode = open
	//		the following modes are valid:  open, openReadOnly, create
	virtual void Open(CMemFile& mf, int iMode = open);


	// Function name	: AddNewFile
	// Description	    : add quickly a new file to the archive
	// Return type		: bool 
	// Argument         : LPCTSTR lpszFilePath
	//		file to be added
	// Argument         : int iLevel = Z_DEFAULT_COMPRESSION
	//		the compression level (see OpenNewFile() for detailed desciption)
	// Argument			: bool bFullPath
	//		if true, include full path of the file inside the archive	
	// 	Argument         : ZIPCALLBACKFUN pCallback = NULL
	// callback function (may be NULL)
	// 	To set the callback function for this operation pass its pointer as the 
	// 	argument (do not use SetSpanCallback for it - its for different purposes).
	// 	The callback function, if set, is called after reading and writing one portion of data.
	// 	- the first argument (DWORD):
	// 			total number of bytes to read (the size of the file)
	// 	- the second one (int) :
	// 			total number bytes already read
	// 	- the third argument (void*): 
	// 		pUserData argument passed to #AddNewFile
	// 	Argument         : void* pUserData = NULL
	// 		user - defined data passed on to the callback function
	// 		(doesn't matter if there is no callback function defined)
	// Argument         : DWORD nBufSize = 65535
	//		the size of the buffer used during compression

	/*
	*/
	virtual bool AddNewFile(LPCTSTR lpszFilePath, int iLevel = -1, bool bFullPath = true, ZIPCALLBACKFUN pCallback = NULL, void* pUserData = NULL, unsigned long nBufSize = 65535);

	virtual bool AddNewFile(LPCTSTR lpszFilePath, LPCTSTR lpszSubFilePath, int iLevel = -1, bool bFullPath = true, ZIPCALLBACKFUN pCallback = NULL, void* pUserData = NULL, unsigned long nBufSize = 65535);


	// Function name	: OpenNewFile
	// Description	    : add a new file to the zip archive
	// Return type		: bool 
	//		return false in the following cases:
	//		- the lpszFilePath is not NULL and the file	attributes and data was not correctly retreived
	//		- a file is already opened for extraction or compression
	//		- archive is an existing disk span archive
	//		- maximum file count inside archive reached (65535)
	// Argument         : CZipFileHeader & header
	//		structure that have addtional information; the following fields are valid:
	//			- m_uMethod - file compression method; can be 0 (storing) or Z_DEFLATE (deflating)
	//				otherwise Z_DEFLATE is assumed
	//			- m_uModDate, m_uModTime - use SetTime method of CFileHeadeer to set them
	//				if lpszFilePath is not NULL this fields are updated automaticaly
	//			- m_uExternalAttr - attributes of the file
	//				if lpszFilePath is not NULL this field is updated automaticaly
	//			- m_szFileName - file name (may be with path) to be stored inside archive
	//				to represent this file
	//			- m_szComment - file comment
	//			- m_pExtraField - LOCAL extra field, use SetExtraField() after opening 
	//				a new file, but before closing it to set the extra field 
	//				in the header in the central directory
	//		other fields are ignored - they are updated automaticaly
	// Argument         : int iLevel = Z_DEFAULT_COMPRESSION
	//		the level of compression (-1, 0 - 9); named values:
	//		Z_DEFAULT_COMPRESSION	: -1
	//		Z_NO_COMPRESSION		: 0
	//		Z_BEST_SPEED			: 1
	//		Z_BEST_COMPRESSION		: 9
	// Argument         : LPCTSTR lpszFilePath = NULL
	//		the path to the file to retreive date and attributes from
	virtual bool OpenNewFile(CZipFileHeader & header, int iLevel = Z_DEFAULT_COMPRESSION, LPCTSTR lpszFilePath = NULL);

	// Function name	: WriteNewFile
	// Description	    : compress the contents of the buffer and write it to a new file
	// Return type		: bool 
	//		return false if the new file hasn't been opened
	// Argument         : void *pBuf
	//		buffer containing the data to be compressed and written
	// Argument         : DWORD iSize
	//		the size of the buffer
	virtual bool WriteNewFile(void *pBuf, DWORD iSize);

	// Function name	: SetExtraField
	// Description	    : set the extra field in the file header in the central directory
	//		must be used after opening a new file in the archive, but before closing it
	// Return type		: void 
	// Argument         : char *pBuf
	//		bufer with the data to be copied
	// Argument         : DWORD iSize
	//		size of the buffer
	virtual void SetExtraField(char *pBuf, WORD iSize);

	// Function name	: CloseNewFile
	// Description	    : close the new file in the archive
	// Return type		: bool 
	//		return false if no new file is opened
	// Argument         : bool bAfterException  = false
	//		set it to true if you want to reuse CZipArchive after is has thrown an exception
	virtual bool CloseNewFile();

	// Function name	: ExtractFile
	// Description	    : fast extracting
	// Return type		: bool 
	// Argument         : WORD uIndex
	//		the index of the file
	// Argument         : LPCTSTR lpszPath
	//		PATH only to extract the file to
	// Argument			: bool bFullPath = true
	//		extract the file with full path (if there is a path stored with the filename)
	//		or just with the filename alone
	//		(it means that the resulting file path is lpszPath + one of the above)	
	// Argument			: LPCTSTR lpszNewName = NULL
	//			if NULL the default file name is taken (from the archive)
	// 	Argument         : ZIPCALLBACKFUN pCallback = NULL
	//		See the description at AddNewFile
	// 	Argument         : void* pUserData = NULL
	// Argument         : DWORD nBufSize = 65535
	//		the size of the buffer used during extraction
	virtual bool ExtractFile(WORD uIndex, LPCTSTR lpszPath, bool bFullPath = true, LPCTSTR lpszNewName = NULL, ZIPCALLBACKFUN pCallback = NULL, void* pUserData = NULL, DWORD nBufSize = 65535);

	// Function name	: OpenFile
	// Description	    : open the file with the given index in the archive for extracting
	// Argument         : WORD uIndex
	// Return type		: bool 
	virtual bool OpenFile(WORD uIndex);

	// Function name	: ReadFile
	// Description	    : decompress currently opened file to the bufor
	// Return type		: DWORD 
	//		number of bytes read			
	// Argument         : void *pBuf
	//		buffer to receive data
	// Argument         : DWORD iSize
	//		the size of the buffer
	virtual DWORD ReadFile(void *pBuf, DWORD iSize);

	// Function name	: GetLocalExtraField
	// Description	    : get the local extra filed of the currently opened 
	//					  for extraction file in the archive
	// Return type		: int 
	//		if pBuf == NULL return the size of the local extra field
	// Argument         : char* pBuf
	//		the buffer to receive the data
	// Argument         : int iSize
	//		the size of the buffer
	virtual int GetLocalExtraField(char* pBuf, int iSize);

	// Function name	: CloseFile
	// Description	    : close current file  and update
	//		date and attribute information of CFile, closes CFile
	// Return type		: int
	//		see below
	// Argument         : CFile & file
	//		OPENED CFile structure of the extracted file
	virtual int CloseFile(CFile &file);


	/**
	Close the file opened for extraction in the archive and copy its date and
	attributes to the file pointed by \e lpszFilePath
	\param	lpszFilePath
	Points to the path of the file to have the date and attributes information updated.
	\param bAfterException
	Set to \c true to close the file inside archive after an exception has been
	thrown, to allow futher operations on the archive.
	\warning Close the file pointed by \e lpszFilePath before using this method,
	because the system may not be able to retrieve information from it.
	\return
	-  "1" = ok
	-  "-1" = some bytes left to uncompress - probably due to a bad password
	-  "-2" = setting extracted file date and attributes was not successful
	\note Throws exceptions.
	*/
	virtual int CloseFile(LPCTSTR lpszFilePath = NULL, bool bAfterException = false);

	// Function name	: DeleteFile
	// Description	    : delete the file with the given index
	// Return type		: bool 
	// Argument         : WORD uIndex
	//		index of the file to be deleted
	virtual bool DeleteFile(WORD uIndex);

	/*	delete files from the archive opened in the Delete mode specified by aIndexes
	or aNames

	aIndexes is a array of indexes of the files inside the archive;
	the index no. 0 is the first file in the archive

	aNames is a array of file names inside the archive; they must be the
	same as they apppear in the archive (the name and the path (if persists)
	is required, lower and upper case are not distinguished)

	*/
	virtual void DeleteFiles(CWordArray &aIndexes);
	virtual void DeleteFiles(CStringArray &aNames, bool bCaseSensitive = false);


	// Function name	: SetGlobalComment
	// Description	    : set the global comment in the archive
	// Return type		: bool 
	//		return false if the archive is closed or if it is an existing disk spanning archive
	// Argument         : const CString& szComment
	virtual bool SetGlobalComment(const CString& szComment);

	// Function name	: GetGlobalComment
	// Description	    : get the global commment
	// Return type		: CString 
	//		return an empty string if the archive is closed
	virtual CString GetGlobalComment();


	// Function name	: SetFileComment
	// Description	    : set the comment of the file with the given index
	// Return type		: bool 
	//		return false if the comment change is imposible
	// Argument         : WORD uIndex
	//		index of the file
	// Argument         : CString szComment
	//		comment to add
	virtual bool SetFileComment(WORD uIndex, CString szComment);

	// Function name	: GetArchivePath
	// Description	    : return the path of the currently opended archive volume
	// Return type		: CString 
	virtual CString GetArchivePath();

	// Function name	: GetCurrentDisk
	// Description	    : return the zero-base number of the current disk
	// Return type		: int 
	//		return -1 if there is no current disk (archive is closed)
	virtual int GetCurrentDisk();

	// Function name	: GetSpanMode
	// Description	    : return the disk spanning mode of the cuurrent archive
	// Return type		: int 
	//		CZipStorage::tdSpan		 == - 2 - exisitng TD compatible disk spanning
	//		CZipStorage::pkzipSpan	 == - 1 - exisitng pkzip compatible disk spanning
	//		CZipStorage::noSpan		 ==	  0 - no disk spanning
	//		CZipStorage::pkzipSpan	 ==   1 - pkzip compatible disk spanning in creation
	//		CZipStorage::tdSpan		 ==   2 - TD compatible disk spanning in creation
	virtual int GetSpanMode();

	// Function name	: IsFileDirectory
	// Description	    : check if the file with the given index is a directory
	// Argument         : WORD uIndex
	//		index of the file
	// Return type		: bool 
	//		return true if the file is a directory
	//		return false if the file is not a directory or if there is no file
	//		with the given index
	virtual bool IsFileDirectory(WORD uIndex);

	// Function name	: FindFile
	// Description	    : find the file in the archive
	// 	This function requires CZipCentralDir::m_bFindFastEnabled set to true
	// 	Use EnableFindFast()
	// Return type		: int
	//		the index of the file found or -1 if no file was found
	// Argument         : CString szFileName
	//		the name of the file to be found
	virtual int FindFile(CString szFileName);


	/*
	Function name	: EnableFindFast
	Description	    :
	Enable fast finding by the file name of the files inside the archive.
	Set CZipCentralDir::m_bFindFastEnabled to true, which is required by FindFile.
	Do not enable it, if you don't plan to use FindFile function

	Return type		: void
	Argument         : bool bEnable = true
	*/
	virtual void EnableFindFast(bool bEnable = true);


	/*
	Function name	: SetConvertAfterOpen
	Description	    : Set CZipCentralDir::m_bConvertAfterOpen value. Use before opening the archive
	see CZipCentralDir::m_bConvertAfterOpen
	Return type		: void
	Argument         : bool bConvertAfterOpen
	*/
	void SetConvertAfterOpen(bool bConvertAfterOpen)
	{
		if (!IsClosed())
		{
			TRACE(_T("Set it before opening the archive"));
			return;
		}
		m_centralDir.m_bConvertAfterOpen = bConvertAfterOpen;

	}


	// Function name	: GetFileInfo
	// Description	    : get the info of the file with the given index
	// Return type		: bool 
	//		true if successful
	// Argument         : CZipFileHeader & fhInfo
	//		structure to receive info
	// Argument         : WORD uIndex
	//		zero-based index of the file
	virtual bool GetFileInfo(CZipFileHeader & fhInfo, WORD uIndex);


	// Function name	: GetNoEntries
	// Description	    : get number of files in the archive
	// Return type		: int 
	virtual int GetNoEntries();

	// Function name	: Close
	// Description	    : close archive
	// Return type		: void 
	// Argument         : bool bAfterException  = false
	//		set it to true if you want to close and reuse CZipArchive after is has thrown an exception
	//		( it doesn't write any data to the file but only makes some cleaning then)
	virtual void Close(bool bAfterException = false);



	// Function name	: IsClosed
	// Description	    : test if the archive or the current volume file is closed
	// Return type		: bool 
	// Argument         : bool bArchive = true
	//		if true test for the whole archive, if false - for the volume file only
	virtual bool IsClosed(bool bArchive = true);

	// specify whether to control memory allocation and freeing by zlib library
	// strongly suggested to set it to true (default)
	// set it before opening a file (new or current) in the archive
	bool m_bDetectZlibMemoryLeaks;


	CZipArchive();
	CZipArchive(const CZipArchive& r);
	virtual ~CZipArchive();

	////////////////////////////////////////////////////////////////////////
	//////////////////////  static helper functions  ///////////////////////
	////////////////////////////////////////////////////////////////////////

	// Function name	: GetFileTitle
	// Description	    : get the title of the file
	// Return type		: CString 
	// Argument         : LPCTSTR lpszFilePath
	static CString GetFileTitle(LPCTSTR lpszFilePath);

	// Function name	: GetFileDirAndName
	// Description	    : get the directory and the file name from the file path
	// Return type		: static CString 
	// Argument         : LPCTSTR lpszFilePath
	static CString GetFileDirAndName(LPCTSTR lpszFilePath);

	// Function name	: GetDrive
	// Description	    : return the (drive:) part from the path
	// Return type		: static CString 
	// Argument         : LPCTSTR lpszFilePath
	static CString GetDrive(LPCTSTR lpszFilePath);

	// Function name	: IsDriveRemovable
	// Description	    : return true if the file, path or (disk:) specified by the
	//		argument is (on) a removable drive
	// Return type		: static bool 
	// Argument         : LPCTSTR lpszFilePath
	static bool IsDriveRemovable(LPCTSTR lpszFilePath);

	// Function name	: DirectoryExists
	// Description	    : check if the given directory exists
	// Return type		: static bool 
	// Argument         : LPCTSTR lpszDir
	static bool DirectoryExists(LPCTSTR lpszDir);


	// Function name	: FileExists
	// Description	    : check if the given file or directory exists
	// Return type		: static int 
	//		return -1 if the given file is a directory, 1 if is a file
	//		or 0 if there is no such a file
	// Argument         : LPCTSTR lpszName
	static int FileExists(LPCTSTR lpszName);

	// Function name	: ForceDirectory
	// Description	    : create nested directories with a single command
	// Return type		: static bool 
	// Argument         : LPCTSTR lpDirectory
	static bool ForceDirectory(LPCTSTR lpDirectory);

	// Function name	: GetFilePath
	// Description	    : get the path of the given file
	// Return type		: static CString 
	// Argument         : LPCTSTR strFilePath
	static CString GetFilePath(LPCTSTR lpszFilePath);


	// Function name	: GetFileExt
	// Description	    : return the file extension
	// Return type		: static CString 
	// Argument         : LPCTSTR lpszFilePath
	static CString GetFileExt(LPCTSTR lpszFilePath);


	// Function name	: GetFileName
	// Description	    : return the name of the file (title + extension)
	// Return type		: static CString 
	// Argument         : LPCTSTR lpszFilePath
	static CString GetFileName(LPCTSTR lpszFilePath);

	// just a helper for a various purposes
	static const DWORD* GetCRCTable();
	// function for a VERY advanced use - you normally never need that
	CZipStorage* GetStorage() { return &m_storage; }
	CZipCentralDir* m_pCentralDir;
	CZipCentralDir& m_centralDir;

	virtual void EmptyPtrList();
	virtual void CryptDecodeBuffer(DWORD uCount);
	virtual void CryptEncodeBuffer();
	virtual void CryptEncode(char &c);
	virtual void CryptCryptHeader(long iCrc, CZipAutoBuffer& buf);
	virtual DWORD CryptCRC32(DWORD l, char c);
	virtual void CryptDecode(char &c);
	virtual char CryptDecryptByte();
	virtual bool CryptCheck();
	virtual void CryptUpdateKeys(char c);
	virtual void CryptInitKeys();
	CZipAutoBuffer m_pszPassword;
	DWORD m_keys[3];

	bool m_bFromStream;
	static int ComparePairs(const void *pArg1, const void *pArg2);
	virtual bool IsDirectory(DWORD uAttr);
	virtual void DeleteInternal(WORD uIndex);
	virtual DWORD RemovePackedFile(DWORD uStartOffset, DWORD uEndOffset);
	virtual DWORD DeleteInternalList(CWordArray& uIndexs);
	virtual CZipFileHeader* CurrentFile();
	virtual void CheckForError(int iErr);
	CZipInternalInfo m_info;
	CZipStorage* m_pStorage;
	CZipStorage& m_storage;
	CPtrList m_list;
	static void* myalloc(void* opaque, UINT items, UINT size);
	static void myfree(void* opaque, void* address);
	enum { extract = -1, nothing, compress };
	// 0 - no file inside archive opened
	// -1 - current opened for extract
	// 1 - new opened for compression
	char m_iFileOpened;
	virtual void ThrowError(int err);
	static TCHAR m_gszCopyright[];

	// MIF
	bool m_bCloned; // if true : the instance is a clone of some other instance
	virtual bool IsOptimized();
	virtual void Optimize();

	typedef std::map <int, CString> storage;
	typedef std::vector<CComPtr<IStream>> streamVector;
	virtual bool getXML(streamVector* strmVec, IStream** pStream) { return 0; }
	virtual bool FixSplitFiles(storage oDuplicates) { return 0; }
	virtual void FindDuplicates() {}
};

class MockCZipArchive : public CZipArchive
{
public:
	MockCZipArchive() : CZipArchive()
	{

	}

	virtual ~MockCZipArchive()
	{

	}

	MOCK_METHOD0(GetFilesNum, int());
	MOCK_METHOD4(WriteFile, bool(CString filePath, void* pBuf, int length, int iLevel));
	MOCK_METHOD2(SingleToWide, int(CZipAutoBuffer& szSingle, CString& szWide));
	MOCK_METHOD2(WideToSingle, int(LPCTSTR lpWide, CZipAutoBuffer& szSingle));
	MOCK_METHOD4(TestFile, bool(WORD uIndex, ZIPCALLBACKFUN pCallback, void* pUserData, DWORD uBufSize));
	MOCK_METHOD0(CloseFileAfterTestFailed, void());
	MOCK_METHOD1(SetPassword, bool(LPCTSTR lpszPassword));
	MOCK_METHOD3(SetAdvanced, void(int iWriteBuffer, int iExtractBuffer, int iSearchBuffer));
	MOCK_METHOD2(SetSpanCallback, void(ZIPCALLBACKFUN pFunc, void* pData));
	MOCK_METHOD3(Open, void(LPCTSTR szPathName, int iMode, int iVolumeSize));
	MOCK_METHOD2(Open, void(CMemFile& mf, int iMode));
	MOCK_METHOD6(AddNewFile, bool(LPCTSTR lpszFilePath, int iLevel, bool bFullPath, ZIPCALLBACKFUN pCallback, void* pUserData, unsigned long nBufSize));
	MOCK_METHOD7(AddNewFile, bool(LPCTSTR lpszFilePath, LPCTSTR lpszSubFilePath, int iLevel, bool bFullPath, ZIPCALLBACKFUN pCallback, void* pUserData, unsigned long nBufSize));
	MOCK_METHOD3(OpenNewFile, bool(CZipFileHeader & header, int iLevel, LPCTSTR lpszFilePath));
	MOCK_METHOD2(WriteNewFile, bool(void* pBuf, DWORD iSize));
	MOCK_METHOD2(SetExtraField, void(char* pBuf, WORD iSize));
	MOCK_METHOD0(CloseNewFile, bool());
	MOCK_METHOD7(ExtractFile, bool(WORD uIndex, LPCTSTR lpszPath, bool bFullPath, LPCTSTR lpszNewName, ZIPCALLBACKFUN pCallback, void* pUserData, DWORD nBufSize));
	MOCK_METHOD1(OpenFile, bool(WORD uIndex));
	MOCK_METHOD2(ReadFile, DWORD(void* pBuf, DWORD iSize));
	MOCK_METHOD2(GetLocalExtraField, int(char* pBuf, int iSize));
	MOCK_METHOD1(CloseFile, int(CFile& file));
	MOCK_METHOD2(CloseFile, int(LPCTSTR lpszFilePath, bool bAfterException));
	MOCK_METHOD1(DeleteFile, bool(WORD uIndex));
	MOCK_METHOD1(DeleteFiles, void(CWordArray& aIndexes));
	MOCK_METHOD2(DeleteFiles, void(CStringArray& aNames, bool bCaseSensitive));
	MOCK_METHOD1(SetGlobalComment, bool(const CString& szComment));
	MOCK_METHOD0(GetGlobalComment, CString());
	MOCK_METHOD2(SetFileComment, bool(WORD uIndex, CString szComment));
	MOCK_METHOD0(GetArchivePath, CString());
	MOCK_METHOD0(GetCurrentDisk, int());
	MOCK_METHOD0(GetSpanMode, int());
	MOCK_METHOD1(IsFileDirectory, bool(WORD uIndex));
	MOCK_METHOD1(FindFile, int(CString szFileName));
	MOCK_METHOD1(EnableFindFast, void(bool bEnable));
	MOCK_METHOD2(GetFileInfo, bool(CZipFileHeader & fhInfo, WORD uIndex));
	MOCK_METHOD0(GetNoEntries, int());
	MOCK_METHOD1(Close, void(bool bAfterException));
	MOCK_METHOD1(IsClosed, bool(bool bArchive));
	MOCK_METHOD1(GetFileTitle, CString(LPCTSTR lpszFilePath));
	MOCK_METHOD1(GetFileDirAndName, CString(LPCTSTR lpszFilePath));
	MOCK_METHOD1(GetDrive, CString(LPCTSTR lpszFilePath));
	MOCK_METHOD1(IsDriveRemovable, bool(LPCTSTR lpszFilePath));
	MOCK_METHOD1(DirectoryExists, bool(LPCTSTR lpszDir));
	MOCK_METHOD1(FileExists, int(LPCTSTR lpszName));
	MOCK_METHOD1(ForceDirectory, bool(LPCTSTR lpDirectory));
	MOCK_METHOD1(GetFilePath, CString(LPCTSTR lpszFilePath));
	MOCK_METHOD1(GetFileExt, CString(LPCTSTR lpszFilePath));
	MOCK_METHOD1(GetFileName, CString(LPCTSTR lpszFilePath));
	MOCK_METHOD0(GetCRCTable, const DWORD*());
	MOCK_METHOD0(EmptyPtrList, void());
	MOCK_METHOD1(CryptDecodeBuffer, void(DWORD uCount));
	MOCK_METHOD0(CryptEncodeBuffer, void());
	MOCK_METHOD1(CryptEncode, void(char& c));
	MOCK_METHOD2(CryptCryptHeader, void(long iCrc, CZipAutoBuffer& buf));
	MOCK_METHOD2(CryptCRC32, DWORD(DWORD l, char c));
	MOCK_METHOD1(CryptDecode, void(char& c));
	MOCK_METHOD0(CryptDecryptByte, char());
	MOCK_METHOD0(CryptCheck, bool());
	MOCK_METHOD1(CryptUpdateKeys, void(char c));
	MOCK_METHOD0(CryptInitKeys, void());
	MOCK_METHOD2(ComparePairs, int(const void* pArg1, const void* pArg2));
	MOCK_METHOD1(IsDirectory, bool(DWORD uAttr));
	MOCK_METHOD1(DeleteInternal, void(WORD uIndex));
	MOCK_METHOD2(RemovePackedFile, DWORD(DWORD uStartOffset, DWORD uEndOffset));
	MOCK_METHOD1(DeleteInternalList, DWORD(CWordArray& uIndexs));
	MOCK_METHOD0(CurrentFile, CZipFileHeader*());
	MOCK_METHOD1(CheckForError, void(int iErr));
	MOCK_METHOD3(myalloc, void*(void* opaque, UINT items, UINT size));
	MOCK_METHOD2(myfree, void(void* opaque, void* address));
	MOCK_METHOD1(ThrowError, void(int err));
	MOCK_METHOD0(IsOptimized, bool());
	MOCK_METHOD0(Optimize, void());
	MOCK_METHOD2(getXML, bool(streamVector* strmVec, IStream** pStream));
	MOCK_METHOD1(FixSplitFiles, bool(storage oDuplicates));
	MOCK_METHOD0(FindDuplicates, void());
};


void AfxThrowZipException(...) {}
int inflateInit2(...) { return 0; }
int crc32(...) { return 0; }
int inflate(...) { return Z_STREAM_END; }
void inflateEnd(...) {}
int deflateInit2(...) { return 0; }
int deflate(...) { return Z_STREAM_END; }
int deflateEnd(...) { return Z_STREAM_END; }
DWORD get_crc_table_buff[200];
DWORD * get_crc_table()
{
	memset(get_crc_table_buff, 0, sizeof(get_crc_table_buff));
	return get_crc_table_buff;
}

#define CreateDirectory CreateDirectory_Test
bool CreateDirectory_Test(...) { return true; }

#define GetDriveType GetDriveType_test
int GetDriveType_test(CString str) { return DRIVE_REMOVABLE; }

#define rand rand_test
int rand_test() { return 0; }

bool ZIPCALLBACKFUN_test(int iNumber, int iCode, void* pData) { return true; }

IStream_test IStream_test_test;
#define CreateStreamOnHGlobal CreateStreamOnHGlobal_test
void CreateStreamOnHGlobal_test(void* ptr, bool flag, IStream_test** pStream)
{
	*pStream = &IStream_test_test;
}

#endif // !defined(AFX_ZIPARCHIVE_H__A7F528A6_1872_4071_BE66_D56CC2DDE0E6__INCLUDED_)

