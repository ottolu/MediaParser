#pragma once

#include "FileReader.h"
#include "Box.h"

namespace MediaParser
{
	class Parser
	{
	protected:
		FileReader *m_filereader;

	public:
		virtual void parse() = 0;

		FileReader* getFileReader()
		{
			return m_filereader;
		}
	};

	class MP4Parser : public Parser
	{
	protected:
		MPEG4Parser::ContainerBox *m_MP4Container;

	public:
		MP4Parser(char* filename);
		~MP4Parser();

		void parse();
		void mediaSplite();
	};
}