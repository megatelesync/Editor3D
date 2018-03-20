#pragma once

#include "../CoreUtil/CoreUtil_StdInclude.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>

namespace Dv
{
	namespace Util
	{
		// read an array of values from a given ptree
		// stop reading values if endIterator engaged;
		// return number of read values
		template <class T, class Iterator, class Iterator2> int LoadArray_FromPropertyTree
		(
			const boost::property_tree::ptree& tree, 
			Iterator startIterator, Iterator2 endIterator
		);
		template <class T, class Iterator> int LoadArray_FromPropertyTree
		(
			const boost::property_tree::ptree& tree, 
			Iterator startIterator
		);



		// IMPL
		template <class T, class Iterator, class Iterator2> int LoadArray_FromPropertyTree
		(
			const boost::property_tree::ptree& tree, 
			Iterator startIterator, Iterator2 endIterator
		)
		{
			int numValues = 0;
			Iterator it = startIterator;
			boost::property_tree::ptree::const_assoc_iterator itTreeNode = tree.ordered_begin();
			while(true)
			{
				if(it == endIterator)
				{
					return numValues;
				}
				if(itTreeNode == tree.not_found())
				{
					return numValues;
				}
				std::string val_str = itTreeNode->second.get_value<std::string>();
				typename T val = boost::lexical_cast<T>(val_str.data(), val_str.length());
				*it = val;
				numValues++;
				++it;
				++itTreeNode;
			}
			return numValues;
		}
		
		template <class T, class Iterator> int LoadArray_FromPropertyTree
		(
			const boost::property_tree::ptree& tree, 
			Iterator startIterator
		)
		{
			int numValues = 0;
			Iterator it = startIterator;
			boost::property_tree::ptree::const_assoc_iterator itTreeNode = tree.ordered_begin();
			while(true)
			{
				if(itTreeNode == tree.not_found())
				{
					return numValues;
				}
				std::string val_str = itTreeNode->second.get_value<std::string>();
				typename T val = boost::lexical_cast<T>(val_str.data(), val_str.length());
				*it = val;
				numValues++;
				++it;
				++itTreeNode;
			}
			return numValues;
		}
	} // Util
} // Dv