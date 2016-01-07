#pragma once

#include "Types.h"

using namespace MediaParser;

namespace MPEG4Parser
{
	class H264
	{
	protected:
		std::string *m_PPS;
		std::string *m_SPS;

		std::vector<ChunkEntry>* m_chunkEntries;

	public:
		~H264()
		{
			delete m_PPS;
			delete m_SPS;
			delete m_chunkEntries;
		}

		void setSPS(std::string *SPS)
		{
			m_SPS = SPS;
		}

		void setPPS(std::string *PPS)
		{
			m_PPS = PPS;
		}

		void setChunkEntries(std::vector<ChunkEntry>* chunkEntries)
		{
			m_chunkEntries = chunkEntries;
		}

		void extract(FileReader *filereader, char *filename);
	};
}