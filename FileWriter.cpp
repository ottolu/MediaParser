#include "Stdafx.h"
#include "Exceptions.h"
#include "FileWriter.h"

using namespace MediaParser;

void FileWriter::open(char *filename)
{
	if (NULL == filename)
	{
		FileWriterException e = FileWriterException(FileWriterException::NO_FILE_NAME);

		throw e;
	}

	m_stream.open(filename, std::ios::out | std::ios::binary);
}

void FileWriter::write(const char *buffer, uint32_t size)
{
	if (size <= 0)
	{
		size = strlen(buffer);
	}

	uint32_t restSize = 4 * 1024 * 1024 / 8 - m_bufSize;

	if (size <= restSize)
	{
		memcpy_s(&(m_buf[m_bufSize]), size, buffer, size);

		m_filesize += size;
		m_bufSize += size;
	}
	else
	{
		memcpy_s(&(m_buf[m_bufSize]), restSize, buffer, restSize);

		m_filesize += restSize;
		m_bufSize += restSize;

		this->flush();

		this->write(&(buffer[restSize]), size - restSize);
	}
}

void FileWriter::flush()
{
	m_stream.write(m_buf, m_bufSize);
	m_stream.flush();

	m_bufSize = 0;
}