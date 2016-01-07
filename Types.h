#pragma once

#include <hash_map>
#include <hash_set>
#include <cstdint>

#include "FileReader.h"

namespace MediaParser
{
	enum DataType 
	{
		// Integer Default to Unsighed
		Integer = 0,
		Float = 1,
		Double = 2,
		Char = 3,
		String = 4,
		Bool = 5,
		SighedInteger = 6,
		FixedPointNumber = 7,
		IntegerArray = 8,
		StringArray = 9,
		Matrix = 10,
		NullTerminalString = 11,
		HexIngeter = 12,
		Unknown = -1
	};

	enum BoxType
	{
		BOX = 1,
		CONTAINER_BOX = 2,
		META_BOX = 3,
		OTHER_BOX = 4
	};

	enum TrakType
	{
		VIDEO_TRACK = 1,
		AUDIO_TRACK = 2,
		HINT_TRACK = 3
	};

	struct StscEntry
	{
		uint32_t first_chunk;
		uint32_t sample_per_chunk;
		uint32_t sample_des_idx;
	};

	struct ChunkEntry
	{
		uint64_t begin;
		uint64_t end;
	};

	// m_length can be negetive,
	// which represents that it should
	// be double size when version = 1
	struct MetaDataRuleEntry
	{
		char* m_name;
		int16_t m_length;
		int8_t m_dataType;
	};

	union MetaData
	{
		bool bool1;
		char char8;
		//float float32;
		double double64;
		uint64_t uint64;
		int64_t int64;
		const char* ccharptr;
		std::string* strptr;
		void *other;
	};

	struct Matrix;

	struct MetaDataEntry
	{
		int8_t m_dataType;
		MetaData m_metaData;

		~MetaDataEntry()
		{
			switch (m_dataType)
			{
			case DataType::String:
			case DataType::StringArray:
			case DataType::NullTerminalString:
			case DataType::Unknown:
				delete m_metaData.strptr;
				break;

			case DataType::IntegerArray:
			case DataType::Matrix:
				break;

			default:
				break;
			}

			
		}

		//friend FileReader& operator >> (FileReader& filereader, MetaDataEntry& metaDataEntry)
		//{
		//	//
		//}

		friend std::ostream& operator << (std::ostream& stream, MetaDataEntry& metaDataEntry)
		{
			switch (metaDataEntry.m_dataType)
			{
			case DataType::Bool:
				stream << metaDataEntry.m_metaData.bool1;
				break;

			case DataType::Char:
				stream << metaDataEntry.m_metaData.char8;
				break;

			case DataType::SighedInteger:
				stream << metaDataEntry.m_metaData.int64;
				break;

			case DataType::Integer:
				stream << metaDataEntry.m_metaData.uint64;
				break;

			case DataType::Float:
			case DataType::Double:
			case DataType::FixedPointNumber:
				stream << metaDataEntry.m_metaData.double64;
				break;

			case DataType::String:
			case DataType::StringArray:
			case DataType::NullTerminalString:
				stream << &metaDataEntry.m_metaData.strptr;
				break;

			case DataType::HexIngeter:
				stream << std::hex << std::uppercase << 
					metaDataEntry.m_metaData.uint64;

			case DataType::IntegerArray:
			case DataType::Matrix:
				break;

			default:
				break;
			}

			return stream;
		}
	};

	// The following class defines a hash function for strings 
	class stringhasher : public std::hash_compare <const char*>
	{
	public:
		size_t operator() (const char* s) const
		{
			size_t h = 0;

			while (*s != '\0')
			{
				h = 31 * h + (*s);
				++s;
			}

			return h;
		}

		bool operator() (const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};

	typedef std::hash_set<const char*, stringhasher> string_hset;
	typedef std::hash_map<const char*, MetaDataRuleEntry*, stringhasher> rule_hmap;
	typedef std::hash_map<const char*, MetaDataEntry*, stringhasher> meta_hmap;
}