#ifndef _IteratorMap_h_
#define _IteratorMap_h_

#include <map>
#include <boost/tuple/tuple.hpp>
#include <type_traits>

namespace Dv
{
	namespace Util
	{
		namespace Container
		{	
			template<class T1, class T2>
			struct PairTuple : public boost::tuples::tuple<T1, T2>
			{
				typedef PairTuple<T1, T2>														ThisType;
				typedef boost::tuples::tuple<T1, T2>											TupleType;
				typedef typename std::remove_reference<T1>::type								T1NoRef;
				typedef typename std::remove_reference<T2>::type								T2NoRef;
				typedef std::pair<T1NoRef, T2NoRef>												PairTypeNoRef;

				T1& first;
				T2& second;

				PairTuple(T1 val1, T2 val2) : tuple(val1, val2), first(get<0>()), second(get<1>()) {}
				PairTuple(const PairTuple& other) : tuple(other), first(get<0>()), second(get<1>()) {}
				PairTuple(const TupleType& other) : tuple(other), first(get<0>()), second(get<1>()) {}
				operator PairTypeNoRef() const { return PairTypeNoRef(first, second); }
			};

			// fwd declarations					
			template<class IteratorKeyArg, class TArg> struct IteratorMapMappedValue;
			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg> struct IteratorMapTypedefs;
			template<class IteratorMapTypedefs> class ItMapIterator;
			template<class IteratorMapTypedefs> class ConstItMapIterator;
			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg> class IteratorMap;			
			
			template<class IteratorKeyArg, class TArg>
			struct IteratorMapMappedValue
			{
				IteratorKeyArg				iterator;
				TArg						value;

				IteratorMapMappedValue();
				IteratorMapMappedValue(IteratorKeyArg iteratorIn, const TArg& valueIn);
			};

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			struct IteratorMapTypedefs
			{
				typedef IteratorMap<IteratorKeyArg, ConstIteratorKeyArg, TArg>				ThisType;
				// IteratorValueArg - value that the iterator points to				
				typedef typename IteratorKeyArg::reference									IteratorValue;
				typedef typename IteratorKeyArg::pointer									IteratorPointerValue;
				typedef IteratorKeyArg														IteratorKey;
				typedef ConstIteratorKeyArg													ConstIteratorKey;

				// Container typedefs
				typedef typename IteratorMapMappedValue<IteratorKeyArg, TArg>				ContT;
				typedef std::map<IteratorPointerValue, ContT>								ContType;

				typedef typename ContType::value_type										Cont_value_type;
				typedef typename ContType::iterator											Cont_iterator;
				typedef typename ContType::const_iterator									Cont_const_iterator;

				// typedefs
				typedef typename boost::tuples::tuple<const IteratorKeyArg&,TArg&>			value_type_ref;
				typedef typename std::pair<const IteratorKeyArg,TArg>						value_type;				
				typedef typename IteratorKeyArg												key_type;
				typedef typename TArg														mapped_type;		
			};

			// Base class for the iteratora of the IteratorMap
			template<class IteratorMapTypedefs, class ImplIteratorArg>
			class ItMapIteratorBase : 
				public std::iterator<std::bidirectional_iterator_tag, typename IteratorMapTypedefs::value_type>
			{
			public:
				typedef ImplIteratorArg													 ImplIterator;
				typedef IteratorMapTypedefs												 typedefs;
				typedef ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>			 ThisType;				
				typedef typename IteratorMapTypedefs::mapped_type&						 Reference;
				typedef typename IteratorMapTypedefs::mapped_type*						 Pointer;
				typedef typename IteratorMapTypedefs::key_type					         key_type;
				typedef typename IteratorMapTypedefs::mapped_type						 mapped_type;
				typedef typename PairTuple<const key_type&, mapped_type&>				 value_type_pair_tuple_ref;
				typedef typename IteratorMapTypedefs::value_type						 value_type;
				typedef typename value_type_pair_tuple_ref								value_type_ref;
				typedef typename value_type_pair_tuple_ref*								 PointerToValue;				

				ItMapIteratorBase() {}
				ItMapIteratorBase												(ImplIteratorArg iteratorIn);
				ItMapIteratorBase												(const ThisType& other);
				typename ThisType&							operator=			(const ThisType& other);
				PointerToValue								operator->			() const;
				typename value_type_ref						operator*			() const;
				typename ThisType&							operator++			();
				typename ThisType							operator++			(int);
				typename ThisType&							operator--			();
				typename ThisType							operator--			(int);		

				ImplIteratorArg								GetIterator			() const { return _iterator; }

			private:
				ImplIteratorArg								_iterator;
				// Current result of the iterator dereferencing
				// (may be not valid)
				mutable std::unique_ptr<value_type_pair_tuple_ref> _pKeyValuePair;
				value_type_ref _GetValueType() const;				
			};						

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			bool operator== (ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> first, ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> second);
			template<class IteratorMapTypedefs, class ImplIteratorArg>
			bool operator!=	(ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> first, ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> second);

			template<class IteratorMapTypedefs>
			class ConstItMapIterator : 
				public ItMapIteratorBase<IteratorMapTypedefs, typename IteratorMapTypedefs::Cont_const_iterator>
			{
			};

			template<class IteratorMapTypedefs>
			class ItMapIterator : 
				public ItMapIteratorBase<IteratorMapTypedefs, typename IteratorMapTypedefs::Cont_iterator>
			{
			public:
				typedef ConstItMapIterator<IteratorMapTypedefs>						ConstIterator;

				ItMapIterator() : ItMapIteratorBase() {}
				ItMapIterator(ImplIterator implIteratorIn) : ItMapIteratorBase(implIteratorIn) {}				

				operator ConstIterator() const;
			};

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg> class IteratorMap
			{
			public:
				typedef IteratorMapTypedefs<IteratorKeyArg,ConstIteratorKeyArg,TArg>		typedefs;

				typedef typename typedefs::ThisType											ThisType;
				// IteratorValueArg - value that the iterator points to				
				typedef typename typedefs::IteratorValue									IteratorValue;
				typedef typename typedefs::IteratorPointerValue								IteratorPointerValue;
				typedef typename typedefs::IteratorKey										IteratorKey;
				typedef typename typedefs::ConstIteratorKey									ConstIteratorKey;

				// Container typedefs
				typedef typename typedefs::ContT											ContT;
				typedef typename typedefs::ContType											ContType;
				typedef typename typedefs::Cont_value_type									Cont_value_type;

				typedef typename ItMapIterator<typedefs>									iterator;
				typedef typename ConstItMapIterator<typedefs>								const_iterator;

				typedef size_t																size_type;
				typedef typename typedefs::mapped_type										mapped_type;
				typedef typename typedefs::key_type											key_type;
				typedef typename typedefs::value_type										value_type;
				typedef typename typedefs::value_type_ref									value_type_ref;

				typedef std::pair<typename iterator,bool>									insert_result;
				typedef std::pair<typename iterator, typename iterator>						iterator_range;
				typedef std::pair<typename const_iterator, typename const_iterator>			const_iterator_range;

				IteratorMap();
				template<class Iterator>		
				IteratorMap											(Iterator first, Iterator last);
				IteratorMap											(const ContType& other);
				IteratorMap											(const ThisType& other);
				IteratorMap											(ThisType&& other);
				IteratorMap											(ContType&& other);

				ThisType&							operator=		(const ThisType& other);
				ThisType&							operator=		(ThisType&& other);

				iterator							begin();
				iterator							end();
				const_iterator						cbegin()			const;
				const_iterator						cend()				const;

				bool								empty()				const;
				size_type							size()				const;
				size_type							max_size()			const;

				const mapped_type&					operator[]			(const key_type& k)		const;
				mapped_type&						operator[]			(const key_type& k);
				const mapped_type&					at					(const key_type& k)     const;
				mapped_type&						at					(const key_type& k);				

				insert_result						insert				(const value_type& val);
				iterator							insert				(iterator position, const value_type& val);
				template <class InputIterator>
				void								insert				(InputIterator first, InputIterator last);

				void								erase				(iterator position);
				size_type							erase				(const key_type& k);
				void								erase				(iterator first, iterator last);

				void								swap				(ThisType& x);
				void								clear();

				iterator							find				(const key_type& k);
				const_iterator						find				(const key_type& k) const;

				size_type							count				(const key_type& k) const;

				iterator							lower_bound			(const key_type& k);
				const_iterator						lower_bound			(const key_type& k) const;

				iterator							upper_bound			(const key_type& k);
				const_iterator						upper_bound			(const key_type& k) const;

				iterator_range						equal_range			(const key_type& k);
				const_iterator_range				equal_range			(const key_type& k) const;

			private:
				ContType							_cont;
				Cont_value_type						_GetContValueType	(const value_type& val);
			};
			// Helper function for getting address of the value pointed to by the given dereferenceble object
			template<class DereferencableType, class T> T *GetAddressOfPointedValue(DereferencableType dereferencable) 
			{ 
				return &(*dereferencable); 
			}

			// IteratorMap impl
			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::IteratorMap () : _cont() {}
			
			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			template<class Iterator>
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::IteratorMap (Iterator first, Iterator last) :
				_cont(first,last)
			{
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::IteratorMap(const ContType& other) :
				_cont(other._cont)
			{
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::IteratorMap(const ThisType& other) :
				_cont(other)
			{
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::IteratorMap(ThisType&& other) :
				_cont(std::move(other._cont))
			{
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::IteratorMap(ContType&& other) :
				_cont(std::move(other))
			{
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::ThisType&
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::operator=	(const ThisType& other)
			{
				_cont = other._cont;
				return *this;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::ThisType&						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::operator=	(ThisType&& other)
			{
				_cont = std::move(other._cont);
				return *this;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator	
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::begin()
			{
				return iterator(_cont.begin());
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator							
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::end()
			{
				return iterator(_cont.end());
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::const_iterator						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::cbegin() const
			{
				return const_iterator(_cont.cbegin());
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::const_iterator						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::cend() const
			{
				return const_iterator(_cont.cend());
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			bool IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::empty() const
			{
				return _cont.empty();
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::size_type 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::size() const
			{
				return _cont.size();
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::size_type 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::max_size() const
			{
				return _cont.max_size();
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			const typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::mapped_type& 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::operator[] (const key_type& k) const
			{
				return at(k);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::mapped_type& 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::operator[] (const key_type& k)
			{
				return at(k);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			const typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::mapped_type& 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::at (const key_type& k) const
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				const auto& cont_mapped_value = _cont[cont_key];
				auto& mapped_value = cont_mapped_value.value;
				return mapped_value;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::mapped_type& 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::at (const key_type& k)
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto& new_cont_mapped_value = _cont[cont_key];
				new_cont_mapped_value.iterator = k;
				auto &mapped_value = new_cont_mapped_value.value;
				return mapped_value;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::Cont_value_type 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::_GetContValueType(const value_type& val)
			{
				auto key = val.first;
				auto mapped_value = val.second;
				auto& value_pointed_by_iterator = *key;
				auto cont_mapped_value = ContT(key, mapped_value);
				auto new_pair = Cont_value_type(&value_pointed_by_iterator, cont_mapped_value);
				return new_pair;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::insert_result						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::insert(const value_type& val)
			{
				insert_result res;					
				auto new_pair = _GetContValueType(val);
				auto insert_res = _cont.insert(new_pair);
				res = std::make_pair(iterator(insert_res.first), insert_res.second);
				return res;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator							
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::insert(iterator position, const value_type& val)
			{
				auto new_pair = _GetContValueType(val);
				auto iterator_to_inserted = _cont.insert(position, new_pair);
				return IteratorKeyArg(iterator_to_inserted);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			template <class InputIterator>
			void IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::insert(InputIterator first, InputIterator last)
			{
				std::for_each
				(
					first, last,
					[this](const value_type& valueToInsert)
					{
						this->insert(valueToInsert);
					}
				);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			void IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::erase(iterator position)
			{
				_cont.erase(position.GetIterator());
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::size_type
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::erase(const key_type& k)
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k)
				size_type res = _cont.erase(cont_key);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			void IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::erase (iterator first, iterator last)
			{
				auto cont_key_first = first.GetIterator();
				auto cont_key_last = last.GetIterator();
				_cont.erase(cont_key_first, cont_key_last);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			void IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::swap(ThisType& x)
			{
				_cont.swap(x._cont);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			void IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::clear()
			{
				_cont.clear();
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::find(const key_type& k)
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_iterator = _cont.find(cont_key);
				auto res = iterator(cont_iterator);
				return res;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::const_iterator 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::find(const key_type& k) const
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_iterator = _cont.find(cont_key);
				auto res = const_iterator(cont_iterator);
				return res;
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::size_type 
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::count	(const key_type& k) const
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				return _cont.count(cont_key);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator		
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::lower_bound (const key_type& k)
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_iterator = _cont.lower_bound(cont_key);
				return iterator(cont_iterator);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::const_iterator						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::lower_bound (const key_type& k) const
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_iterator = _cont.lower_bound(cont_key);
				return const_iterator(cont_iterator);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator							
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::upper_bound (const key_type& k)
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_iterator = _cont.upper_bound(cont_key);
				return iterator(cont_iterator);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::const_iterator						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::upper_bound (const key_type& k) const
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_iterator = _cont.upper_bound(cont_key);
				return const_iterator(cont_iterator);
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::iterator_range						
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::equal_range (const key_type& k)
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_range = _cont.equal_range(cont_key);
				return iterator_range(iterator(cont_range.first), iterator(cont_range.second));
			}

			template<class IteratorKeyArg, class ConstIteratorKeyArg, class TArg>
			typename IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::const_iterator_range				
			IteratorMap<IteratorKeyArg,ConstIteratorKeyArg,TArg>::equal_range (const key_type& k) const
			{
				auto cont_key = GetAddressOfPointedValue<key_type, typename key_type::value_type>(k);
				auto cont_range = _cont.equal_range(cont_key);
				return const_iterator_range(const_iterator(cont_range.first), const_iterator(cont_range.second));
			}

			// ItMapIteratorBase impl
			template<class IteratorMapTypedefs, class ImplIteratorArg>
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ItMapIteratorBase (ImplIteratorArg iteratorIn) : _iterator(iteratorIn) {}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ItMapIteratorBase (const ThisType& other) : _iterator(other._iterator) {}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ThisType&	ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator=(const ThisType& other)
			{
				_iterator = other._iterator;
				return *this;
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::value_type_ref
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator*() const
			{
				return _GetValueType();
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::PointerToValue
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator->() const
			{
				_pKeyValuePair.reset(new value_type_pair_tuple_ref(_GetValueType()));
				return _pKeyValuePair.get();
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::value_type_ref
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::_GetValueType() const
			{
				return value_type_ref(_iterator->second.iterator, _iterator->second.value);
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ThisType&
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator++()
			{
				++_iterator;
				return *this;
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ThisType 
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator++(int)
			{
				ThisType oldValue = *this;
				++_iterator;
				return oldValue;
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ThisType& 
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator--()
			{
				--_iterator;
				return *this;
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			typename ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::ThisType 
			ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg>::operator--(int)
			{
				ThisType oldValue = *this;
				--_iterator;
				return oldValue;
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			bool operator== (ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> first, ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> second)
			{
				return first.GetIterator() == second.GetIterator();
			}

			template<class IteratorMapTypedefs, class ImplIteratorArg>
			bool operator!=	(ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> first, ItMapIteratorBase<IteratorMapTypedefs,ImplIteratorArg> second)
			{
				return first.GetIterator() != second.GetIterator();
			}
			
			template<class IteratorMapTypedefs>
			ItMapIterator<IteratorMapTypedefs>::operator ConstIterator() const
			{
				return ConstIterator(_iterator);
			}

			// IteratorMapMappedValue impl
			template<class IteratorKeyArg, class TArg> 
			IteratorMapMappedValue<IteratorKeyArg,TArg>::IteratorMapMappedValue() : iterator(), value() {}

			template<class IteratorKeyArg, class TArg> 
			IteratorMapMappedValue<IteratorKeyArg,TArg>::IteratorMapMappedValue(IteratorKeyArg iteratorIn, const TArg& valueIn) :
				iterator(iteratorIn), value(valueIn)
			{
			}			
		} // Container
	} // Util
} // Dv

#endif // _IteratorMap_h_