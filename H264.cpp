#include "Stdafx.h"
#include "H264.h"
#include "FileWriter.h"

using namespace MPEG4Parser;

void H264::extract(FileReader *filereader, char *filename)
{
	uint8_t NALUType;
	//std::string iframePos;
	char startCode[4] = { 0, 0, 0, 1 };

	//
	std::shared_ptr<FileWriter> filewriter(new FileWriter());

	filewriter->open(filename);

	uint32_t chkEntryLength, NALULength = 0;

	filewriter->write(startCode, 4);
	filewriter->write(m_SPS->c_str(), m_SPS->size());

	filewriter->write(startCode, 4);
	filewriter->write(m_PPS->c_str(), m_PPS->size());

	std::for_each(std::begin(*m_chunkEntries), std::end(*m_chunkEntries),
		[&](ChunkEntry &chkEntry)
	{
		chkEntryLength = (uint32_t) (chkEntry.end - chkEntry.begin);

		if (filereader->getCurPos() != chkEntry.begin)
		{
			filereader->seekAbs(chkEntry.begin);
		}

		do
		{
			if (NALULength <= 0)
			{
				NALULength = filereader->readIntBigEndian(4, NALULength);
				chkEntryLength -= 4;

				NALUType = filereader->peek() & 0x1F;

				if (NALUType == 5)
				{
					//iframePos = std::to_string(filewriter->getFileSize());
				}

				filewriter->write(startCode, 4);
			}

			if (NALULength >= chkEntryLength)
			{
				filewriter->write(
					filereader->read(chkEntryLength), 
					chkEntryLength);

				NALULength -= chkEntryLength;
				chkEntryLength = 0;
			}
			else
			{
				filewriter->write(
					filereader->read(NALULength), 
					NALULength);

				chkEntryLength -= NALULength;
				NALULength = 0;
			}

		} while (chkEntryLength != 0);

		filewriter->write(
			filereader->read(chkEntryLength), 
			chkEntryLength);
	});
}