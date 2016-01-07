#include "Stdafx.h"
#include "FileReader.h"

using namespace MediaParser;

void FileReader::open(char* filename)
{
	m_curPos = 0;

	if (NULL == filename)
	{
		FileReaderException e = FileReaderException(FileReaderException::NO_FILE_NAME);

		throw e;
	}

	m_stream.open(filename, std::ios::in | std::ios::binary);

	this->streamCheck();
}

// mostly read less than 16 bytes
char* FileReader::read(uint32_t bytesCount)
{
	this->streamCheck();

	if (bytesCount > m_bufSize - 1)
	{
		// FileReaderException e = FileReaderException(FileReaderException::BUFFER_OVERFLOW);

		// throw e;

		delete m_buf;

		m_buf = new char[bytesCount + 1];
	}

	m_stream.read(m_buf, bytesCount);
	
	// TODO:
	// should add judgement of `curPos` 
	// has been out of the file or not
	m_curPos += bytesCount;

	// help to cout the char*
	m_buf[bytesCount] = '\0';

	return m_buf;
}

bool FileReader::seek(int64_t deltaPos)
{
	this->streamCheck();

	uint64_t newPos = 0;

	if (deltaPos >= 0)
	{
		newPos = m_curPos + deltaPos;
	} 
	else 
	{
		deltaPos = -deltaPos;
		if ((uint64_t) deltaPos < m_curPos)
		{
			newPos = m_curPos - deltaPos;
		}
		else
		{
			newPos = 0;
		}
	}

	if (!m_stream.seekg(newPos))
	{
		return false;
	}
	else
	{
		m_curPos = newPos;
		return true;
	}
}

bool FileReader::seekAbs(uint64_t directPos)
{
	this->streamCheck();

	if (!m_stream.seekg(directPos))
	{
		return false;
	}
	else
	{
		m_curPos = directPos;
		return true;
	}
}

void FileReader::close()
{
	if (m_stream.is_open())
	{
		m_stream.close();
	}
}

float FileReader::readFloat()
{
	uint32_t binary;
	float    *flt;

	binary = this->readIntBigEndian(4, binary);
	flt    = (float*) &binary;

	return *flt;
}

double FileReader::readDouble()
{
	uint64_t binary;
	double   *dbl;

	binary   = this->readIntBigEndian(8, binary);
	dbl      = (double*) &binary;

	return   *dbl;
}

double FileReader::readFixedPointBigEndian(uint32_t integerLength, uint32_t fractionalLength)
{
	uint64_t n;
	uint64_t integer;
	uint64_t fractionalMask;
	double   fractional;

	n = this->readIntBigEndian(integerLength + fractionalLength, n);

	integer        = n >> fractionalLength;
	fractionalMask = (1 << fractionalLength)  - 1;
	fractional     = (double) (n & fractionalMask) / (1 << fractionalLength);

	return integer + fractional;
}

std::string* FileReader::readString(uint32_t bytesCount)
{
	this->read(bytesCount);

	return new std::string(m_buf, bytesCount);
}

std::string* FileReader::readNULLTerminatedString()
{
	char c;
	std::string *s = new std::string();

	do
	{
		c = this->get();
		s->append(1, c);
	} while (c != 0);

	return s;
}