#ifndef _CommutiveMap2D_h_
#define _CommutiveMap2D_h_

#include <cassert>
#include <iomanip>
#include <map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> class CommutativeMap2D
			{
			public:
				friend class boost::serialization::access;

				typedef KeyTypeArg											KeyType;
				typedef ValueTypeArg										ValueType;
				// Row container type
				typedef RowContTypeArg										RowContType;
				// Impl container type
				typedef std::map<KeyType,RowContType>						Cont2DType;				

				// Const iterator to the row
				typedef typename Cont2DType::const_iterator					RowConstItType;
				// Non-const iterator to the row
				typedef typename Cont2DType::iterator						RowItType;
				// Const iterator to the row element
				typedef typename RowContType::const_iterator				RowElementConstItType;
				// Non-const iterator to the row element
				typedef typename RowContType::iterator						RowElementItType;
				// Insert result type
				typedef std::pair<RowElementItType,bool>					InsertRes;				

				typedef typename Cont2DType::mapped_type					mapped_value;
				typedef typename Cont2DType::value_type						value_type;

				// pair of iterators denoting particular element
				typedef std::pair<RowConstItType,RowElementConstItType>		const_iterator;
				typedef std::pair<RowItType,RowElementItType>				iterator;

				CommutativeMap2D() {}
				CommutativeMap2D(const CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>& other) : _rows(other._rows)			{}
				CommutativeMap2D(CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>&& other) : _rows(std::move(other._other))	    {}
				CommutativeMap2D& operator=(const CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>& other)						{ _rows=other._rows; }
				CommutativeMap2D& operator=(CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>&& other)							{ _rows = std::move(other._rows); }
				
				const RowContType&						operator[]	(KeyType key) const			{ return _rows.find(key)->second; }
				// returns cend() if no such key defined
				typename RowConstItType				    find		(KeyType key) const			{ return _rows.find(key); }
				typename RowItType					    find		(KeyType key) 				{ return _rows.find(key); }

				// number of keys (both size and numRows return the same)
				size_t									numKeys()				  const			{ return _rows.size(); }
				size_t									size()					  const			{ return _rows.size(); }

				// get row iterators
				typename RowConstItType				    cbegin()				  const			{ return _rows.cbegin(); }		
				typename RowConstItType				    cend()					  const			{ return _rows.cend(); }
				typename RowConstItType				    begin()					  const			{ return _rows.cbegin(); }		
				typename RowConstItType				    end()					  const			{ return _rows.cend(); }
				
				void									set(KeyType keyA, KeyType keyB, const ValueType& newValue);
				const ValueType&						at(KeyType keyA, KeyType keyB) const											{ return _rows[keyA][keyB]; }
				size_t									erase(KeyType keyA, KeyType keyB);
				void									erase(KeyType key);
				void									clear();
				// tries to insert "keyA, keyB" if keyA and keyB not exist together)
				// returnes: pair of iterator and bool value, equal to true if insertion occured
				InsertRes								insert(KeyType keyA, KeyType keyB, const ValueType& newValue);	

				// boost serialization
				template<class Archive> void serialize(Archive &ar, const unsigned int version);

			private:				
				Cont2DType _rows;
			};				
			//BOOST_CLASS_VERSION(CommutativeMap2D, 1);

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			bool ContainsKey(const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg> &mp, const KeyTypeArg& key);

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			bool ContainsEdge(const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg> &mp, const KeyTypeArg& keyA, const KeyTypeArg& keyB);

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			size_t NumAdjKeys(const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg> &mp, const KeyTypeArg& keyA);

			template<class Strm, class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			Strm& operator<<(Strm& oStrm, const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>& cont);			

			// Impl
			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			template<class Archive> 
			void CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>::serialize(Archive &ar, const unsigned int version)
			{
				ar & _rows;
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> bool ContainsKey(const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg> &mp, const KeyTypeArg& key)
			{
				return mp.find(key) != mp.end();
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> bool ContainsEdge(const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg> &mp, const KeyTypeArg& keyA, const KeyTypeArg& keyB)
			{
				CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>::RowElementConstItType keyAIt = mp.find(keyA); 
				const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>::RowContType& row = keyAIt->second;
				return keyAIt != _rows.end() && row.find(keyB) != row.end();
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			size_t NumAdjKeys(const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg> &mp, const KeyTypeArg& keyA)
			{
				auto it = mp.find(keyA);
				if(it == mp.cend()) 
				{
					return 0; 
				}
				else
				{
					return it->second.size();
				}
			}

			template<class Strm, class KeyTypeArg, class ValueTypeArg, class RowContTypeArg> 
			Strm& operator<<(Strm& oStrm, const CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>& cont)
			{
				for(std::pair<KeyTypeArg, RowContTypeArg> rowPair : cont)
				{
					KeyTypeArg rowKey = rowPair.first;
					const RowContTypeArg& row = rowPair.second;
					std::cout << rowKey << ":";
					for(const auto& elemPair : row)
					{
						KeyTypeArg key = elemPair.first;
						const ValueTypeArg& value = elemPair.second;
						std::cout << "{" << key << ":" << value << "} ";
					}
					std::cout << std::endl;
				}
				return oStrm;
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg>
			void CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>::set(KeyType keyA, KeyType keyB, const ValueType& newValue)
			{ 
				assert(keyA != keyB); 
				_rows[keyA][keyB] = newValue; 
				_rows[keyB][keyA] = newValue;				
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg>
			size_t CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>::erase(KeyType keyA, KeyType keyB)
			{
				assert(keyA != keyB);
				RowItType rowAIt = _rows.find(keyA);
				RowItType rowBIt = _rows.find(keyB);
				if(rowAIt == _rows.end()) { return 0; }
				if(rowBIt == _rows.end()) { return 0; }
				RowContType& rowA = rowAIt->second;
				RowContType& rowB = rowBIt->second;
				rowA.erase(keyB);
				size_t eraseRes = rowB.erase(keyA);				
				if(rowA.size() == 0) { _rows.erase(keyA); }
				if(rowB.size() == 0) { _rows.erase(keyB); }
				return eraseRes;
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg>
			void CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>::erase(KeyType key)
			{
				RowItType keyRowIt = find(key);
				if(keyRowIt != _rows.end())
				{
					for(auto keyValue : keyRowIt->second)
					{
						KeyType adjKey = keyValue.first;
						RowContType &adjRow = _rows[adjKey];
						adjRow.erase(key);
					}
					_rows.erase(key);
				}
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg>
			void CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>::clear()
			{
				_rows.clear();
			}

			template<class KeyTypeArg, class ValueTypeArg, class RowContTypeArg>
			typename CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>::InsertRes 
			CommutativeMap2D<KeyTypeArg,ValueTypeArg,RowContTypeArg>::insert(KeyType keyA, KeyType keyB, const ValueType& newValue)
			{
				assert(keyA != keyB);
				std::pair<Cont2DType::iterator,bool> rowInsertRes = _rows.insert(std::make_pair(keyA,RowContType()));
				bool bRowKeyInserted = rowInsertRes.second;
				RowItType insertedRowIter = rowInsertRes.first;
				RowContType& row = insertedRowIter->second;
				std::pair<RowElementItType,bool> valueInsertRes = row.insert(std::make_pair(keyB,newValue));
				RowElementItType insertedPairIterator = valueInsertRes.first;
				auto bNewPairAdded=valueInsertRes.second;
				if(bNewPairAdded)
				{
					// add symmetrical pair
					auto keyBRowInsertRes = _rows.insert(std::make_pair(keyB,RowContType()));				
					auto keyBRowIterator = keyBRowInsertRes.first;
					RowContType& keyBRow = keyBRowIterator->second;
					keyBRow.insert(std::make_pair(keyA,newValue));
				}				
				return valueInsertRes;
			}
		} // Container
	} // Util
} // Dv

#endif // _CommutiveMap2D_h_