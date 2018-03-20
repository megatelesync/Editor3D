#ifndef _ModelVertexHandleBase_h_
#define _ModelVertexHandleBase_h_

#include "EditableModel_structs.h"
#include "ModelVertexConstHandle_fwd.h"
#include "EditableModel_Model_types_fwd.h"
#include "EditableModel_handle_fwd.h"

namespace Dv
{
	namespace EditableModel
	{
		template <class ModelTypeArg, class VertexIteratorTypeArg>
		class ModelVertexHandleBase
		{
		public:
			typedef ModelTypeArg ModelType;
			typedef VertexIteratorTypeArg VertexIteratorType;

			inline ModelVertexHandleBase() : _pModel(nullptr), _perVertexDataIt(), _hash(0)								{}
			inline ModelVertexHandleBase(ModelTypeArg *pModelIn, const PerVertexDataIt& perVertexDataItIn);
			inline operator ModelVertexConstHandle()															{ return ModelVertexConstHandle(_pModel, _perVertexDataIt); }

			size_t												hash()							 const					{ return _hash; }
			ModelType										   *GetModel()						 const					{ return _pModel; }		
			PerVertexDataIt										GetPerVertexDataIterator()		 const					{ return _perVertexDataIt; }		
			typename VertexIteratorType::reference&				GetVertexInfo()					 const					{ return _perVertexDataIt->info;  }
			std::list<ModelEdgeHandle>						   &GetOutEdges()					 const					{ return _perVertexDataIt->outEdges; }					
			bool												IsNull()						 const					{ return _pModel == nullptr; }

		protected:
			ModelType *_pModel;
			PerVertexDataIt  _perVertexDataIt;
			size_t _hash;
		};	

		// fwd decl
		inline bool operator==										(const ModelVertexConstHandle& first, const ModelVertexConstHandle& second);
		inline bool operator!=										(const ModelVertexConstHandle& first, const ModelVertexConstHandle& second);
		// Check whether the vertex is linked with edge
		inline bool IsLinked										(const ModelVertexConstHandle& vert, const ModelEdgeConstHandle& edge);
		// count of output poly edges
		inline int  NumOutEdges										(const ModelVertexConstHandle& vert);
		// "orphan" means "contains zero output poly edges"
		inline bool IsOrhpan										(const ModelVertexConstHandle& vert);
		inline bool IsSingleLinked									(const ModelVertexConstHandle& vert)					{ return NumOutEdges(vert) == 1; }
		inline void UnlinkEdgeStartFromVertex						(const ModelEdgeHandle& edge);
		inline bool CheckEdgeVertexLink								(const ModelEdgeConstHandle& edge);
		void LinkEdgeStartToVertex									(const ModelEdgeHandle& edge);
		// Links a given edge and vertex (if they have not been linked yet)
		// If the edge coincides with other edge and there's a link from one of edge's vertices
		// to that coinciding edge, than it will make a link between this two edges.
		void							LinkVertexToEdge										(const ModelVertexHandle& vertex, const ModelEdgeHandle& edge);
		void							LinkVertexToEdge_WithoutLinkingEdges					(const ModelVertexHandle& vertex, const ModelEdgeHandle& edge);
		// remove all polys linked with a given vertex
		inline void RemoveLinkedPolys								(const ModelVertexHandle& vert);

		// Impl
		inline bool operator==(const ModelVertexConstHandle& first, const ModelVertexConstHandle& second)
		{
			return first.GetPerVertexDataIterator() == second.GetPerVertexDataIterator();
		}
		inline bool operator!=(const ModelVertexConstHandle& first, const ModelVertexConstHandle& second)
		{
			return !(first == second);
		}

		template<class ModelTypeArg, class VertexIteratorTypeArg>
		ModelVertexHandleBase<ModelTypeArg,VertexIteratorTypeArg>::ModelVertexHandleBase(ModelTypeArg *pModelIn, const PerVertexDataIt& perVertexDataItIn) :
			_pModel(pModelIn), _perVertexDataIt(perVertexDataItIn)
		{
			_hash = reinterpret_cast<size_t>(&(*_perVertexDataIt));
		}

		inline int NumOutEdges(const ModelVertexConstHandle& vert)
		{
			return vert.GetOutEdges().size();
		}

		inline bool IsOrhpan(const ModelVertexConstHandle& vert)
		{
			return NumOutEdges(vert) == 0;
		}

		inline bool CheckEdgeVertexLink (const ModelEdgeConstHandle& edge)
		{
			return IsLinked(edge.GetStartModelVertexHandle(), edge);
		}

		inline bool IsLinked (const ModelVertexConstHandle& vert, const ModelEdgeConstHandle& edge)
		{
			const Model *pModel = edge.GetModel();
			auto outEdges = vert.GetOutEdges();
			return std::find(outEdges.cbegin(), outEdges.cend(), edge) != outEdges.cend();
		}
	} // EditableModel
} // Dv

#endif // _ModelVertexHandleBase_h_