#include "../../h/StringUtil/Util_StringUtil_Master.h"
#include <cassert>

namespace Dv
{
	namespace Util
	{
		bool LoadStringVectorFromStream
		(
			std::vector<std::string> *pResultText,
			std::ifstream *pStrm
		)
		{
			assert(pResultText != nullptr);
			assert(pStrm != nullptr);
			std::string curr_line;
			while (std::getline(*pStrm, curr_line))
			{
				pResultText->push_back(curr_line);
			}
			if (pStrm->eof())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		std::string str_repeat(char c, int count)
		{
			std::string res_str;
			for (int i = 0; i < count; i++)
			{
				res_str.push_back(c);
			}
			return res_str;
		}
		void append_str_repeat(std::string *pOutString, char c, int count)
		{
			assert(pOutString != nullptr);
			size_t str_size_before = pOutString->size();
			size_t new_str_size = pOutString->size() + count;
			pOutString->append(count, c);
		}
	} // Util
} // Dv