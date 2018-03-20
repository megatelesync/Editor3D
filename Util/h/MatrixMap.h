#ifndef _MatrixMap_h_
#define _MatrixMap_h_

#include <map>
#include <ostream>
#include <iomanip>
#include <cassert>
#include <cstddef>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			//template<class KeyType, class ValueType> using MatrixMapMap = std::map<KeyType,std::map<KeyType,ValueType>>;

			// Foward declarations
			template<class KeyType, class ValueType> class MatrixMap;
			template<class KeyType, class ValueType> 
			std::ostream& operator<<(std::ostream& oStrm, const MatrixMap<KeyType,ValueType>& matrixMap);

			template<class KeyType, class ValueType> class MatrixMap
			{
			public:				
				typedef std::map<KeyType,std::map<KeyType,ValueType>> MatrixMapMap;
				template<class KeyType, class ValueType>
				friend std::ostream& operator<<(std::ostream& oStrm, const MatrixMap<KeyType,ValueType>& matrixMap);

				void RegisterKey(KeyType newKey);
				void RemoveKey(KeyType keyToRemove);
				int GetKeyCount() const;
				const ValueType& Get(KeyType keyA, KeyType keyB) const;
				ValueType& Get(KeyType keyA, KeyType keyB);				
				void Set(KeyType keyA, KeyType keyB, const ValueType& newValue);
				typename MatrixMapMap::const_iterator GetRowKeyBeginConstIterator(KeyType key) const;
				typename MatrixMapMap::const_iterator GetRowKeyEndConstIterator(KeyType key) const;
				typename MatrixMapMap::iterator GetRowKeyBeginIterator(KeyType key);
				typename MatrixMapMap::iterator GetRowKeyEndIterator(KeyType key);
				int GetRowKeyCount(KeyType key) const;				
			private:
				MatrixMapMap _mapMap;
			};

			template<class KeyType, class ValueType> std::ostream& operator<<(std::ostream& oStrm, const MatrixMap<KeyType,ValueType>& matrixMap)
			{
				assert(&oStrm);
				assert(&matrixMap);
				for(auto rowIt = matrixMap._mapMap.cbegin(); rowIt != matrixMap._mapMap.cend(); ++rowIt)
				{
					KeyType keyA = rowIt->first;
					auto& mp = rowIt->second;
					oStrm << keyA << ":";
					for(auto colIt = mp.cbegin(); colIt != mp.cend(); ++colIt)
					{
						KeyType keyB = colIt->first;
						ValueType val = colIt->second;
						oStrm << "{" << keyB << ":" << val << "}";
					}
					oStrm << std::endl;
				}
				return oStrm;
			}

			template<class KeyType, class ValueType> 
			typename MatrixMap<KeyType,ValueType>::MatrixMapMap::iterator MatrixMap<KeyType,ValueType>::GetRowKeyBeginIterator(KeyType key)
			{
				return _mapMap[key].begin();
			}

			template<class KeyType, class ValueType> 
			typename MatrixMap<KeyType,ValueType>::MatrixMapMap::iterator MatrixMap<KeyType,ValueType>::GetRowKeyEndIterator(KeyType key)
			{
				return _mapMap[key].end();
			}

			template<class KeyType, class ValueType> 
			typename MatrixMap<KeyType,ValueType>::MatrixMapMap::const_iterator MatrixMap<KeyType,ValueType>::GetRowKeyBeginConstIterator(KeyType key) const
			{
				return _mapMap[key].cbegin();
			}

			template<class KeyType, class ValueType> 
			typename MatrixMap<KeyType,ValueType>::MatrixMapMap::const_iterator MatrixMap<KeyType,ValueType>::GetRowKeyEndConstIterator(KeyType key) const
			{
				return _mapMap[key].end();
			}

			template<class KeyType, class ValueType> 
			int MatrixMap<KeyType,ValueType>::GetRowKeyCount(KeyType key) const
			{
				return _mapMap[key].size();
			}

			template<class KeyType, class ValueType> 
			void MatrixMap<KeyType,ValueType>::Set(KeyType keyA, KeyType keyB, const ValueType& newValue)
			{
				assert(keyA != keyB);
				_mapMap[keyA][keyB] = newValue;
				_mapMap[keyB][keyA] = newValue;
			}

			template<class KeyType, class ValueType> 
			ValueType& MatrixMap<KeyType,ValueType>::Get(KeyType keyA, KeyType keyB)
			{
				return _mapMap[keyA][keyB];
			}

			template<class KeyType, class ValueType> 
			const ValueType& MatrixMap<KeyType,ValueType>::Get(KeyType keyA, KeyType keyB) const
			{
				return _mapMap[keyA][keyB];
			}

			template<class KeyType, class ValueType> 
			int MatrixMap<KeyType,ValueType>::GetKeyCount() const
			{
				return _mapMap.size();
			}

			template<class KeyType, class ValueType> 
			void MatrixMap<KeyType,ValueType>::RemoveKey(KeyType keyToRemove)
			{
				std::size_t eraseRes = _mapMap.erase(keyToRemove);
				if(eraseRes == 1)
				{
					for(auto mapIt = _mapMap.begin(); mapIt != _mapMap.end(); ++mapIt)
					{
						auto& mp = mapIt->second;
						mp.erase(keyToRemove);
					}
				}
			}

			template<class KeyType, class ValueType> 
			void MatrixMap<KeyType,ValueType>::RegisterKey(KeyType newKey)
			{
				if(_mapMap.find(newKey) != _mapMap.end()) { return; }
				// generate map of all keys excluding newKey
				std::map<KeyType,ValueType> newKeyMap;
				for(auto keyIt = _mapMap.cbegin(); keyIt != _mapMap.cend(); ++keyIt)
				{
					KeyType key = keyIt->first;
					auto newKeyValue = std::make_pair(key,ValueType());
					newKeyMap.insert(newKeyValue);			
				}

				// insert newKey for all maps
				for(auto mapIt = _mapMap.begin(); mapIt != _mapMap.end(); ++mapIt)
				{
					auto &mp = mapIt->second;
					mp.insert(std::make_pair(newKey,ValueType()));
				}

				// insert the new map
				_mapMap.insert(std::make_pair(newKey,newKeyMap));
			}
		} // Container
	} // Util
} // DV

#endif