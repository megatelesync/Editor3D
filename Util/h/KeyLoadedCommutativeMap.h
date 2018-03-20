#ifndef _KeyLoadedCommutativeMap_h_
#define _KeyLoadedCommutativeMap_h_

#include "CommutativeMap2D.h"
#include <unordered_map>
#include <boost/serialization/unordered_map.hpp>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{
			template<class KeyValueTypeArg, class ValueTypeArg>
			struct KeyValueCommutativeMapImplValue
			{
				typedef std::pair<KeyValueTypeArg, ValueTypeArg>		PairType;

				KeyValueTypeArg				key;
				ValueTypeArg				value;
				operator PairType() { return PairType(key,value); }

				KeyValueCommutativeMapImplValue() {}
				KeyValueCommutativeMapImplValue(const KeyValueTypeArg& keyIn, const ValueTypeArg& valueIn) :
					key(keyIn), value(valueIn) {}
			};

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			class KeyLoadedCommutativeMap
			{
			public:
				friend boost::serialization::access;

				typedef KeyTypeArg																						KeyType;
				typedef KeyValueTypeArg																					KeyValueType;
				typedef RowContTypeArg																					RowContType;

				typedef CommutativeMap2D<KeyTypeArg, ValueTypeArg, RowContTypeArg>										ImplContType;
				typedef	typename ImplContType::value_type																ImplContValueType;
				typedef	typename ImplContType::mapped_value																ImplContMappedValueType;
				typedef typename ImplContType::RowItType																ImplIteratorType;
				typedef typename ImplContType::RowConstItType															ImplConstIteratorType;

				typedef typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>		ThisType;	
				typedef typename ImplIteratorType																		iterator;
				typedef typename ImplConstIteratorType																	const_iterator;
				typedef typename ImplContType::InsertRes																InsertRes;
				typedef typename ImplContValueType																		ValueType;
				typedef typename ImplContMappedValueType																mapped_type;

				KeyLoadedCommutativeMap							();
				KeyLoadedCommutativeMap							(const ThisType& other);
				KeyLoadedCommutativeMap							(ThisType&& other);

				ThisType& operator=								(const ThisType& other);
				ThisType& operator=								(ThisType&& other);

				// Working with key values
				KeyValueTypeArg				get_key_value		(const KeyTypeArg& key) const		   { return _keyValues.find(key)->second; }
				void						set_key_value		(const KeyTypeArg& key, const KeyValueTypeArg& newValue);

				ImplContMappedValueType operator[] (const KeyTypeArg& key);

				iterator					find				(const KeyTypeArg& key);
				const_iterator				find				(const KeyTypeArg& key) const;

				size_t									numKeys()				  const			{ return _cont.size(); }
				size_t									size()					  const			{ return _cont.size(); }

				typename const_iterator				    cbegin()				  const			{ return _cont.cbegin(); }		
				typename const_iterator				    cend()					  const			{ return _cont.cend(); }
				typename iterator						begin()					  const			{ return _cont.cbegin(); }		
				typename iterator						end()					  const			{ return _cont.cend(); }

				void									set   (KeyType keyA, KeyType keyB, const ValueTypeArg& newValue);
				const ValueTypeArg&						at	  (KeyType keyA, KeyType keyB) const											{ return _rows[keyA][keyB]; }
				ValueTypeArg&							at	  (KeyType keyA, KeyType keyB) 													{ return _rows[keyA][keyB]; }
				size_t									erase (KeyType keyA, KeyType keyB);
				void									erase (KeyType key);
				void									clear ();

				// tries to insert "keyA, keyB" if keyA and keyB not exist together)
				// returnes: pair of iterator and bool value, equal to true if insertion occured	
				// (assigns a default-constructible value for key if the key does not exist)
				InsertRes								insert(KeyType keyA, KeyType keyB, const ValueTypeArg& newValue);

				template<class Archive>
				void serialize(Archive &ar, const unsigned int version);

			private:
				ImplContType _cont;
				std::unordered_map<KeyTypeArg,KeyValueTypeArg> _keyValues;
			};

			// Get map of pairs <KeyValueTypeArg, ValueTypeArg>
			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			std::map<KeyValueTypeArg, ValueTypeArg> GetKeyValueToValueMap
			(
				const KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>& mp,
				typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::const_iterator iterator
			);

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			template<class Archive>
			void KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::serialize(Archive &ar, const unsigned int version)
			{
				ar & _keyValues;
				ar & _cont;				
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::KeyLoadedCommutativeMap() {}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::KeyLoadedCommutativeMap(const ThisType& other) :
				_cont(other._cont)
				_keyValues(other._keyValues)
			{
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::KeyLoadedCommutativeMap(ThisType&& other) :
				_cont(std::move(other._cont))
				_keyValues(std::move(other._keyValues))
			{
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::ThisType& 
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::operator=(const ThisType& other)
			{
				_cont = other._cont;
				_keyValues = other._keyValues;
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::ThisType& 
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::operator=(ThisType&& other)
			{
				_cont = std::move(other._cont);
				_keyValues = std::move(other._keyValues);
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::ImplContMappedValueType 
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::operator[] (const KeyTypeArg& key)
			{
				return _cont[key];
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::iterator 
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::find(const KeyTypeArg& key)
			{
				return _cont.find(key);
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			void KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::set(KeyType keyA, KeyType keyB, const ValueTypeArg& newValue)
			{
				_cont.set(keyA, keyB, newValue);
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			size_t KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::erase (KeyType keyA, KeyType keyB)
			{
				auto res = _cont.erase(keyA, keyB);

				if(NumAdjKeys(_cont, keyA) == 0)
				{
					_keyValues.erase(keyA);
				}

				if(NumAdjKeys(_cont, keyB) == 0)
				{
					_keyValues.erase(keyB);
				}

				return res;
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			void KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::erase (KeyType key)
			{				
				_keyValues.erase(key);
				_cont.erase(key);
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			void KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::clear()
			{
				_cont.clear();
				_keyValues.clear();
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::InsertRes								
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::insert(KeyType keyA, KeyType keyB, const ValueTypeArg& newValue)
			{
				InsertRes insertRes = _cont.insert(keyA, keyB, newValue);
				auto keyAIt = _keyValues.find(keyA);				
				if(keyAIt == _keyValues.cend())
				{
					keyAIt->second = KeyValueTypeArg();
				}

				auto keyBIt = _keyValues.find(keyB);
				if(keyBIt == _keyValues.cend())
				{
					keyBIt->second = KeyValueTypeArg();
				}
				return insertRes;
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			void KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::set_key_value(const KeyTypeArg& key, const KeyValueTypeArg& newValue)
			{
				_keyValues[key] = newValue;
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::const_iterator				
			KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::find(const KeyTypeArg& key) const
			{
				return _cont.find(key);
			}

			template<class KeyTypeArg, class ValueTypeArg, class KeyValueTypeArg, class RowContTypeArg>
			std::map<KeyValueTypeArg, ValueTypeArg> 
			GetKeyValueToValueMap
			(
				const KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>& mp,
				typename KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>::const_iterator iterator
			)
			{
				typedef KeyLoadedCommutativeMap<KeyTypeArg, ValueTypeArg, KeyValueTypeArg, RowContTypeArg>		MpContType;				
				typedef typename MpContType::RowContType														MpRowContType;

				std::map<KeyValueTypeArg, ValueTypeArg> res_mp;
				const auto& rowMap = iterator->second;
				std::for_each
				(
					rowMap.cbegin(), rowMap.cend(),
					[&res_mp, &mp](typename const MpRowContType::value_type& elem)
					{
						auto key=elem.first;
						auto value=elem.second;
						auto keyValue = mp.get_key_value(key);
						res_mp.insert(std::make_pair(keyValue,value));
					}
				);
				return res_mp;
			}
		} // Container
	} // Util
} // Dv

#endif // _KeyLoadedCommutativeMap_h_