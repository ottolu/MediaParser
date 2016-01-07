#include "Stdafx.h"
#include "Exceptions.h"
#include "FileWriter.h"
#include "SpecificBox.h"

using namespace MPEG4Parser;

void TrakBox::parseTrack(FileReader *filereader, std::shared_ptr<H264> h264)
{
	ContainerBox* stblBox = (ContainerBox*) this->findBox("stbl");

	if (nullptr == stblBox)
	{
		ParserException e = ParserException(ParserException::CANNOT_FIND_STBL_BOX);

		throw e;
	}

	// read stsc
	std::vector<StscEntry> stscEntries;

	StscBox *stscBox = (StscBox*) stblBox->findChildBox("stsc");
	
	if (nullptr == stscBox)
	{
		ParserException e = ParserException(ParserException::CANNOT_FIND_STSC_BOX);

		throw e;
	}

	stscBox->parseStsc(filereader, stscEntries);

	// read stsz
	uint32_t sampleSize = 0;
	std::vector<uint32_t> sampleSizeEntries;

	StszBox *stszBox = (StszBox*) stblBox->findChildBox("stsz");

	if (nullptr == stszBox)
	{
		ParserException e = ParserException(ParserException::CANNOT_FIND_STSZ_BOX);

		throw e;
	}

	stszBox->parseStsz(filereader, sampleSize, sampleSizeEntries);

	//read stco and co64
	std::vector<ChunkEntry> *chunkEntries = new std::vector<ChunkEntry>;

	StcoBox *stcoBox = (StcoBox*) stblBox->findChildBox("stco"),
		*co64Box = (StcoBox*) stblBox->findChildBox("co64");

	if (nullptr == stcoBox && nullptr == co64Box)
	{
		ParserException e = ParserException(ParserException::CANNOT_FIND_STCO_CO64_BOX);

		throw e;
	}

	uint32_t curStscEntriesIdx = 0;

	if (nullptr != stcoBox)
	{
		stcoBox->parseStco(filereader,
			*chunkEntries,
			stscEntries,
			sampleSizeEntries,
			curStscEntriesIdx,
			sampleSize);
	}

	if (nullptr != co64Box)
	{
		co64Box->parseStco(filereader,
			*chunkEntries,
			stscEntries,
			sampleSizeEntries,
			curStscEntriesIdx,
			sampleSize,
			1);
	}

	h264->setChunkEntries(chunkEntries);
}

void StscBox::parseStsc(FileReader *filereader, 
						std::vector<StscEntry>& stscEntries)
{
	uint32_t stscEntriesCount;

	// plus 4 means pass the version and flags
	filereader->seekAbs(this->m_dataBegPos + 4);

	filereader->readIntBigEndian(4, stscEntriesCount);

	for (uint32_t i = 0; i < stscEntriesCount; i++)
	{
		StscEntry stscEntry;
		filereader->readIntBigEndian(4, stscEntry.first_chunk);
		filereader->readIntBigEndian(4, stscEntry.sample_per_chunk);
		filereader->readIntBigEndian(4, stscEntry.sample_des_idx);
		stscEntries.push_back(stscEntry);
	}
}

void StszBox::parseStsz(FileReader *filereader, uint32_t sampleSize, 
						std::vector<uint32_t>& sampleSizeEntries)
{
	uint32_t sampleCount;

	// plus 4 means pass the version and flags
	filereader->seekAbs(this->m_dataBegPos + 4);
	filereader->readIntBigEndian(4, sampleSize);

	if (0 == sampleSize)
	{
		filereader->readIntBigEndian(4, sampleCount);

		sampleSizeEntries.reserve(sampleCount + 5);

		for (uint32_t i = 0; i < sampleCount; i++)
		{
			filereader->readIntBigEndian(4, sampleSize);
			sampleSizeEntries.push_back(sampleSize);
		}

		sampleSize = 0;
	}
}

void StcoBox::parseStco(FileReader *filereader, 
						std::vector<ChunkEntry>& chunkEntries,
						const std::vector<StscEntry>& stscEntries, 
						const std::vector<uint32_t>& sampleSizeEntries,
						uint32_t& curStscEntriesIdx,
						uint32_t sampleSize,
						uint8_t chunkOffsetLengthTimes)
{
	uint32_t 
		chunkCount, 
		chunkAbsPos,
		nextChkIdx = 0, 
		sampleIdx = 0, 
		curSamplePerChk = stscEntries[curStscEntriesIdx].sample_per_chunk;

	// plus 4 means pass the version and flags
	filereader->seekAbs(this->m_dataBegPos + 4);
	filereader->readIntBigEndian(4, chunkCount);

	chunkEntries.reserve(chunkEntries.size() + chunkCount + 5);

	for (uint32_t chunkIdx = 1; chunkIdx <= chunkCount; chunkIdx++)
	{
		if (chunkIdx >= nextChkIdx)
		{
			curSamplePerChk = stscEntries[curStscEntriesIdx].sample_per_chunk;
			++curStscEntriesIdx;

			if (curStscEntriesIdx < stscEntries.size())
			{
				nextChkIdx = stscEntries[curStscEntriesIdx].first_chunk;
			}
			else
			{
				nextChkIdx = UINT32_MAX;
			}
		}

		filereader->readIntBigEndian(4 << chunkOffsetLengthTimes, chunkAbsPos);

		ChunkEntry chunk;
		chunk.end = chunk.begin = chunkAbsPos;

		if (0 == sampleSize)
		{
			for (uint32_t i = 0; i < curSamplePerChk; i++)
			{
				chunk.end += sampleSizeEntries[sampleIdx++];
			}
		}
		else
		{
			chunk.end += sampleSize* curSamplePerChk;
		}

		chunkEntries.push_back(chunk);
	}
}

void StsdBox::parseStsd(FileReader *filereader, std::shared_ptr<H264> h264)
{
	char     *buf = nullptr;
	uint8_t  NALULength;
	uint8_t  SPSCount;
	uint16_t SPSLength;
	uint8_t  PPSCount;
	uint16_t PPSLength;

	std::string *SPS = new std::string(), 
				*PPS = new std::string();

	// plus 4 means pass the version and flags
	filereader->seekAbs(this->m_dataBegPos + 98);

	buf = filereader->read(4);

	if (0 != strcmp(buf, "avcC"))
	{
		std::cout << "error occur" << std::endl;
	}

	filereader->seek(4);

	NALULength = (filereader->get() & 0x03) + 1;

	SPSCount = filereader->get() & 0x1F;

	for (int i = 0; i < SPSCount; i++)
	{
		SPSLength = filereader->readIntBigEndian(2, SPSLength);
		
		SPS = filereader->readString(SPSLength);
	}

	PPSCount = filereader->get();

	for (int i = 0; i < PPSCount; i++)
	{
		PPSLength = filereader->readIntBigEndian(2, PPSLength);
		
		PPS = filereader->readString(PPSLength);
	}

	h264->setSPS(SPS);
	h264->setPPS(PPS);
}