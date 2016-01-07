#include "Stdafx.h"
#include "Box.h"

using namespace MPEG4Parser;

Box* OtherBox::parse(FileReader* filereader, int64_t unParsedLength)
{
	this->m_dataBegPos = filereader->getCurPos();

	filereader->seek(unParsedLength);

	std::cout << this->toString();

	return this;
}

std::string OtherBox::toString()
{
	std::ostringstream os;

	os << "OtherBox: " << this->m_boxName 
		<< "\n  length: " << this->m_length
		<< "\n  begin position: " << this->m_begPos
		<< "\n\n";

	return os.str();
}

Box* ContainerBox::parse(FileReader* filereader, int64_t unParsedLength)
{
	uint64_t length;
	int64_t needParsedLength;
	char *buf = nullptr, nameBuf[5];
	Box* nboxPtr;

	std::cout << this->toString();

	while (!(filereader->eof() || filereader->peek() == EOF))
	{
		// The size is the entire size of the box, 
		//  including the size and type header,
		//  fields, and all contained boxes.
		// via ISO/IEC 14496-12:2008 Page 4 

		// read length of the block
		length = filereader->readIntBigEndian(4, length);

		// read the type name
		buf = filereader->read(4);

		strcpy_s(nameBuf, buf);

		if (1 == length)
		{
			length = filereader->readIntBigEndian(8, length);
			needParsedLength = length - 16;
		}
		else
		{
			needParsedLength = length - 8;
		}

		unParsedLength -= length;

		if (isContainerBox(buf))
		{
			nboxPtr = new ContainerBox(buf, length, filereader->getCurPos());
		}
		else if (isMetaBox(buf))
		{
			nboxPtr = new MetaBox(buf, length, filereader->getCurPos());
		}
		else
		{
			nboxPtr = new OtherBox(buf, length, filereader->getCurPos());
		}

		nboxPtr->parse(filereader, needParsedLength);
	
		this->addChildBox(nameBuf, nboxPtr);

		if (unParsedLength <= 0)
		{
			break;
		}
	}

	if (unParsedLength < 0)
	{
		ParserException e = ParserException(ParserException::PARSE_LENGTH_INCORRECT);

		throw e;
	}

	if (0 != strcmp(this->m_boxName, "MP4C"))
	{
		filereader->seek(unParsedLength);
	}

	return this;
}

void ContainerBox::addChildBox(char* name, Box* boxPtr)
{
	ChildBoxEntry childBoxEntry;
	childBoxEntry.m_boxPtr = boxPtr;
	strcpy_s(childBoxEntry.m_name, name);

	m_childBoxEntries.push_back(childBoxEntry);
}

Box* ContainerBox::findChildBox(char* boxName)
{
	for (size_t i = 0; i < m_childBoxEntries.size(); i++)
	{
		if (0 == strcmp(m_childBoxEntries[i].m_name, boxName))
		{
			return m_childBoxEntries[i].m_boxPtr;
		}
	}

	return nullptr;
}

Box* ContainerBox::findChildBox(char* boxName, int skipnum)
{
	BoxPtrVec boxVec;
	int size = this->findAllChildBoxes(boxName, boxVec);

	if (size <= skipnum)
	{
		return nullptr;
	}

	return boxVec[skipnum];
}

int ContainerBox::findAllChildBoxes(char* boxName, BoxPtrVec& rtnBoxVec)
{
	int count = 0;

	for (size_t i = 0; i < m_childBoxEntries.size(); i++)
	{
		if (0 == strcmp(m_childBoxEntries[i].m_name, boxName))
		{
			rtnBoxVec.push_back(m_childBoxEntries[i].m_boxPtr);
			++count;
		}
	}

	return count;
}

Box* ContainerBox::findBox(char* boxName)
{
	Box* boxPtr;

	for (size_t i = 0; i < m_childBoxEntries.size(); i++)
	{
		if (0 == strcmp(m_childBoxEntries[i].m_name, boxName))
		{
			return m_childBoxEntries[i].m_boxPtr;
		}
		else if (isContainerBox(m_childBoxEntries[i].m_name))
		{
			boxPtr = ((ContainerBox*) m_childBoxEntries[i].m_boxPtr)->findBox(boxName);

			if (nullptr != boxPtr)
			{
				return boxPtr;
			}
		}
	}

	return nullptr;
}

Box* ContainerBox::findBox(char* boxName, int skipnum)
{
	BoxPtrVec boxVec;
	int size = this->findAllBoxes(boxName, boxVec);

	if (size <= skipnum)
	{
		return nullptr;
	}

	return boxVec[skipnum];
}

int ContainerBox::findAllBoxes(char* boxName, BoxPtrVec& rtnBoxVec)
{
	int count = 0;

	for (size_t i = 0; i < m_childBoxEntries.size(); i++)
	{
		if (0 == strcmp(m_childBoxEntries[i].m_name, boxName))
		{
			rtnBoxVec.push_back(m_childBoxEntries[i].m_boxPtr);
			++count;
		}
		else if (isContainerBox(m_childBoxEntries[i].m_name))
		{
			count += ((ContainerBox*) m_childBoxEntries[i].m_boxPtr)->findAllBoxes(boxName, rtnBoxVec);
		}
	}

	return count;
}

std::string ContainerBox::toString()
{
	std::ostringstream os;

	os << "Container Box: " << this->m_boxName 
		<< "\n  length: " << this->m_length
		<< "\n  begin position: " << this->m_begPos;

	os << "\n\n";

	return os.str();
}

Box* MetaBox::parse(FileReader* filereader, int64_t unParsedLength)
{
	char *metaName;
	int32_t metaLength;
	
	MetaDataRuleEntry* metaDataRule = metaDataRules[this->m_boxName];
	MetaDataEntry* metaDataEntryPtr;

	if (nullptr == metaDataRule)
	{
		ParserException e = ParserException(ParserException::META_RULE_MISSED);

		throw e;
	}

	while (nullptr != metaDataRule->m_name)
	{
		metaName = metaDataRule->m_name;
		metaLength = metaDataRule->m_length;

		metaDataEntryPtr = new MetaDataEntry;
		metaDataEntryPtr->m_dataType = metaDataRule->m_dataType;
		

		/** this code will be removed later **/
		// should parse the meta data judging from the metaType
		// those code logic need to be added later [1]
		//if (0 == metaLength)
		//{
		//	// parse the NULL terminal string
		//	// need to be done with [1]
		//	// finish it later, skip now 
		//}
		//else
		//{
		//	// due to [1] has not been done yet
		//	// all data storage in string first
		//	std::string str = std::string(filereader->read(metaLength));
		//	this->addMetaData(metaName, str);
		//}

		if (metaLength < 0) 
		{
			metaLength = -metaLength;

			const MetaDataEntry* metaDataEntryPtrTmp = this->getMetaData("version");

			if (metaDataEntryPtrTmp == nullptr || 
				metaDataEntryPtrTmp->m_metaData.uint64 != 1)
			{
				metaLength <<= 1;
			}
		}

		switch (metaDataEntryPtr->m_dataType)
		{
		case DataType::Bool:
			std::cout<<"undone yet"<<std::endl;
			break;

		case DataType::Char:
			metaDataEntryPtr->m_metaData.char8 = filereader->read(1)[0];
			break;

		case DataType::Float:
			metaDataEntryPtr->m_metaData.double64 = filereader->readFloat();
			break;

		case DataType::Double:
			metaDataEntryPtr->m_metaData.double64 = filereader->readDouble();
			break;

		case DataType::FixedPointNumber:
			metaDataEntryPtr->m_metaData.double64 = 
				filereader->readFixedPointBigEndian(metaLength >> 1, metaLength >> 1);
			break;
			
		case DataType::Integer:
		case DataType::HexIngeter:
			filereader->readIntBigEndian(metaLength, metaDataEntryPtr->m_metaData.uint64);
			break;

		case DataType::SighedInteger:
			filereader->readIntBigEndian(metaLength, metaDataEntryPtr->m_metaData.int64);
			
		case DataType::IntegerArray:
			filereader->read(metaLength);
			std::cout<<"undone yet"<<std::endl;
			break;

		case DataType::Matrix:
			filereader->read(metaLength);
			std::cout<<"undone yet"<<std::endl;
			break;

		case DataType::NullTerminalString:
			metaDataEntryPtr->m_metaData.strptr = 
				filereader->readNULLTerminatedString();
			unParsedLength -= metaDataEntryPtr->m_metaData.strptr->length();
			break;

		case DataType::StringArray:
			metaLength = (int32_t) unParsedLength;
			// drop down on purpose
		
		case DataType::Unknown:
		case DataType::String:
			metaDataEntryPtr->m_metaData.strptr =
				filereader->readString(metaLength);
			break;

		default:
			// error occur
			std::cout<<"error occur"<<std::endl;
			break;
		}

		this->addMetaData(metaName, metaDataEntryPtr);
		
		unParsedLength -= metaLength;

		++metaDataRule;
	}

	// due to [1] has not been done yet
	// error must occur if remove these annotation
	//if (unParsedLength != 0) 
	//{
		// ParserException e = ParserException(ParserException::PARSE_LENGTH_INCORRECT);

		// throw e;
	//}

	filereader->seek(unParsedLength);

	std::cout << this->toString();

	return this;
}

std::string MetaBox::toString()
{
	std::ostringstream os;

	os << "Meta Box: " << this->m_boxName 
		<< "\n  length: " << this->m_length
		<< "\n  begin position: " << this->m_begPos;

	os << "\n\n";

	return os.str();
}