#ifndef _Util_StringAlgo_h_
#define _Util_StringAlgo_h_

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <cassert>
#include <boost/multi_array.hpp>

namespace Dv
{
	namespace Util
	{	
		// Link to some node
		struct KeywordTreeLink
		{
			// Length of the word prefix corresponding the node that this link points to
			int nodeLength;
			// Index of the node this link points to
			// The index is unical within the nodes corresponding words with length == nodeLength;
			int nodeIndex;

			bool IsNull() const { return nodeLength == 0; }

			KeywordTreeLink(int nodeLengthIn, int nodeIndexIn) : nodeLength(nodeLengthIn), nodeIndex(nodeIndexIn) {}
			KeywordTreeLink() : nodeLength(0), nodeIndex(0) {}
		};

		struct LinkedKeywordTreeNode
		{
			// Index of the parsed substring (if the link is the leaf link),
			// or -1 (if the link is not the leaf link).
			int substringIndex;
			// Table that sets a relationship between
			// next character and a next state index
			// (stored -1 if the transition is not valid for the parsed prefix)
			std::vector<int>								transitionTable;
			// Suffix link
			KeywordTreeLink									suffixTransition;
			// Index of the node corresponding the maximal prefix
			// of the word corresponding the current node
			int												prefix_nodeIndex;

			// True if the character is valid for the parsed prefix
			bool IsValidChar(char c)						const { GetNextNodeIndex(c) >= 0; }
			// Get index of the next node
			// (supposing the char is a valid char)
			int GetNextNodeIndex(char c)					const { return transitionTable[c]; }			

			// Transform to link leaf
			void SetAsLeaf(int substringIndexIn) { assert(substringIndex >= 0); assert(std::find(transitionTable.cbegin(), transitionTable.cend(), -1) == transitionTable.cend()); substringIndex = substringIndexIn; }

			// Set all non-suffix links as null link
			void ClearLinks() { std::for_each(transitionTable.begin(), transitionTable.end(), [](int& trans){ trans = -1; }); }
			void AddTransition(char c, int newNodeIndex) { assert(!IsValidChar(c)); transitionTable[c] = newNodeIndex; }
			// Get next transition of this node for a given transition from this state corresponding this node to
			// the next state corresponding this node
			// returns std::make_pair('\0', -1) if the end of the transition table is found;			
			std::pair<char,int> GetNextTransition(char ch) const;
			// Get first transition of this nodex
			std::pair<char,int> GetFirstTransition() const { return GetNextTransition('\0'); }
			// Set suffix transition
			void SetSuffixTransition(const KeywordTreeLink& suffixTransitionIn)		{ suffixTransition = suffixTransitionIn; }
			void SetSuffixTransition(int nodeLengthIn, int nodeIndexIn)				{ SetSuffixTransition(KeywordTreeLink(nodeLengthIn, nodeIndexIn)); }
			void SetPrefixNodeIndex(int prefixNodeIndexIn)							{ prefix_nodeIndex = prefixNodeIndexIn; }

			// Create a node with no links and suffix link to the zero node
			LinkedKeywordTreeNode(int substringIndexIn, int prefix_nodeIndexIn, int alphabet_size = 256) : 
				transitionTable(alphabet_size), suffixTransition(), prefix_nodeIndex(prefix_nodeIndexIn), substringIndex(substringIndexIn) { ClearLinks(); }
			// Create a non-leaf link
			LinkedKeywordTreeNode(int prefix_nodeIndexIn = 0, int alphabet_size = 256) : transitionTable(alphabet_size), suffixTransition(), prefix_nodeIndex(prefix_nodeIndexIn), substringIndex(-1) { ClearLinks(); }
		};

		// The tree for storing keywords
		// The first index: index of the length of the string
		// the second index: number of the node of the given length
		// node [0][0] is the empty node
		typedef std::vector<std::vector<LinkedKeywordTreeNode>> KeywordTree;
		// Geta a node for the given link
		inline const LinkedKeywordTreeNode&					GetNode  (const KeywordTree& keywordTree, const KeywordTreeLink& node);
		inline LinkedKeywordTreeNode&						GetNode  (KeywordTree& keywordTree, const KeywordTreeLink& node);
		// Add a node and link it with a given parent node 
		inline KeywordTreeLink AddNode
		(
			KeywordTree& keywordTree, 
			const KeywordTreeLink& parentNodeLink,
			char transitionChar
		);
		
		// Use the search algorithm of Aho-Corasick
		// to search the first occurence of one of the words represented with a given keyword tree
		// in the given sequence of chars within the range represented with the start/end iterator pair
		// Returns: end if none of the given words was found
		std::string::iterator AhoCorasickSearch
		(
			std::string::iterator start, std::string::iterator end,
			const KeywordTree& keywordTree
		);

		// searchs a given substring using the Boyer Moore search algorithm
		// Neither the [start, end), nor the [patternStart, patternEnd)
		// range may be empty
		// returns: end if the pattern string is not found 
		std::string::iterator BoyerMooreSearch
		(
			std::string::iterator start, std::string::iterator end,
			std::string::const_iterator patternStart, std::string::const_iterator patternEnd
		);		
		// searches a given substring using the Boyer Moore Horsppl aglorithm.
		// Neither the [start, end), nor the [patternStart, patternEnd)
		// range may be empty
		// returns: end if the pattern string is not found
		std::string::iterator BoyerMoorHorspool
		(
			std::string::iterator start, std::string::iterator end,
			std::string::const_iterator patternStart,
			std::string::const_iterator patternEnd
		);

		// Returns a vector, where element with index n contains
		// a shift that the prefix to be shifted to be contained
		// in the string [start;end) at that position
		template<class Iterator> std::vector<int>					GetPrefixShiftTable(Iterator start, Iterator end);
		template<class Iterator> std::unordered_map<char,int>		GetFirstCharOccurenceIndexTable(Iterator first, Iterator last);

		// Impl
		template<class Iterator> std::vector<int> GetPrefixShiftTable(Iterator start, Iterator end)
		{
			assert(start != end);
			std::vector<int> prefixShifts;
			// Add shift for prefix of zero length			
			prefixShifts.push_back(0);
			int prefix_length = 1;
			Iterator substr_start = std::find(std::next(start), end, *start);						
			if(substr_start != end)
			{
				Iterator prefix_end = std::next(start);	
				Iterator substr_end = std::next(substr_start);
				while(true)
				{	
					if(substr_end == end)
					{
						break;
					}
					int prefix_shift = substrStart - start;
					prefixShifts.push_back(prefix_shift);					
					prefix_length++;
					substr_end++;
					prefix_end++;
					if (*prefix_end != *substr_end)
					{
						substr_start = std::search(substr_start, end, prefix_start, prefix_end);
					}															
				}
			}
			int total_length = end-start;
			// Fill up all shifts for prefixes those length is greater that maximal length
			for(; ++prefix_length <= total_length; )
			{
				prefixShifts.push_back(total_length);
			}
			return prefixShifts;
		}

		template<class Iterator>
		std::unordered_map<char,int> GetFirstCharOccurenceIndexTable(Iterator first, Iterator last)
		{
			std::unordered_map<char,int> res;
			int index = 0;
			for(auto it = first; it != last; ++it)
			{				
				char ch = *it;
				auto it_char_in_table = res.find(ch);
				if (it_char_in_table != res.end())
				{
					res.insert(std::make_pair(ch,index));
				}
				index++;
			}
			return res;
		}

		// KeywordTree impl
		inline const LinkedKeywordTreeNode& GetNode(const KeywordTree& keywordTree, const KeywordTreeLink& node)
		{
			return keywordTree[node.nodeLength][node.nodeIndex];
		}

		inline LinkedKeywordTreeNode& GetNode(KeywordTree& keywordTree, const KeywordTreeLink& node)
		{
			return keywordTree[node.nodeLength][node.nodeIndex];
		}

		inline KeywordTreeLink AddNode
		(
			KeywordTree& keywordTree, 
			const KeywordTreeLink& parentNodeLink,
			char transitionChar
		)
		{
			LinkedKeywordTreeNode			parentNode					= GetNode(keywordTree, parentNodeLink);
			int								parentNodeWordLength		= parentNodeLink.nodeLength;
			KeywordTreeLink					newNodeLink					= KeywordTreeLink(parentNodeWordLength + 1, 0);
			LinkedKeywordTreeNode			newNode						= LinkedKeywordTreeNode(newNodeLink.nodeLength);
			newNode.SetPrefixNodeIndex(parentNodeWordLength);
			keywordTree[newNodeLink.nodeLength].push_back(newNode);		
			newNodeLink.nodeIndex = keywordTree.size() - 1;
			parentNode.AddTransition(transitionChar, newNodeLink.nodeIndex);
			return newNodeLink;
		}
	} // Util
} // Dv

#endif // _Util_StringAlgo_h_