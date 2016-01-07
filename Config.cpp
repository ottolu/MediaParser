#include "Stdafx.h"
#include "Config.h"

using namespace MediaParser;

namespace MPEG4Parser
{
	string_hset metaBoxSet;
	string_hset containerBoxSet;
	rule_hmap metaDataRules;

	void metaBoxSetInit()
	{
		metaBoxSet.insert("ftyp");
		metaBoxSet.insert("mvhd");
		metaBoxSet.insert("tkhd");
		metaBoxSet.insert("mdhd");
		metaBoxSet.insert("hdlr");
		metaBoxSet.insert("vmhd");
		metaBoxSet.insert("smhd");
	}

	void containerBoxSetInit()
	{
		containerBoxSet.insert("moov");
		containerBoxSet.insert("trak");
		containerBoxSet.insert("edts");
		containerBoxSet.insert("mdia");
		containerBoxSet.insert("minf");
		containerBoxSet.insert("dinf");
		containerBoxSet.insert("stbl");
		containerBoxSet.insert("mvex");
		containerBoxSet.insert("moof");
		containerBoxSet.insert("traf");
		containerBoxSet.insert("mfra");
	}

	bool isContainerBox(char* type)
	{
		string_hset::iterator iter = containerBoxSet.find(type);
		return iter != containerBoxSet.end();
	}

	bool isMetaBox(char* type)
	{
		string_hset::iterator iter = metaBoxSet.find(type);
		return iter != metaBoxSet.end();
	}

	void metaDataRuleInit()
	{
		// m_length (the second value) can be negetive,
		// which represents that it's an array
		// with n bytes per-elem-length, 
		// parse it until the end of MetaBox
		// e.g. -n
		static MetaDataRuleEntry 
			ftypRule[] = {
				{"major brand", 4, DataType::String},
				{"minor version", 4, DataType::String},
				{"compatible brands", 4, DataType::StringArray},
				{nullptr, 0, DataType::Unknown}
		},
			mvhdRule[] = {
				{"version", 1, DataType::Integer},
				{"flags", 3, DataType::Integer},
				{"creation time", -4, DataType::Integer},
				{"modification time", -4, DataType::Integer},
				{"time scale", 4, DataType::Integer},
				{"duration", -4, DataType::Integer},
				{"rate", 4, DataType::FixedPointNumber},
				{"volume", 2, DataType::FixedPointNumber},
				{"reserved", 10, DataType::Unknown},
				{"matrix", 36, DataType::Matrix},
				{"pre-defined", 24, DataType::Integer},
				{"next track id", 4, DataType::Integer},
				{nullptr, 0, DataType::Unknown}
		},
			tkhdRule[] = {
				{"version", 1, DataType::Integer},
				{"flags", 3, DataType::Integer},
				{"creation time", -4, DataType::Integer},
				{"modification time", -4, DataType::Integer},
				{"track id", 4, DataType::Integer},
				{"reserved", 4, DataType::Unknown},
				{"duration", -4, DataType::Integer},
				{"reserved2", 8, DataType::Unknown},
				{"layer", 2, DataType::Integer},
				{"alternate group", 2, DataType::Integer},
				{"volume", 2, DataType::FixedPointNumber},
				{"reserved3", 2, DataType::Unknown},
				{"matrix", 36, DataType::Matrix},
				{"width", 4, DataType::Integer},
				{"height", 4, DataType::Integer},
				{nullptr, 0, DataType::Unknown}
		},
			mdhdRule[] = {
				{"version", 1, DataType::Integer},
				{"flags", 3, DataType::Integer},
				{"creation time", -4, DataType::Integer},
				{"modification time", -4, DataType::Integer},
				{"time scale", 4, DataType::Integer},
				{"duration", -4, DataType::Integer},
				{"language", 2, DataType::String},
				{"pre-defined", 2, DataType::Integer},
				{nullptr, 0, DataType::Unknown}
		},
			hdlrRule[] = {
				{"version", 1, DataType::Integer},
				{"flags", 3, DataType::Integer},
				{"pre-defined", 4, DataType::Integer},
				{"handler type", 4, DataType::String},
				{"reserved", 12, DataType::Unknown},
				{"name", 0, DataType::NullTerminalString},
				{nullptr, 0, DataType::Unknown}
		},
			vmhdRule[] = {
				{"version", 1, DataType::Integer},
				{"flags", 3, DataType::Integer},
				{"graphics mode", 4, DataType::Integer},
				{"opcolor red", 2, DataType::HexIngeter},
				{"opcolor green", 2, DataType::HexIngeter},
				{"opcolor blue", 2, DataType::HexIngeter},
				{nullptr, 0, DataType::Unknown}
		},
			smhdRule[] = {
				{"version", 1, DataType::Integer},
				{"flags", 3, DataType::Integer},
				{"balance", 2, DataType::FixedPointNumber},
				{"reserved", 2, DataType::Unknown},
				{nullptr, 0, DataType::Unknown}
		}
		// Hint Media Header Box(hmhd) miss
		// Null Media Header Box(nmhd) miss
		// Data Information Box(dinf) miss
		;

		metaDataRules["ftyp"] = ftypRule;
		metaDataRules["mvhd"] = mvhdRule;
		metaDataRules["tkhd"] = tkhdRule;
		metaDataRules["mdhd"] = mdhdRule;
		metaDataRules["hdlr"] = hdlrRule;
		metaDataRules["vmhd"] = vmhdRule;
		metaDataRules["smhd"] = smhdRule;
	}

	void ConfigInit()
	{
		metaBoxSetInit();
		containerBoxSetInit();
		metaDataRuleInit();
	}
}