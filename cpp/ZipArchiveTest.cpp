#include <stdafx.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <ZipArchiveTest.h>

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::ReturnRef;
using ::testing::ReturnPointee;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;
using ::testing::Matcher;
using ::testing::_;
using ::testing::A;

CZipAutoBuffer::MockCZipAutoBuffer* CZipAutoBuffer::MockCZipAutoBuffer::pMockCZipAutoBuffer = NULL;
CZipFileHeader::MockCZipFileHeader* CZipFileHeader::MockCZipFileHeader::pMockCZipFileHeader = NULL;
CZipStorage::MockCZipStorage* CZipStorage::MockCZipStorage::pMockCZipStorage = NULL;
CZipBigFile::MockCZipBigFile* CZipBigFile::MockCZipBigFile::pMockCZipBigFile = NULL;
CZipCentralDir::MockCZipCentralDir* CZipCentralDir::MockCZipCentralDir::pMockCZipCentralDir = NULL;
CZipFindFastArray::MockCZipFindFastArray* CZipFindFastArray::MockCZipFindFastArray::pMockCZipFindFastArray = NULL;
OLVXML::IXMLDOMDocument::MockIXMLDOMDocument* OLVXML::IXMLDOMDocument::MockIXMLDOMDocument::pMockIXMLDOMDocument = NULL;
CFile_Test::MockCFile_Test* CFile_Test::MockCFile_Test::pMockCFile_Test = NULL;
IStream_test::MockIStream_test* IStream_test::MockIStream_test::pMockIStream_test = NULL;

#ifdef _DEBUG
#include <../../../OlvWebUtilsObject/Zip/ZipArchive.cpp>
#else
#include <../../../OlvWebUtilsObject/Zip/ZipArchive.cpp>
#endif

class MockCZipArchive_Tests : public ::testing::Test
{
public:
	std::shared_ptr<MockCZipArchive> pMockCZipArchive;

	CZipAutoBuffer::MockCZipAutoBuffer mockCZipAutoBuffer;
	CZipFileHeader::MockCZipFileHeader mockCZipFileHeader;
	CZipStorage::MockCZipStorage mockCZipStorage;
	CZipBigFile::MockCZipBigFile mockCZipBigFile;
	CZipCentralDir::MockCZipCentralDir mockCZipCentralDir;
	CZipFindFastArray::MockCZipFindFastArray mockCZipFindFastArray;
	CFile_Test::MockCFile_Test mockCFile_Test;
	OLVXML::IXMLDOMDocument::MockIXMLDOMDocument mockIXMLDOMDocument;
	IStream_test::MockIStream_test mockIStream_test;

	CString testCString;
	CZipFileHeader testCZipFileHeader;
	CZipFindFast testCZipFindFast;

	void SetUp() override
	{
		pMockCZipArchive = std::make_shared<MockCZipArchive>();

		pMockCZipArchive->m_centralDir.m_pOpenedFile = &testCZipFileHeader;

		testCString = "";
		testCZipFileHeader = CZipFileHeader();
		testCZipFindFast = CZipFindFast();

		OLVXML::IXMLDOMDocumentPtr::m_pIXMLDOMDocument = new OLVXML::IXMLDOMDocument();
		memset(get_crc_table_buff, 0, sizeof(get_crc_table_buff));
	}
	void TearDown() override
	{
		pMockCZipArchive.reset();

		if (NULL != OLVXML::IXMLDOMDocumentPtr::m_pIXMLDOMDocument)
		{
			delete OLVXML::IXMLDOMDocumentPtr::m_pIXMLDOMDocument;
			OLVXML::IXMLDOMDocumentPtr::m_pIXMLDOMDocument = NULL;
		}
	}
};

TEST_F(MockCZipArchive_Tests, IsClosed_FullLogic_false)
{
	//Arrange
	bool bArchive = true;

	EXPECT_CALL(mockCZipStorage, GetCurrentDisk)
		.Times(AtLeast(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::IsClosed(bArchive);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, Open_PathName_NotClosed_nothing)
{
	//Arrange
	LPCTSTR szPathName = "";
	int iMode = CZipArchive::open;
	int iVolumeSize = 0;

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1));

	//Act
	pMockCZipArchive->CZipArchive::Open(szPathName, iMode, iVolumeSize);

	//Assert
	ASSERT_EQ(CZipArchive::nothing, pMockCZipArchive->m_iFileOpened);
}

TEST_F(MockCZipArchive_Tests, Open_PathName_FullLogic_nothing)
{
	//Arrange
	LPCTSTR szPathName = "";
	int iMode = CZipArchive::open;
	int iVolumeSize = 0;

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));

	//Act
	pMockCZipArchive->CZipArchive::Open(szPathName, iMode, iVolumeSize);

	//Assert
	ASSERT_EQ(CZipArchive::nothing, pMockCZipArchive->m_iFileOpened);
}

TEST_F(MockCZipArchive_Tests, Open_CMemFile_NotClosed_nothing)
{
	//Arrange
	CMemFile mf;
	int iMode = CZipArchive::open;
	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1));

	//Act
	pMockCZipArchive->CZipArchive::Open(mf, iMode);

	//Assert
	ASSERT_EQ(CZipArchive::nothing, pMockCZipArchive->m_iFileOpened);
}

TEST_F(MockCZipArchive_Tests, Open_CMemFile_createSpan_nothing)
{
	//Arrange
	CMemFile mf;
	int iMode = CZipArchive::createSpan;
	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));

	//Act
	pMockCZipArchive->CZipArchive::Open(mf, iMode);

	//Assert
	ASSERT_EQ(CZipArchive::nothing, pMockCZipArchive->m_iFileOpened);
}

TEST_F(MockCZipArchive_Tests, Open_CMemFile_FullLogic_nothing)
{
	//Arrange
	CMemFile mf;
	int iMode = CZipArchive::open;
	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));

	//Act
	pMockCZipArchive->CZipArchive::Open(mf, iMode);

	//Assert
	ASSERT_EQ(CZipArchive::nothing, pMockCZipArchive->m_iFileOpened);
}

TEST_F(MockCZipArchive_Tests, ThrowError_FullLogic_NotNull)
{
	//Arrange
	int err = 0;

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1));
	EXPECT_CALL(mockCFile_Test, GetFilePath)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(testCString));

	//Act
	pMockCZipArchive->CZipArchive::ThrowError(err);

	//Assert
	ASSERT_NE((MockCZipArchive*)NULL, pMockCZipArchive.get());
}

TEST_F(MockCZipArchive_Tests, DeleteFile_IsSpanMode_false)
{
	//Arrange
	WORD uIndex = 0;

	EXPECT_CALL(mockCZipStorage, IsSpanMode)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::DeleteFile(uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, DeleteFile_open_false)
{
	//Arrange
	WORD uIndex = 0;
	pMockCZipArchive->m_iFileOpened = CZipArchive::openReadOnly;

	EXPECT_CALL(mockCZipStorage, IsSpanMode)
		.Times(AtLeast(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::DeleteFile(uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, DeleteFile_ValidIndex_false)
{
	//Arrange
	WORD uIndex = 0;

	EXPECT_CALL(mockCZipStorage, IsSpanMode)
		.Times(AtLeast(1));
	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::DeleteFile(uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, DeleteFile_FullLogic_true)
{
	//Arrange
	WORD uIndex = 0;

	EXPECT_CALL(mockCZipStorage, IsSpanMode)
		.Times(AtLeast(1));
	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));
	EXPECT_CALL(*pMockCZipArchive, DeleteInternal)
		.Times(AtLeast(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::DeleteFile(uIndex);

	//Assert
	ASSERT_TRUE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, GetNoEntries_FullLogic_Zero)
{
	//Act
	int returnedValue = pMockCZipArchive->CZipArchive::GetNoEntries();

	//Assert
	ASSERT_EQ(0, returnedValue);
}

TEST_F(MockCZipArchive_Tests, GetFileInfo_IsClosed_false)
{
	//Arrange
	CZipFileHeader fhInfo;
	WORD uIndex = 0;

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::GetFileInfo(fhInfo, uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, GetFileInfo_ValidIndex_false)
{
	//Arrange
	CZipFileHeader fhInfo;
	WORD uIndex = 0;

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1));
	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::GetFileInfo(fhInfo, uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, GetFileInfo_FullLogic_true)
{
	//Arrange
	CZipFileHeader fhInfo;
	WORD uIndex = 0;
	pMockCZipArchive->m_centralDir.m_headers.Add(&testCZipFileHeader);

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1));
	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::GetFileInfo(fhInfo, uIndex);

	//Assert
	ASSERT_TRUE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, FindFile_Closed_Negative)
{
	//Arrange
	CString szFileName;
	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));

	//Act
	int returnedValue = pMockCZipArchive->CZipArchive::FindFile(szFileName);

	//Assert
	ASSERT_EQ(-1, returnedValue);
}

TEST_F(MockCZipArchive_Tests, FindFile_FullLogic_Negative)
{
	//Arrange
	CString szFileName;
	pMockCZipArchive->m_centralDir.m_bConvertAfterOpen = false;
	pMockCZipArchive->m_centralDir.m_bFindFastEnabled = false;

	EXPECT_CALL(*pMockCZipArchive, IsClosed)
		.Times(AtLeast(1));
	EXPECT_CALL(*pMockCZipArchive, EnableFindFast)
		.Times(AtLeast(1));
	EXPECT_CALL(mockCZipCentralDir, FindFileNameIndex)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(-1));

	//Act
	int returnedValue = pMockCZipArchive->CZipArchive::FindFile(szFileName);

	//Assert
	ASSERT_EQ(-1, returnedValue);
}

TEST_F(MockCZipArchive_Tests, OpenFile_ValidIndex_false)
{
	//Arrange
	WORD uIndex = 0;

	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(false));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::OpenFile(uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, OpenFile_IsSpanMode_false)
{
	//Arrange
	WORD uIndex = 0;

	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));
	EXPECT_CALL(mockCZipStorage, IsSpanMode)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(1));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::OpenFile(uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

TEST_F(MockCZipArchive_Tests, OpenFile_openReadOnly_false)
{
	//Arrange
	WORD uIndex = 0;
	pMockCZipArchive->m_iFileOpened = CZipArchive::openReadOnly;

	EXPECT_CALL(mockCZipCentralDir, IsValidIndex)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(true));
	EXPECT_CALL(mockCZipStorage, IsSpanMode)
		.Times(AtLeast(1))
		.WillRepeatedly(Return(0));

	//Act
	bool returnedValue = pMockCZipArchive->CZipArchive::OpenFile(uIndex);

	//Assert
	ASSERT_FALSE(returnedValue);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
