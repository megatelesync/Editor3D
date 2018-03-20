#ifndef _TriangleMap_h_
#define _TriangleMap_h_

#include <map>
#include <set>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			template<class KeyType, class ValueType> using TriangleMapMap = std::map<KeyType,std::map<KeyType,ValueType>>;
			template<class KeyType> using TriangleMapKeySet = std::set<KeyType>;

			template<class KeyType, class ValueType> class TriangleMap
			{
			public:			
				bool RegisterKey(KeyType key);
				TriangleMapKeySet::const_iterator GetKeyBeginIterator();
				TriangleMapKeySet::const_iterator GetKeyEndIterator();
				bool IsKeyRegistered(KeyType key);
				void RemoveKey(KeyType key);
				void Set(KeyType keyA, KeyType keyB, ValueType newValue);
				ValueType Get(KeyType keyA, KeyType keyB);
				TriangleMapMap::iterator GetNextAdjKeyBeginIterator(KeyType keyA);
				TriangleMapMap::iterator GetNextAdjKeyEndIterator(KeyType keyA);

			private:
				TriangleMapMap _mapMap;
				TriangleMapKeySet _keySet;
				void _SetMapMapForKey(TriangleMapKeySet::const_iterator newKeyIt);
			};

			template<class KeyType, class ValueType>
			void TriangleMap<KeyType,ValueType>::_SetMapMapForKey(TriangleMapKeySet::const_iterator newKeyIt)
			{
				auto keyAIterator = _keySet.cbegin();
				KeyType newKey = _keySet[newKeyIt];
				// Add elements to rows before newKey row
				while(keyAIterator != newKeyIt)
				{
					auto keyA = *keyAIterator;
					auto& mp = _mapMap[keyA].second;
					mp[newKey] = ValueType();
					++keyAIterator;
				}			
				_mapMap[newKey]=std::map<KeyType,ValueType>();
				auto& newKeyMap=_mapMap[newKey];
				// Add elements to row for newKey
				auto keyBIterator = newKeyIt;
				++keyBIterator;
				while(keyBIterator != _keySet.cend())			
				{
					auto keyB = *keyBIterator;
					newKeyMap[keyB] = ValueType();
					++keyBIterator;
				}
			}

			template<class KeyType, class ValueType> 
			bool TriangleMap<KeyType,ValueType>::RegisterKey(KeyType key)
			{
				std::pair<TriangleMapSet::iterator,bool> insertRes = _keySet.insert(key);
				bool bInserted = insertRes.first;
				if(bInserted)
				{
					_SetMapMapForKey(insertRes.second);
				}
				return bInserted;
			}

			template<class KeyType, class ValueType> 
			TriangleMapKeySet::const_iterator TriangleMap<KeyType,ValueType>::GetKeyBeginIterator()
			{
				return _keySet.cbegin();
			}

			template<class KeyType, class ValueType>
			TriangleMapKeySet::const_iterator TriangleMap<KeyType,ValueType>::GetKeyEndIterator()
			{
				return _keySet.cend();
			}

			template<class KeyType, class ValueType>
			bool TriangleMap<KeyType,ValueType>::IsKeyRegistered(KeyType key)
			{
				return _keySet.find(key) != _keySet.cend();
			}

			template<class KeyType, class ValueType>
			void TriangleMap<KeyType,ValueType>::RemoveKey(KeyType key)
			{
				_keySet.erase(key);
				_mapMap.erase(key);
				for(auto keyA : keySet)
				{
					auto& mp = _mapMaps[keyA];
					mp.erase(key);
				}
			}

			template<class KeyType, class ValueType>
			void TriangleMap<KeyType,ValueType>::Set(KeyType keyA, KeyType keyB, ValueType newValue)
			{
				auto& keyAMap = _mapMaps[keyA];
				keyAMap[keyB] = newValue;
			}

			template<class KeyType, class ValueType>
			ValueType TriangleMap<KeyType,ValueType>::Get(KeyType keyA, KeyType keyB)
			{
				auto& keyAMap = _mapMaps[keyA];
				return keyAMap[keyB];
			}

			template<class KeyType, class ValueType>
			TriangleMapMap::iterator TriangleMap<KeyType,ValueType>::GetNextAdjKeyBeginIterator(KeyType keyA)
			{
				return _mapMaps[keyA].begin();
			}

			template<class KeyType, class ValueType>
			TriangleMapMap::iterator TriangleMap<KeyType,ValueType>::GetNextAdjKeyEndIterator(KeyType keyA)
			{
				return _mapMaps[keyA].end();
			}
		}	
	}		
}

#endif