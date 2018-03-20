#ifndef _Util_Container_Interface_Master_h_
#define _Util_Container_Interface_Master_h_

namespace Dv
{
	namespace Container
	{
		// Untyped containers
		class IIndexedWriteOnlyBase
		{
		public:
			virtual ~IIndexedWriteOnlyBase();
	
			virtual UINT GetElementSize() const =0;

			virtual size_t			GetNumElems()		  const =0;
		};

		// typed containers
		template class <ElemTypeArg> ITypedIndexedWriteOnly : 
			public IIndexedWriteOnlyBase
		{
		public:
			typedef      ElemTypeArg 	ElemType;

			virtual ~ITypedIndexedWriteOnly();

			virtual const ElemTypeArg&	operator[] (int index)	  const =0;
		};

		template class <ElemTypeArg> ITypedIndexedExpandableWriteOnly :
			public ITypedIndexedWriteOnly<ElemTypeArg>
		{
		public:
			virtual ~ITypedIndexedExpandableWriteOnly();

			virtual void Append(const ElemTypeArg& newElem) =0;
		};

		// Untyped containers
		class IIndexedWriteOnly :
			public IIndexedWriteOnlyBase
		{
		public:
			virtual ~IIndexedWriteOnly();
	
			virtual UINT GetElementSize() const =0;

			virtual void  *operator[] (int index)	  const =0;
		};

		class IIndexedExpandableWriteOnly : public IIndexedWriteOnly
		{
		public:
			virtual ~IIndexedExpandableWriteOnly();

			virtual void Append(void *pNewElem, size_t numBytes) =0;
		};
	} // Container
} // Dv

#endif // _Util_Container_Interface_Master_h_