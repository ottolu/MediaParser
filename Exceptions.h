#pragma once

#include <exception>

namespace MediaParser 
{
	class ParserException : public std::exception
	{
	protected:
		int code;

	public:
		enum
		{
			NO_INPUT_FILE = 0,
			PARSE_LENGTH_INCORRECT = 1,
			META_RULE_MISSED = 2,
			CANNOT_FIND_TRACK_BOX = 3,
			CANNOT_FIND_STBL_BOX = 4,
			CANNOT_FIND_STSC_BOX = 5,
			CANNOT_FIND_STSZ_BOX = 6,
			CANNOT_FIND_STCO_CO64_BOX = 7
		};

		ParserException();
		ParserException(int c);
		virtual const char* what() const throw();
	};

	class FileReaderException : public std::exception
	{
	protected:
		int code;

	public:
		enum
		{
			NO_FILE_NAME = 0,
			OPEN_ERROR = 1,
			BUFFER_OVERFLOW = 2
		};

		FileReaderException();
		FileReaderException(int c);
		virtual const char* what() const throw();
	};

	class FileWriterException : public std::exception
	{
	protected:
		int code;

	public:
		enum
		{
			NO_FILE_NAME = 0,
			OPEN_ERROR = 1
		};

		FileWriterException();
		FileWriterException(int c);
		virtual const char* what() const throw();
	};
}