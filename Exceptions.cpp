#include "Stdafx.h"
#include "Exceptions.h"

using namespace MediaParser;

FileReaderException::FileReaderException() :
	code(-1)
{
	//
}

FileReaderException::FileReaderException(int c) :
	code(c)
{
	//
}

const char* FileReaderException::what() const throw()
{
	switch (code)
	{
	case FileReaderException::NO_FILE_NAME:
		return "ERROR: no input file name. (FileReaderException)";

	case FileReaderException::OPEN_ERROR:
		return "ERROR: file cannot open. (FileReaderException)";

	case FileReaderException::BUFFER_OVERFLOW:
		return "ERROR: stream buffer overflow. (FileReaderException)";

	default:
		return "ERROR: unknown exception. (FileReaderException)";
	}
}


ParserException::ParserException() :
	code(-1)
{
	//
}

ParserException::ParserException(int c) :
	code(c)
{
	//
}

const char* ParserException::what() const throw()
{
	switch (code)
	{
	case ParserException::NO_INPUT_FILE:
		return "ERROR: no input file. (ParserException)";

	case ParserException::PARSE_LENGTH_INCORRECT:
		return "ERROR: parse length incorrect. (ParserException)";

	case ParserException::META_RULE_MISSED:
		return "ERROR: meta rule missed. (ParserException)";

	case ParserException::CANNOT_FIND_TRACK_BOX:
		return "ERROR: cannot find trak boxes. (ParserException)";

	case ParserException::CANNOT_FIND_STBL_BOX:
		return "ERROR: cannot find stbl boxes. (ParserException)";

	case ParserException::CANNOT_FIND_STSC_BOX:
		return "ERROR: cannot find stsc boxes. (ParserException)";

	case ParserException::CANNOT_FIND_STSZ_BOX:
		return "ERROR: cannot find stsz boxes. (ParserException)";

	case ParserException::CANNOT_FIND_STCO_CO64_BOX:
		return "ERROR: cannot find stco & co64 boxes. (ParserException)";

	default:
		return "ERROR: unknown exception. (ParserException)";
	}
}


FileWriterException::FileWriterException() :
	code(-1)
{
	//
}

FileWriterException::FileWriterException(int c) :
	code(c)
{
	//
}

const char* FileWriterException::what() const throw()
{
	switch (code)
	{
	case FileWriterException::NO_FILE_NAME:
		return "ERROR: no input file name. (FileWriterException)";

	case FileWriterException::OPEN_ERROR:
		return "ERROR: file cannot open. (FileWriterException)";

	default:
		return "ERROR: unknown exception. (FileWriterException)";
	}
}