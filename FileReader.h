#pragma once

#include <fstream>

#include "Tools.h"
#include "Types.h"
#include "Exceptions.h"

namespace MediaParser
{
	class FileReader
	{
	protected:
		std::ifstream m_stream;
		uint64_t m_curPos;
		uint32_t m_bufSize;
		char *m_buf;

		void streamCheck();

	public:
		FileReader(uint32_t bufSize) :
			m_curPos(0),
			m_bufSize(bufSize)
		{
			m_buf = new char[m_bufSize];
		}

		FileReader() :
			m_curPos(0),
			m_bufSize(1024 * 256) // 1MB Buffer
		{
			m_buf = new char[m_bufSize];
		}

		~FileReader()
		{
			if (m_stream.is_open())
			{
				m_stream.close();
			}

			delete m_buf;
		}

		void clear();
		bool good();
		bool eof();
		bool fail();
		bool bad();
		int peek();
		int get();

		void open(char* filename);
		char* read(uint32_t bytesCount);

		template <typename IntType>
		IntType& readIntBigEndian(uint32_t bytesCount, IntType& result);

		template <typename IntType>
		IntType& readIntLittleEndian(uint32_t bytesCount, IntType& result);

		float readFloat();
		double readDouble();

		double readFixedPointBigEndian(uint32_t integerLength, uint32_t fractionalLength);
		// double readFixedPointLittleEndian(uint32_t integerLength, uint32_t fractionalLength);

		// std::string* readUTF8String(uint32_t bytesCount);
		std::string* readString(uint32_t bytesCount);
		std::string* readNULLTerminatedString();

		bool seek(int64_t deltaPos = 0);
		bool seekAbs(uint64_t directPos = 0);
		bool ignore();
		void close();

		std::streampos tellg();
		std::istream& ignore(std::streamsize n = 1, int delim = EOF);
		std::istream& seekg(std::streampos pos);
		std::istream& seekg(std::streamoff off, std::ios_base::seekdir dir);

		std::ifstream& getStream()
		{
			return m_stream;
		}

		uint64_t getCurPos()
		{
			return m_curPos;
		}

		uint32_t getBufSize()
		{
			return m_bufSize;
		}

		char* getBuffer()
		{
			return m_buf;
		}
	};

	template <typename IntType>
	IntType& FileReader::readIntBigEndian(uint32_t bytesCount, IntType& result)
	{
		this->read(bytesCount);

		return bytesToIntegerBigEndian(this->m_buf, result, bytesCount);
	}

	template <typename IntType>
	IntType& FileReader::readIntLittleEndian(uint32_t bytesCount, IntType& result)
	{
		this->read(bytesCount);

		return bytesToIntegerLittleEndian(this->m_buf, result, bytesCount);
	}

	inline void FileReader::clear()
	{
		m_stream.clear();
	}

	inline bool FileReader::good()
	{
		return m_stream.good();
	}

	inline bool FileReader::eof()
	{
		return m_stream.eof();
	}

	inline bool FileReader::fail()
	{
		return m_stream.fail();
	}

	inline bool FileReader::bad()
	{
		return m_stream.bad();
	}

	inline int FileReader::peek()
	{
		return m_stream.peek();
	}

	inline int FileReader::get()
	{
		this->streamCheck();
		m_curPos++;
		return m_stream.get();
	}

	inline std::streampos FileReader::tellg()
	{
		return m_stream.tellg();
	}

	inline std::istream& FileReader::ignore(std::streamsize n, int delim)
	{
		return m_stream.ignore(n, delim);
	}

	inline std::istream& FileReader::seekg(std::streampos pos)
	{
		return m_stream.seekg(pos);
	}

	inline std::istream& FileReader::seekg(std::streamoff off, std::ios_base::seekdir dir)
	{
		return m_stream.seekg(off, dir);
	}

	inline void FileReader::streamCheck()
	{
		if (!m_stream.is_open() || !m_stream.good())
		{
			FileReaderException e = FileReaderException(FileReaderException::OPEN_ERROR);

			throw e;
		}
	}
}