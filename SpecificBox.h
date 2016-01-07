#pragma once

#include "Box.h"
#include "H264.h"

namespace MPEG4Parser
{
	// DO NOT add member varible into these classes
	// just a container of Special Box's functions

	class TrakBox : public ContainerBox
	{
	public:
		void parseTrack(FileReader *filereader, std::shared_ptr<H264> h264);
		//int8_t getTrakType();
	};

	class StscBox : public OtherBox
	{
	public:
		void parseStsc(FileReader *filereader, 
			std::vector<StscEntry>& stscEntries);
	};

	class StszBox : public OtherBox
	{
	public:
		void parseStsz(FileReader *filereader, uint32_t sampleSize, 
			std::vector<uint32_t>& sampleSizeEntries);
	};

	class StcoBox : public OtherBox
	{
	public:
		void parseStco(FileReader *filereader,
			std::vector<ChunkEntry>& chunkEntries,
			const std::vector<StscEntry>& stscEntries, 
			const std::vector<uint32_t>& sampleSizeEntries,
			uint32_t& curStscEntriesIdx,
			uint32_t sampleSize,
			uint8_t chunkOffsetLengthTimes = 0);
	};

	class StsdBox : public OtherBox
	{
	public:
		void parseStsd(FileReader *filereader, std::shared_ptr<H264> h264);
	};
}