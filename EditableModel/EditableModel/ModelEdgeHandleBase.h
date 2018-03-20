#ifndef _ModelEdgeHandleBase_h_
#define _ModelEdgeHandleBase_h_

#include <cstddef>
#include "EditableModel_structs.h"
#include "ModelEdgeConstHandle_fwd.h"

namespace Dv
{
	namespace EditableModel
	{		
		template<class PolyHandleTypeArg, class EdgeIteratorTypeArg, class VertexHandleTypeArg>
		class ModelEdgeHandleBase
		{
		public:
			typedef ModelEdgeHandleBase<PolyHandleTypeArg, EdgeIteratorTypeArg, VertexHandleTypeArg>				ThisType;
			typedef PolyHandleTypeArg																				PolyHandleType;
			typedef EdgeIteratorTypeArg																				EdgeIteratorType; 
			typedef VertexHandleTypeArg																				VertexHandleType; 

			inline ModelEdgeHandleBase() : _hash(0) {}
			inline ModelEdgeHandleBase(const typename PolyHandleType::PolygonHandleBaseType & polyHandleIn, EdgeIteratorTypeArg startVertexIterIn);			
			inline operator ModelEdgeConstHandle() const { return ModelEdgeConstHandle(_polyHandle, _startVertexIterator); }

			size_t											  hash()			  const { return _hash; }
			inline typename PolyHandleType::ModelType		 *GetModel()		  const { return _polyHandle.GetModel(); }
			inline const PolyHandleType&					  GetPolygon()		  const { return _polyHandle; }		
			inline EdgeIteratorType							  GetVertexIterator() const { return _startVertexIterator; }

			inline VertexHandleType							  GetStartModelVertexHandle() const;
			inline VertexHandleType						      GetEndModelVertexHandle() const;
			inline ThisType									  GetNextEdge() const;
			inline ThisType									  GetPreviousEdge() const;

			inline const PolygonVertexInfo&					  GetStartVertexInfo() const;
			inline const PolygonVertexInfo&					  GetEndVertexInfo() const;

			inline bool										  IsNull() const { return _polyHandle.IsNull(); }

		protected:
			PolyHandleTypeArg _polyHandle;
			EdgeIteratorTypeArg _startVertexIterator;
			size_t _hash;
		};
	} // EditableModel
} // Dv

#endif // _ModelEdgeHandleBase_h_