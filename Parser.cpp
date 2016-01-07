#include "Stdafx.h"
#include "SpecificBox.h"
#include "Parser.h"
#include "H264.h"

using namespace MediaParser;
using namespace MPEG4Parser;

MP4Parser::MP4Parser(char* filename)
{
	m_filereader = new FileReader();

	m_filereader->open(filename);

	m_MP4Container = new MPEG4Parser::ContainerBox("MP4C", UINT64_MAX, 0);
}

MP4Parser::~MP4Parser()
{
	m_filereader->close();

	delete m_filereader;
	delete m_MP4Container;
}

void MP4Parser::parse()
{
	m_MP4Container->parse(m_filereader, INT64_MAX);
}

void MP4Parser::mediaSplite()
{
	std::vector<Box*> trakBoxes;
	int trakBoxCount = m_MP4Container->findAllBoxes("trak", trakBoxes);

	if (trakBoxCount <= 0)
	{
		ParserException e = ParserException(ParserException::CANNOT_FIND_TRACK_BOX);

		throw e;
	}

	m_filereader->clear();

	// TODO:
	for (int i = 0; i < trakBoxCount; i++)
	{
		TrakBox* trakBox = (TrakBox*) trakBoxes[i];

		if (nullptr != trakBox->findBox("vmhd"))
		{
			std::shared_ptr<H264> h264(new H264());

			StsdBox* stsdBox = (StsdBox*) trakBox->findBox("stsd");
			stsdBox->parseStsd(m_filereader, h264);

			trakBox->parseTrack(m_filereader, h264);

			h264->extract(m_filereader, "test1.h264");
		}
		/*else
		{
			trakBox->parseTrack(m_filereader, "test.aac");
		}*/
	}
}