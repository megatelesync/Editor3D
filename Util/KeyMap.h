#ifndef _KeyMap_h_
#define _KeyMap_h_

#include <map>

namespace Dv
{
	namespace Utils
	{
		template<class KeyType, class KeyTypeConst, class T, class Compare = less<Key>> class KeyMap
		{
		public:
			typedef KeyMap<KeyType, KeyTypeConst, T, Compare>		ThisType;
			typedef std::map<KeyType,T,Compare>						ContType;
			typedef ContType::value_type							value_type;
			typedef std::pair<KeyTypeConst,T>						value_type_constkey;
			typedef ContType::iterator								iterator;
			typedef ContType::const_iterator						const_iterator;
			typedef ContType::reference								reference;
			typedef ContType::const_reference						const_reference;
			typedef ContType::key_compare							key_compare;
			typedef ContType::allocator_type						allocator_type;
			typedef ContType::mapped_type							mapped_type;
			typedef ContType::pointer								pointer;
			typedef ContType::const_pointer							const_pointer;
			typedef KeyTypeConst									key_type_const;

			// default ctor
			KeyMap();

			// ctor
			template<class InputIterator> 
			KeyMap
			(
				InputIterator first, InputIterator last, 
				const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type()
			);
			
			// number of elements
			size_t							size()														const			{ return _mp.size(); } 
			bool							empty()														const			{ return _mp.empty(); }
			size_type						max_size()													const			{ return _mp.max_size(); }

			// iterators			
			const_iterator					cbegin()													const			{ return _mp.cbegin(); }
			const_iterator                  cend()														const           { return _mp.cend(); }
			iterator						begin()																		{ return _mp.begin(); }
			iterator						end()																		{ return _mp.end(); }

			// insert
			pair<iterator,bool>				insert(const value_type& val)												{ return _mp.insert(val); }
			iterator						insert(iterator position, const value_type& val)							{ return _mp.insert(position, val); }
			template<class InputIterator>
			void							insert(InputIterator first, InputIterator last)								{ return _mp.insert(first,last); }
			template<class... Args>			
			pair<iterator,bool>				emplace(Args&&... args)														{ return _mp.emplace(args); }

			// swapping
			void							swap(ContType& other)														{ return _mp.swap(other); }
			void							swap(ThisType& other)														{ return swap(other._mp); }

			// clearing
			void							clear()																		{ _mp.clear(); }

			// accesss
			const mapped_type&				operator[](const key_type& k)								const			{ return _mp[k]; }
			mapped_type&					operator[](const key_tpye& k)												{ return _mp[k]; }
			const mapped_type&				operator[](const key_type_const& k)							const			{ return _mp[k]; }
			mapped_type&					operator[](const key_type_const& k)											{ return _mp[k]; }
			const mapped_type&				at(const key_type& k)														{ return _mp.at(k); }
			mapped_type&					at(const key_type& k)														{ return _mp.at(k); }
			const mapped_type&				at(const key_type_const& k)													{ return _mp.at(k); }
			mapped_type&					at(const key_type_const& k)													{ return _mp.at(k); }
						
			// erase
			void							erase(iterator position)													{ _mp.erase(iterator); }
			size_type                       erase(const key_type& k)													{ _mp.erase(k); }
			void							erase(iterator first, iterator last)										{ _mp.erase(first, last); }

			// search
			const_iterator					find(const key_type& k)										const			{ return _mp.find(k); }
			iterator						find(const key_type& k)														{ return _mp.find(k); }

			// count values with specified key
			size_type						count(const key_type& k)									const			{ return _mp.count(k); }

			// bounds
			const_iterator					lower_bound(const key_type& k)								const			{ return _mp.lower_bound(k); }
			iterator						lower_bound(const key_type& k)												{ return _mp.lower_bound(k); }			
			const_iterator					upper_bound(const key_type& k)								const			{ return _mp.upper_bound(k); }
			iterator						upper_bound(const key_type& k)												{ return _mp.upper_bound(k); }
			std::pair<const_iterator,const_iterator> equal_range(const key_type& k)						const			{ return _mp.equal(k); }
			std::pair<iterator, iterator>			 equal_range(const key_type& k)										{ return _mp.equal(k); }

			// get allocator
			allocator_type					get_allocator()												const			{ return _mp.get_allocator(); }

		private:
			ContType _mp;
		};
	} // Utils
} // Dv

#endif // _KeyMap_h_