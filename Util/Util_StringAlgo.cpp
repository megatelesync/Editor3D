#include "h/Util_StringAlgo.h"
#include <cassert>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>

namespace Dv
{
	namespace Util
	{
		std::string::iterator AhoCorasickSearch
		(
			std::string::iterator start, std::string::iterator end,
			const KeywordTree& keywordTree
		)
		{
			std::string::iterator substr_iterator = start;
			// link to node corresponding current parsed text part
			KeywordTreeLink parsedNodeLink = KeywordTreeLink(0, 0);
			while(true)
			{
				LinkedKeywordTreeNode parsedNode = GetNode(keywordTree, parsedNodeLink);
				if(parsedNode.prefix_nodeIndex >= 0) { return substr_iterator; }
				if(substr_iterator == end) { return end; }
				char ch = *substr_iterator;

				if(parsedNode.IsValidChar(ch))
				{
					int nextNodeIndex = parsedNode.GetNextNodeIndex(ch);
					parsedNodeLink = KeywordTreeLink(parsedNodeLink.nodeIndex + 1, nextNodeIndex);
				}
				else
				{
					// backtrack with suffix link
					parsedNodeLink = parsedNode.suffixTransition;
				}

				substr_iterator++;
			}
		}

		std::pair<char,int> LinkedKeywordTreeNode::GetNextTransition(char ch) const 
		{
			while(true) 
			{ 
				if(ch >= transitionTable.size()) 
				{
					break;
				} 
				int transitionNodeIndex = transitionTable[ch];
				if(transitionNodeIndex != -1) 
				{ 
					return std::make_pair(ch,transitionNodeIndex); 
				}
				ch++;
			}; 
			return std::make_pair('\0', -1); 
		} 		

		std::string::iterator BoyerMooreSearch
		(
			std::string::iterator start, std::string::iterator end,
			std::string::const_iterator patternStart, std::string::const_iterator patternEnd
		)
		{
			typedef std::string::reverse_iterator StringReverseIt;
			typedef std::string::const_reverse_iterator ConstStringReverseIt;
			typedef std::pair<StringReverseIt, ConstStringReverseIt> MismatchResult;

			// shifts from the end of the string
			const auto revIt_patternEnd = std::prev(ConstStringReverseIt(patternStart));
			const auto revIt_patternStart = std::prev(ConstStringReverseIt(patternEnd));
			std::vector<int> pattern_prefix_shifts = GetPrefixShiftTable(patternStart,patternEnd);
			std::unordered_map<char,int> pattern_last_indices_for_chars = GetFirstCharOccurenceIndexTable(revIt_patternStart, revIt_patternEnd); // Do it up: assign the result
			
			int str_len = end - start;
			int pattern_len = patternEnd - patternStart;
			// maximal that the substr_pos allow to gain + 1
			std::string::iterator substr_pos_end = end - str_len + 1;
			// Position of substr in the original string
			std::string::iterator substr_pos = start;
			// end of substr position in the original string
			std::string::iterator substr_end = start + pattern_len;
			while(true)
			{
				if(substr_pos == substr_pos_end)
				{
					// could not find substr
					return end;
				}

				auto revit_substr_end = std::prev(std::string::reverse_iterator(substr_end));
				auto revit_substr_pos = std::prev(std::string::reverse_iterator(substr_pos));				
				auto revit_patternEnd = std::prev(std::string::const_reverse_iterator(patternEnd));
				MismatchResult mismatch_iters = std::mismatch(revit_substr_end, revit_substr_pos, revit_patternEnd);				
				if(mismatch_iters.first == revit_substr_pos)
				{
					// the substr is found
					return substr_pos;
				}
				// The substring is not found	
				auto mismatch_pos = mismatch_iters.first;
				auto mismatch_pattern_pos = mismatch_iters.second;	
				char mismatch_char = *mismatch_pos;

				int matched_prefix_len = mismatch_pattern_pos - revit_patternEnd;
				int mismatch_char_pattern_index = mismatch_pattern_pos - revit_patternEnd - 1;

				// No mismatching character in the pattern string
				if(pattern_last_indices_for_chars[mismatch_char] < 0)
				{
					int prefixShift = pattern_prefix_shifts[matched_prefix_len];
					// If we can shift, so that the matched prefix to be contained in the substr
					if (prefixShift >= 0)
					{
						// Shift the substr position, so we'll consider another candidate substr position 
						substr_pos += prefixShift;						
					}
					else
					{
						// Cannot shift, so that the matched prefix to be contained in the substr.
						// Position in such a fashion that the new position to be right after the
						// old position of the end of the substr
						substr_pos = std::next(substr_end);
					}					
				}
				else
				{
					// There's a mismatching character in the pattern string
					int mismatch_char_pattern_last_index = pattern_last_indices_for_chars[mismatch_char];
					if (mismatch_char_pattern_index <= mismatch_char_pattern_last_index)
					{
						// The mismatched character index've been already seen in the prefix
						// So, position substr at the first mismatched character position
						std::string::iterator mismatch_pos_forward = substr_end - mismatch_char_pattern_index - 1;
						substr_pos = std::next(mismatch_pos_forward);
					}
					else
					{
						// The mismatch character has not already be seen in the prefix
						// So, position substr so that the mismatched character matches character
						// in the substr
						int substr_shift = mismatch_char_pattern_last_index - mismatch_char_pattern_index;
						substr_pos += substr_shift;
					}
				}
				substr_end = substr_pos + pattern_len;
			}
		}

		std::string::iterator BoyerMoorHorspool
		(
			std::string::iterator start, std::string::iterator end,
			std::string::const_iterator patternStart,
			std::string::const_iterator patternEnd
		)
		{			
			typedef std::string::reverse_iterator						StringRevIt;
			typedef std::string::const_reverse_iterator					ConstStringRevIt;
			typedef std::pair<StringRevIt, ConstStringRevIt>			MismatchResult;
			assert(start != end);
			assert(patternStart != patternEnd);

			int str_len = end - start;
			int pattern_len = patternEnd - patternStart;
			auto revit_patternStart = std::prev(ConstStringRevIt(patternEnd));
			auto revit_patternEnd = std::prev(ConstStringRevIt(patternStart));
			std::unordered_map<char,int> lastOccurenceIndexTable = GetFirstCharOccurenceIndexTable(revit_patternStart, revit_patternEnd);
			lastOccurenceIndexTable[*std::prev(patternEnd)] = pattern_len;			
			std::string::iterator substr_pos = start;
			std::string::iterator substr_pos_end = end - pattern_len;
			while(true)
			{
				int substr_pos_index = substr_pos - start;				
				std::string::iterator substr_end = substr_pos + pattern_len;
				int substr_end_index = substr_end - start;
				int distance_substr_end_to_end = (str_len - 1) - substr_end_index;

				if(substr_pos == substr_pos_end)
				{
					break;
				}

				auto revit_substr_start = std::prev(std::string::reverse_iterator(substr_end));
				auto revit_substr_end = std::prev(std::string::reverse_iterator(substr_pos));
				auto revit_pattern_end = std::prev(std::string::const_reverse_iterator(patternEnd));
				MismatchResult mismatch_result = std::mismatch(revit_substr_start, revit_substr_end, revit_pattern_end);
				if(mismatch_result.first == revit_substr_end)
				{
					// could not find the substring
					return end;
				}

				// At this point there's a mismatching character in the text
				auto revit_mismatch_char = mismatch_result.first;
				auto revit_mismatch_pattern_char = mismatch_result.second;
				char mismatched_char = *revit_mismatch_char;
				char prefix_first_char = *std::prev(substr_end);

				// move the candidate substring so that the mismatched character matches
				// the first character of the seen prefix of the substring (disregarding of whether this character matched or not)
				int shift = lastOccurenceIndexTable[prefix_first_char];
				// if the last character of the substring will overbound the string after the shift
				// then no substring found
				if(shift > distance_substr_end_to_end)
				{
					return end;
				}

				substr_pos += shift;
			}
			return substr_pos;
		}
	} // Util
} // Dv