#ifndef _ModelPolygonHandleBase_h_
#define _ModelPolygonHandleBase_h_

#include <DirectXMath.h>
#include <list>
#include "EditableModel_structs.h"
#include "ModelPolygonConstHandle_fwd.h"

using namespace DirectX;

namespace Dv
{
	namespace EditableModel
	{	
		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg>
		class ModelPolygonHandleBase
		{
		public:
			typedef ModelTypeArg ModelType;
			typedef PolyIteratorTypeArg PolyIteratorType;
			typedef EdgeTypeArg EdgeType;
			typedef ModelPolygonHandleBase<ModelType, PolyIteratorType, EdgeType> PolygonHandleBaseType;

			// create default model handle
			inline ModelPolygonHandleBase() : _pModel(nullptr), _polygonIterator(), _hash(0) {}
			inline ModelPolygonHandleBase(ModelTypeArg *pModelIn, const PolyIteratorTypeArg& polygonIteratorIn);
			inline operator ModelPolygonConstHandle() const;

			size_t											 hash()						const { return _hash; }
			ModelTypeArg									*GetModel()					const { return _pModel; }
			PolyIteratorTypeArg								 GetPolygonIterator()		const { return _polygonIterator; }

			inline int										 GetVertexCount()			const;
			const ModelPolygonInfo&							 GetPolyInfo()				const { return *_polygonIterator; }
			inline EdgeTypeArg								 GetFirstEdge()				const;
			inline EdgeTypeArg								 GetLastEdge()				const;	
			inline bool										 IsNull()					const { return _pModel == nullptr; }

		protected:
			ModelTypeArg *_pModel;
			PolyIteratorTypeArg _polygonIterator;
			size_t _hash;
		};

		// ModelPolygonConstHandle impl
		class ModelPolygonConstHandle : public ModelPolygonHandleBase<const Model, PolygonConstIt, ModelEdgeConstHandle> 
		{
		public:
			inline ModelPolygonConstHandle() : ModelPolygonHandleBase() {}
			inline ModelPolygonConstHandle(const Model *pModelIn, const PolygonConstIt& polygonIteratorIn) : 
				ModelPolygonHandleBase(pModelIn, polygonIteratorIn) {}
		};		

		// ModelPolygonHandleBase impl
		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg>
		ModelPolygonHandleBase<ModelTypeArg, PolyIteratorTypeArg, EdgeTypeArg>::operator ModelPolygonConstHandle() const { return ModelPolygonConstHandle(_pModel, _polygonIterator); }

		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg>
		ModelPolygonHandleBase<ModelTypeArg, PolyIteratorTypeArg, EdgeTypeArg>::ModelPolygonHandleBase(ModelTypeArg *pModelIn, const PolyIteratorTypeArg& polygonIteratorIn) :
			_pModel(pModelIn), _polygonIterator(polygonIteratorIn)
		{
			_hash = reinterpret_cast<size_t>(&(*_polygonIterator));
		}

		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg>
		int ModelPolygonHandleBase<ModelTypeArg, PolyIteratorTypeArg, EdgeTypeArg>::GetVertexCount() const
		{
			return GetPolyInfo().vertices.size();
		}

		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg>
		EdgeTypeArg ModelPolygonHandleBase<ModelTypeArg, PolyIteratorTypeArg, EdgeTypeArg>::GetFirstEdge() const
		{
			auto polyVertexIt = _polygonIterator->vertices.begin();
			return EdgeTypeArg(*this, polyVertexIt);
		}

		template<class ModelTypeArg, class PolyIteratorTypeArg, class EdgeTypeArg>
		EdgeTypeArg ModelPolygonHandleBase<ModelTypeArg, PolyIteratorTypeArg, EdgeTypeArg>::GetLastEdge() const
		{
			auto polyVertexIt = std::prev(_polygonIterator->vertices.end());
			return EdgeTypeArg(*this, polyVertexIt);
		}

		inline bool operator==(const ModelPolygonConstHandle& first, const ModelPolygonConstHandle& second)
		{
				return first.GetPolygonIterator() == second.GetPolygonIterator();
		}
		inline bool operator!=(const ModelPolygonConstHandle& first, const ModelPolygonConstHandle& second)
		{
			return !(first == second);
		}
	} // EditableModel
} // Dv

#endif // _ModelPolygonHandleBase_h_