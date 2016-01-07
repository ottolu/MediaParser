#pragma once

#include <string>

namespace MediaParser
{
	template <typename IntType>
	IntType& bytesToIntegerBigEndian(char* buf, IntType& result, int length = 0)
	{
		result = 0;

		if (length <= 0)
		{
			length = sizeof(result);
		}

		for (int n = 0; n < length; ++n)
		{
			result = (result << 8) + (IntType) (buf[n] & 0xFF);
		}

		return result;
	}

	template <typename IntType>
	IntType& bytesToIntegerLittleEndian(char* buf, IntType& result, int length = 0)
	{
		IntType* addr = (IntType*) buf;

		if (length > 0 && length < sizeof(IntType))
		{
			std::string str;
			str.clear();

			for (int n = 0; n < length; ++n)
			{
				str.push_back(buf[i]);
			}

			for (int i = length; i < sizeof(IntType); i++)
			{
				str.push_back(0);
			}

			addr = (IntType*) str.c_str();
		}

		return (result = *addr);
	}

	inline std::string int2str(int intvar)
	{
		std::stringstream ss;
		ss << intvar;
		return ss.str();
	}
}