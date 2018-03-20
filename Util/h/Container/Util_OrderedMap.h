#pragma once

#include <list>
#include <map>

namespace Dv
{
	namespace Utils
	{
		namespace Container
		{
			/*******************************************************************************************
			* typedefs
			*******************************************************************************************/					
			template<class K, class V>
			struct OrderMapValue
			{
				/**************************************************************************
				* typedefs
				**************************************************************************/
				typedef std::map<K,int>						MapCont;
				typedef typename MapCont::iterator			MapCont_iterator;

				/**************************************************************************
				* public variables
				**************************************************************************/
				typename V									value;
				typename MapCont_iterator					iterator_to_mapped;

				/**************************************************************************
				* ctors
				**************************************************************************/				
				inline OrderMapValue(const V& InValue, const MapCont_iterator& InIteratorToMapped);
			};

			template<class K, class V>
			class OrderedMap
			{
			public:				
				/********************************************************************************
				* typedefs
				********************************************************************************/
				// Container used of the mapping of the key to order index of the mapped value
				typedef std::map<K,int>												MapCont;				
				typedef std::vector<OrderMapValue<K,V>>								OrderedCont;	
				typedef OrderMapValue<typename K, typename V>						MappedValue;

				/********************************************************************************
				* Clear all elements
				********************************************************************************/
				void											Clear								();

				/********************************************************************************
				* Adding of the new elements
				********************************************************************************/
				// Add the key/value pair and return the index of the added pair
				int												Add									(const K& InKey, const V& InValue);				

				/********************************************************************************
				* enuemration
				********************************************************************************/
				inline int										GetCount							()										const			{ return _order_vector.size(); }

				// Accessing the given order index
				inline const OrderMapValue<K,V>&				GetByIndex							(int InOrderIndex)						const;
				inline OrderMapValue<K,V>&						GetByIndex							(int InOrderIndex);

				// Getting order index by the key value
				inline int										GetOrderByKey						(const K& InKey)						const;
				inline int										GetOrderByKey						(const K& InKey);

			private:
				OrderedCont			  _order_vector;
				MapCont							_mp;
			};		

			



			/************************************************************************************************************************
			* IMPLEMENTATION
			************************************************************************************************************************/
			template<class K, class V>
			void OrderedMap<K,V>::Clear()
			{
				_order_vector.clear();
				_mp.clear();
			}

			template<class K, class V>
			int OrderedMap<K,V>::Add(const K& InKey, const V& InValue)
			{
				auto mp_it = _mp.insert(std::make_pair(InKey, _mp.size()));
				OrderMapValue<K,V> newValue(InValue, mp_it.first);
				_order_vector.push_back(newValue);
			}

			template<class K, class V>
			const OrderMapValue<K,V>&	OrderedMap<K,V>::GetByIndex(int InOrderIndex) const
			{
				return _order_vector[InOrderIndex];
			}

			template<class K, class V>
			OrderMapValue<K,V>&		OrderedMap<K,V>::GetByIndex(int InOrderIndex)
			{
				return _order_vector[InOrderIndex];
			}
			
			template<class K, class V>
			int	OrderedMap<K,V>::GetOrderByKey(const K& InKey) const
			{
				return _mp[InKey];
			}

			template<class K, class V>
			int	OrderedMap<K,V>::GetOrderByKey(const K& InKey)
			{
				return _mp[InKey];
			}

			template<class K, class V>
			OrderMapValue<K,V>::OrderMapValue(const V& InValue, const MapCont_iterator& InIteratorToMapped) :
				value(InValue),
				iterator_to_mapped(InIteratorToMapped)
			{
			}
		} // Container
	} // Util
} // Dv