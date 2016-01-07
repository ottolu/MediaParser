#pragma once

#include <vector>
#include <algorithm>

#include "Types.h"
#include "Config.h"
#include "Exceptions.h"
#include "FileReader.h"

using namespace MediaParser;

namespace MPEG4Parser
{
	class Box;

	// struct of Box's Node
	struct ChildBoxEntry
	{
		char m_name[5];
		Box* m_boxPtr;
	};

	typedef std::vector<Box*> BoxPtrVec;
	typedef std::vector<ChildBoxEntry> ChildBoxVec;

	// should be rewrite to abstract class
	class Box 
	{
	protected:
		char m_boxName[5];
		uint8_t m_boxType;
		uint64_t m_length;
		uint64_t m_begPos;

		Box(char* boxName_, uint64_t length_, uint64_t begPos_, uint8_t boxType_) :
			m_length(length_), 
			m_begPos(begPos_), 
			m_boxType(boxType_)
		{
			strcpy_s(m_boxName, boxName_);
		}
	
	public:
		virtual ~Box() 
		{
			//
		}

		virtual std::string toString() = 0;
		virtual Box* parse(FileReader* filereader, int64_t unParsedLength) = 0;

		uint8_t getBoxType() const
		{ 
			return this->m_boxType; 
		}

		uint64_t getLength() const
		{ 
			return this->m_length; 
		}

		uint64_t getPos() const
		{ 
			return this->m_begPos; 
		}

		char* getBoxName()
		{ 
			return this->m_boxName; 
		}
	};

	class ContainerBox : public Box
	{
	protected:
		ChildBoxVec m_childBoxEntries;

	public:
		ContainerBox(char* boxName_, uint64_t length_, uint64_t begPos_) :
			Box(boxName_, length_, begPos_, BoxType::CONTAINER_BOX)
		{
			//
		}

		~ContainerBox()
		{
			for (size_t i = 0; i < m_childBoxEntries.size(); i++)
			{
				delete m_childBoxEntries[i].m_boxPtr;
			}
		}

		virtual std::string toString();
		virtual Box* parse(FileReader* filereader, int64_t unParsedLength);

		const ChildBoxVec& getChildBoxEntries()
		{
			return m_childBoxEntries;
		}

		void addChildBox(char* name, Box* boxPtr);

		Box* findChildBox(char* boxName);
		// param `skipNum` means 
		// return the NO. (skipNum+1) of the same boxName boxes
		Box* findChildBox(char* boxName, int skipNum);
		int findAllChildBoxes(char* boxName, BoxPtrVec& rtnBoxVec);

		Box* findBox(char* boxName);
		Box* findBox(char* boxName, int skipNum);
		int findAllBoxes(char* boxName, BoxPtrVec& rtnBoxVec);
	};

	class MetaBox : public Box
	{
	protected:
		meta_hmap m_metaDataEntries;

	public:
		MetaBox(char* boxType_, uint64_t length_, uint64_t begPos_) :
			Box(boxType_, length_, begPos_, BoxType::META_BOX) 
		{
			//
		}

		~MetaBox()
		{
			/*for (meta_hmap::iterator iter = m_metaDataEntries.begin(); 
					iter != m_metaDataEntries.end(); ++iter)
			{
				delete iter->second;
			}*/

			// lambda function
			std::for_each(std::begin(m_metaDataEntries), std::end(m_metaDataEntries),
				[&](std::pair<const char*, MetaDataEntry*> pair)
				{
					delete pair.second;
				});
		}

		virtual std::string toString();
		virtual Box* parse(FileReader* filereader, int64_t unParsedLength);

		void addMetaData(char *key, MetaDataEntry* metaDataEntryPtr)
		{
			m_metaDataEntries[key] = metaDataEntryPtr;
		}

		// 
		const MetaDataEntry* const getMetaData(char *key)
		{
			meta_hmap::iterator iter = m_metaDataEntries.find(key);
			
			if (iter == m_metaDataEntries.end())
			{
				return nullptr;
			}

			return iter->second;
		}

		const meta_hmap& getMetaDataEntries()
		{
			return m_metaDataEntries;
		}
	};

	
	class OtherBox : public Box
	{
	protected:
		uint64_t m_dataBegPos;

	public:
		OtherBox(char* boxType_, uint64_t length_, uint64_t begPos_) :
			Box(boxType_, length_, begPos_, BoxType::OTHER_BOX) 
		{
			//
		}

		~OtherBox()
		{
			//
		}

		virtual std::string toString();
		virtual Box* parse(FileReader* filereader, int64_t unParsedLength);

		uint64_t getDataBegPos()
		{
			return m_dataBegPos;
		}
	};
}