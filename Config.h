#pragma once

#include "Types.h"

namespace MPEG4Parser
{
	void ConfigInit();
	bool isContainerBox(char* type);
	bool isMetaBox(char* type);

	extern MediaParser::rule_hmap metaDataRules;
}