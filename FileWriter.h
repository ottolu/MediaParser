#pragma once

#include <fstream>
#include <cstdint>

namespace MediaParser
{
	class FileWriter
	{
	protected:
		std::ofstream m_stream;
		uint32_t m_bufSize;
		uint64_t m_filesize;
		// 4MB Buffer
		char m_buf[4 * 1024 * 1024 / 8];

	public:
		FileWriter() :
			m_bufSize(0),
			m_filesize(0)
		{
			//
		}

		~FileWriter()
		{
			if (m_stream.is_open())
			{
				this->flush();
				m_stream.close();
			}
		}

		void open(char* filename);
		void write(const char* buffer, uint32_t size = 0);
		void flush();

		uint64_t getFileSize() const
		{
			return m_filesize;
		}
	};
}