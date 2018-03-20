#include "EditableModel.h"

namespace Dv
{
	namespace EditableModel
	{
		void LinkEdgeStartToVertex(const ModelEdgeHandle& edge)
		{
			LinkVertexToEdge(edge.GetStartModelVertexHandle(), edge);
		}

		void LinkVertexToEdge(const ModelVertexHandle& vertex, const ModelEdgeHandle& edge)
		{
			LinkVertexToEdge_WithoutLinkingEdges(vertex, edge);
			MakeEdgeToEdgeLink_ByLinkedVerts(edge);
		}

		void LinkVertexToEdge_WithoutLinkingEdges(const ModelVertexHandle& vertex, const ModelEdgeHandle& edge)
		{
			auto &outEdges = vertex.GetOutEdges();
			size_t outEdgesSize = outEdges.size();
			auto edgeIt = std::find(outEdges.begin(), outEdges.end(), edge);
			if (edgeIt == outEdges.end())
			{
				outEdges.push_back(edge);
			}
		}

		void MoveVertexLinks(const ModelVertexHandle& srcVertex, const ModelVertexHandle& destinationVertex)
		{
			auto &destinationOutEdges = destinationVertex.GetOutEdges();
			auto &srcOutEdges = srcVertex.GetOutEdges();
			destinationOutEdges.splice(destinationOutEdges.end(), srcOutEdges, srcOutEdges.cbegin(), srcOutEdges.cend());
		}

		void WeldVertexAndMoveVertexLinks(const ModelVertexHandle& srcVertex, const ModelVertexHandle& destinationVertex)
		{
			Model *pModel = srcVertex.GetModel();
			auto &srcOutEdges = srcVertex.GetOutEdges();
			for(auto &srcEdge : srcOutEdges)
			{
				srcEdge.GetVertexIterator()->vertexIterator = destinationVertex.GetPerVertexDataIterator();
			}			
			MoveVertexLinks(srcVertex,destinationVertex);
			//At this point:
			//assert(IsOrphan(srcVertex));
			pModel->RemoveVertex(srcVertex);
		}
	} // EditableModel
} // Dv