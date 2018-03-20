#pragma once

#include <vector>
#include <fstream>

#include "../CoreUtil/CoreUtil_String.h"

namespace Dv
{
	namespace Util
	{
		/**********************************************************************************************
		* Repeat the character count times and return std::string
		*
		* HINT: The character count CAN be zero.
		**********************************************************************************************/
		std::string str_repeat(char c, int count);
		void append_str_repeat(std::string *pOutString, char c, int count);

		template<class Iterator>
		std::string linearize_str(Iterator first, Iterator last);

		/***********************************************************************************************
		* Load vector of strings from the given stream;
		* Return nullptr false if failure occured while loading file
		* (the content is NOT erased - part of loaded content will be stored in the result string vector)
		***********************************************************************************************/
		bool LoadStringVectorFromStream
		(
			std::vector<std::string>                    *pResultText,
			std::ifstream                               *pStrm
		);

		/***********************************************************************************************
		* IMPLEMENTATION
		***********************************************************************************************/
		template<class Iterator>
		std::string linearize_str(Iterator first, Iterator last)
		{
			std::string res_str;
			Iterator it = first;
			while (it != last)
			{
				res_str.append(*it);
				res_str.append("\n");
				++it;
			}
			return res_str;
		}
	} // Util
} // Dv