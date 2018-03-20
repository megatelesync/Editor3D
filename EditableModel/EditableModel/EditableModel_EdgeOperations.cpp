#ifndef _EditableModel_EdgeOperations_h_
#define _EditableModel_EdgeOperations_h_

#include <algorithm>
#include "EditableModel.h"

namespace Dv
{
	namespace EditableModel
	{
		EdgeMap GetEdgeMap(const EdgeHashMap& edgeHashMap)
		{
			EdgeMap res;
			for(auto it = edgeHashMap.cbegin(); it != edgeHashMap.cend(); ++it)
			{
				res.insert(std::make_pair(it->second.adjEdge, it->second.adj));
			}
			return res;
		}

		void WeldEdgeVertices (const ModelEdgeHandle& edge, bool bRemoveStartVertex)
		{
			/*
			auto adjacencyPair = FirstAdjacentEdge(edge);
			ModelVertexHandle vertexToRemove = bRemoveStartVertex ? edge.GetStartModelVertexHandle() : edge.GetEndModelVertexHandle();
			ModelVertexHandle vertexToWeldInto = bRemoveStartVertex ? edge.GetEndModelVertexHandle() : edge.GetStartModelVertexHandle();

			// Weld vertices of the edge
			WeldPolyEdgeVertices(edge, bRemoveStartVertex);

			// If there's adjacent edge
			if(adjacencyPair.first != edge)
			{
				auto adjEdge = adjacencyPair.first;
				bool bOtherEdge_RemoveStartVertex = bRemoveStartVertex;
				if(adjacencyPair.second.bCounterDirected) 
				{
					bOtherEdge_RemoveStartVertex = !bRemoveStartVertex;
				}

				// Weld vertices of the adjacent edge
				WeldPolyEdgeVertices(adjEdge, bOtherEdge_RemoveStartVertex);
			}

			WeldVertexAndMoveVertexLinks(vertexToRemove, vertexToWeldInto);
			*/
		}

		/*
		void WeldEdgeVertices (const ModelEdgeHandle& edge)
		{
			Model					*pModel = edge.GetModel();
			ModelPolygonHandle		 poly   = edge.GetPolygon();
			ModelVertexHandle		 edgeStartVertex = edge.GetStartModelVertexHandle();
			ModelVertexHandle		 edgeEndVertex = edge.GetEndModelVertexHandle();

			auto					 adjEdgeInfo = FirstAdjacentEdge(edge);

			// Move links from edgeToRemove

			// Remove model edge edgeToRemove and all of its edge links and vertex links

			// Replace the edgeEndVertex by edgeStartVertex

			// Delete the edgeEndVertex (as it must be orphan vertex at this point)

			// Fix poly if it has only 2 edges
			if(poly.GetVertexCount() == 2)
			{
				FixLinks_OfTwoVertexPoly(poly);
				pModel->RemovePoly(poly);
			}
			// If we have adjacent edge:
			// Fix poly of the adjacent edge if the poly is two vertex
			if(adjEdgeInfo.first != edge)
			{
				ModelPolygonHandle adjPoly = adjEdgeInfo.first.GetPolygon();
				if(adjPoly.GetVertexCount() == 2)
				{
					FixLinks_OfTwoVertexPoly(adjPoly);
					pModel->RemovePoly(adjPoly);
				}
			}
		}
		*/

		void WeldPolyEdgeVertices(const ModelEdgeHandle& edge, bool bRemoveStartVertex)
		{
			ModelEdgeHandle edgeToChange = bRemoveStartVertex ? edge.GetPreviousEdge() : edge.GetNextEdge();
			ModelEdgeHandle edgeToRemove = bRemoveStartVertex ? edge : edge.GetNextEdge();
			ModelPolygonHandle poly = edge.GetPolygon();
			Model *pModel = poly.GetModel();

			// Remove edge and links to it (both links to vertex and to edge)
			RemoveEdgeAndLinks(edgeToRemove);

			// Fix poly if it now has only two vertices	
			if(poly.GetVertexCount() == 2)
			{
				FixLinks_OfTwoVertexPoly(poly);
				pModel->RemovePoly(poly);
			}
		}

		/*
		void WeldEdgeVertices (const ModelEdgeHandle& edge)
		{
			Model					*pModel = edge.GetModel();
			ModelPolygonHandle		 poly   = edge.GetPolygon();
			auto					 adjEdgeInfo = FirstAdjacentEdge(edge);
			
			if(adjEdgeInfo.first != edge)
			{
				// If we have adjacent edge
				ModelPolygonHandle adjPoly = adjEdgeInfo.first.GetPolygon();
				RemoveModelEdge_FromBothAdjacentPolys(edge);			
				pModel->UnlinkEdge(edge);
			}
			else
			{
				// If we have no adjacent edge
				poly.RemoveEdge(edge);
			}


		}
		*/

		void RemoveModelEdge_FromBothAdjacentPolys(const ModelEdgeHandle& edge)
		{
			/*
			ModelPolygonHandle poly = edge.GetPolygon();			
			ModelEdgeHandle adjEdge = GetCoincideVertexEdge_ForStartVertexOfEdge(edge);
			poly.RemoveEdge(edge);
			// if we have and adjacent edge (and hence, poly)
			if(adjEdge != edge)
			{
				ModelPolygonHandle adjPoly = adjEdge.GetPolygon();
				adjPoly.RemoveEdge(adjEdge);
			}
			*/
		}

		ModelEdgeConstHandle GetCoincideVertexEdge_ForStartVertexOfEdge(const ModelEdgeConstHandle& edge)
		{
			auto adjEdgeInfo = FirstAdjacentEdge(edge);
			auto adjEdge = adjEdgeInfo.first;
			bool bAdjacentEdgeCounterDirected = adjEdgeInfo.second.bCounterDirected;
			if(adjEdge != edge && bAdjacentEdgeCounterDirected)
			{
				return adjEdge.GetNextEdge();
			}
			else
			{
				return adjEdge;
			}
		}

		std::pair<ModelEdgeConstHandle,EdgeAdjacency> FirstAdjacentEdge(const ModelEdgeConstHandle& edge)
		{
			const Model *pModel = edge.GetModel();
			std::pair<ModelEdgeHandle,EdgeAdjacency> res;			
			auto adjEdges = pModel->GetAdjEdgeHashMap(edge);
			if(adjEdges.size() > 0)
			{
				auto firstAdjEdgeIt = adjEdges.begin();
				auto firstAdjEdge = std::make_pair(firstAdjEdgeIt->second.adjEdge, firstAdjEdgeIt->second.adj);
				return firstAdjEdge;
			}
			else
			{
				return std::make_pair(edge, EdgeAdjacency());
			}
		}

		/*
		std::pair<ModelEdgeHandle,EdgeAdjacency> FirstAdjacentEdge(const ModelEdgeHandle& edge)
		{
			const Model *pModel = edge.GetModel();
			std::pair<ModelEdgeHandle,EdgeAdjacency> res;			
			auto adjEdges = pModel->GetAdjEdgeHashMap(edge);
			if(adjEdges.size() > 0)
			{
				auto firstAdjEdgeIt = adjEdges.begin();
				auto firstAdjEdge = *firstAdjEdgeIt;
				return firstAdjEdge;
			}
			else
			{
				return std::make_pair(edge, EdgeAdjacency());
			}
		}
		*/

		bool LinkEdges_IfOverlapped(const Vector& overlapDirection, const ModelEdgeHandle& edge, const ModelEdgeHandle& otherEdge)
		{
			Model *pModel = edge.GetModel();
			bool bCounterDirected = AreCounterDirected(edge, otherEdge);					
			Vector otherStartPosition = StartPosition(otherEdge);
			Vector otherEndPosition = EndPosition(otherEdge);
			if(bCounterDirected)
			{
				std::swap(otherStartPosition, otherEndPosition);
			}
			bool bOverlapped = AreOverlappedCodirected(overlapDirection, StartPosition(edge), EndPosition(edge), otherStartPosition, otherEndPosition);
			if(bOverlapped)
			{
				EdgeAdjacency adjInfo = EdgeAdjacency(bCounterDirected);
				pModel->Link(edge, otherEdge, adjInfo);
			}
			return bOverlapped;
		}

		void RemoveEdgeAndLinks(const ModelEdgeHandle& edgeToRemove)
		{
			ModelPolygonHandle poly = edgeToRemove.GetPolygon();
			Model *pModel = poly.GetModel();
			UnlinkEdgeStartFromVertex(edgeToRemove);
			pModel->UnlinkEdge(edgeToRemove);
			poly.RemoveEdge(edgeToRemove);
		}
		
		void ReplaceEdge_InEdgeToVertexLinks(const ModelEdgeHandle& oldEdge, const ModelEdgeHandle& newEdge)
		{
			ReplaceEdge_InEdgeToVertexLinks(oldEdge.GetStartModelVertexHandle(), oldEdge, newEdge);
		}

		void ReplaceEdge_InEdgeToVertexLinks(const ModelVertexHandle& vertexHandle, const ModelEdgeHandle& oldEdge, const ModelEdgeHandle& newEdge)
		{
			auto &outEdges = vertexHandle.GetOutEdges();
			std::replace(outEdges.begin(), outEdges.end(), oldEdge, newEdge);
		}

		void ReplaceEdge_InEdgeToEdgeLinks(const ModelEdgeHandle& oldEdge, const ModelEdgeConstHandle& newEdge)
		{
			Model *pModel = oldEdge.GetModel();
			auto &linkedEdges = pModel->GetAdjEdgeHashMap(oldEdge);
			for(auto oldEdgeLink : linkedEdges)
			{
				ModelEdgeHandle edgeLinkedToOldEdge = oldEdgeLink.second.adjEdge;
				pModel->Link(edgeLinkedToOldEdge, newEdge, oldEdgeLink.second.adj);
			}
			pModel->UnlinkEdge(oldEdge);
		}

		ModelEdgeHandle FindCodirectedCoincideEdge_ByLinkedStartVert(const ModelEdgeConstHandle& edge)
		{
			ModelVertexConstHandle endVertex = edge.GetEndModelVertexHandle();
			const auto &outEdges = edge.GetStartModelVertexHandle().GetOutEdges();			
			for(auto linkedEdge : outEdges)
			{
				if(linkedEdge != edge)
				{
					if(linkedEdge.GetEndModelVertexHandle() == endVertex)
					{
						return linkedEdge;
					}
				}
			}
			return ModelEdgeHandle();
		}

		ModelEdgeHandle FindCounterDirectedCoincideEdge_ByLinkedEndVert(const ModelEdgeConstHandle& edge)
		{
			ModelVertexConstHandle endVertex = edge.GetEndModelVertexHandle();
			ModelVertexConstHandle startVertex = edge.GetStartModelVertexHandle();
			const auto &outEdges = edge.GetEndModelVertexHandle().GetOutEdges();	
			for(auto linkedEdge : outEdges)
			{
				if(linkedEdge != edge)
				{
					if(linkedEdge.GetEndModelVertexHandle() == startVertex)
					{
						return linkedEdge;
					}
				}
			}
			return ModelEdgeHandle();
		}

		ModelEdgeHandle FindCoincideEdge_ByLinkedVerts(const ModelEdgeConstHandle& edge)
		{
			ModelEdgeHandle codirectedEdge = FindCodirectedCoincideEdge_ByLinkedStartVert(edge);
			if(!codirectedEdge.IsNull()) { return codirectedEdge; }
			ModelEdgeHandle counterDirectedEdge = FindCounterDirectedCoincideEdge_ByLinkedEndVert(edge);
			return counterDirectedEdge;
		}

		void MakeEdgeToEdgeLink_ByLinkedVerts (const ModelEdgeHandle& edge)
		{
			Model *pModel = edge.GetModel();
			ModelEdgeHandle coincideEdge = FindCoincideEdge_ByLinkedVerts(edge);
			if(!coincideEdge.IsNull())
			{
				int coincideDir = AreCoincide(edge, coincideEdge);
				bool bCounterDirected = coincideDir < 0;
				EdgeAdjacency adjInfo(bCounterDirected);
				pModel->Link(edge, coincideEdge, adjInfo);
			}
		}

		EdgeMapSequence CopyEdgeLinkSetSequence_CorrespodingPolyEdges(const ModelEdgeConstHandle& firstEdge, const ModelEdgeConstHandle& lastEdge)
		{
			EdgeMapSequence res;
			ModelEdgeConstHandle edge = firstEdge;
			while(true)
			{
				EdgeMap edgeLinkSet = GetEdgeLinkMap(edge);
				res.push_back(edgeLinkSet);				
				edge = edge.GetNextEdge();
				if(edge == lastEdge) { break; }								
			}
			return res;
		}		

		void LinkPolyEdges 
		(
			const ModelEdgeHandle& startEdge, const ModelEdgeConstHandle& lastEdge, 
			const EdgeMapSequence& linkMapsCorrespondingEdges
		)
		{
			int edgeIndex = 0;
			ModelEdgeHandle edge = startEdge;
			while(true)
			{
				const EdgeMap &edgesToLinkWith = linkMapsCorrespondingEdges[edgeIndex];
				LinkEdgeTo(edge, edgesToLinkWith);				
				edge = edge.GetNextEdge();
				if(edge == lastEdge)
				{
					break;
				}
				edgeIndex ++;
			}
		}

		void LinkPolyEdges 
		(
			const ModelEdgeHandle& startEdge, const ModelEdgeHandle& lastEdge, 
			EdgeMapSequenceIt firstEdgeMapIt, EdgeMapSequenceIt lastEdgeMapIt
		)
		{
			int edgeIndex = 0;
			EdgeMapSequenceIt it = firstEdgeMapIt;
			ModelEdgeHandle edge = startEdge;
			while(true)
			{
				const EdgeMap &edgesToLinkWith = *it;
				LinkEdgeTo(edge, edgesToLinkWith);
				if(edge == lastEdge)
				{
					break;
				}
				edge = edge.GetNextEdge();
				edgeIndex ++;
				++it;
			}
		}

		void LinkEdgeTo (const ModelEdgeHandle& edge, const EdgeMap& edgesToLinkWith)
		{
			Model *pModel = edge.GetModel();
			for(auto keyValue : edgesToLinkWith)
			{
				ModelEdgeConstHandle edgeToLinkWith = keyValue.first;
				EdgeAdjacency linkAdjInfo = keyValue.second;
				pModel->Link(edge, edgeToLinkWith, linkAdjInfo);
			}
		}

		ModelEdgeHandle FindAdjacentEdgeContainingVertex (const ModelEdgeConstHandle& edge, const ModelVertexConstHandle& vertexToFind)
		{
			const Model *pModel = edge.GetModel();
			auto adjEdges = pModel->GetAdjEdgeHashMap(edge);
			auto adjEdgeIt = adjEdges.cbegin();
			while(adjEdgeIt != adjEdges.cend())
			{
				ModelEdgeHandle adjEdge = adjEdgeIt->second.adjEdge;
				if(EdgeContainsVertex(adjEdge, vertexToFind))
				{
					return adjEdge;
				}
			}
			return ModelEdgeHandle();
		}

		bool EdgeContainsVertex(const ModelEdgeConstHandle& edge, const ModelVertexConstHandle& vertex)
		{
			Vector endPosition = EndPosition(edge);
			Vector startPosition = StartPosition(edge);
			Vector vertexPosition = vertex.GetVertexInfo().GetPositionVector();
			Scalar startToVertexOrder = GetVertexOrder(edge, false, startPosition, vertexPosition);
			Scalar vertexToEndOrder = GetVertexOrder(edge, false, vertexPosition, endPosition);
			if(startToVertexOrder * vertexToEndOrder > 0)
			{
				return true;
			}
			return false;
		}

		int MoveOrCopyEdgeLinkToNextEdge_IfNecessary(const Vector& direction, const ModelEdgeHandle& edge, const ModelEdgeHandle& adjEdge)
		{
			ModelEdgeHandle nextEdge = edge.GetNextEdge();
			Model *pModel = edge.GetModel();
			bool bCounterDirected = GetLinkInfo(edge, adjEdge).bCounterDirected;
			VectorPair adjEdge_startEnd_inDirectionOfEdge = GetStartEnd(adjEdge, bCounterDirected);
			Vector edgeEnd = EndPosition(edge);
			Scalar order_splitter_to_segmentStart = GetVertexOrder(direction, edgeEnd, adjEdge_startEnd_inDirectionOfEdge.first);
			Scalar order_splitter_to_segmentEnd   = GetVertexOrder(direction, edgeEnd, adjEdge_startEnd_inDirectionOfEdge.second);
			// Does segment beyond splitter?
			if(order_splitter_to_segmentEnd > 0)
			{
				CopyEdgeLink(adjEdge, edge, nextEdge);
				if(order_splitter_to_segmentStart >= 0)
				{
					pModel->Unlink(adjEdge, edge);
					return 0;
				}
				else
				{
					return 1;
				}
			}
			else
			{
				return -1;
			}
		}

		void MoveEachEdgeLink_FromEdgeToNextEdge_IfNecessary(const ModelEdgeHandle& firstEdge)
		{
			auto nextEdge = firstEdge.GetNextEdge();
			assert(nextEdge != firstEdge);
			Model *pModel = firstEdge.GetModel();
			const EdgeHashMap& adjEdges = pModel->GetAdjEdgeHashMap(firstEdge);			
			size_t numEdges = adjEdges.size();
			Vector direction = Direction(firstEdge);
			if(adjEdges.empty())
			{ 
				return; 
			}
			auto adjEdgeIt = adjEdges.cbegin();					
			auto nextAdjEdgeIt = adjEdgeIt;
			ModelEdgeHandle adjEdge;
			while(adjEdgeIt != adjEdges.cend())
			{					
				nextAdjEdgeIt = std::next(adjEdgeIt);
				adjEdge = adjEdgeIt->second.adjEdge;
				//if(nextAdjEdgeIt == adjEdges.end()) { break; }				
				//auto adjEdgeLink = adjEdgeIt->second.adj;				
				/*
				Vector adjEdge_Start = adjEdge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector();
				Vector adjEdge_End = adjEdge.GetEndModelVertexHandle().GetVertexInfo().GetPositionVector();				
				Vector nextEdge_Start = nextEdge.GetStartModelVertexHandle().GetVertexInfo().GetPositionVector();
				Vector direction = Direction(firstEdge); 
				if (adjEdgeLink.bCounterDirected) { direction = -direction; }
				if(GetVertexOrder(firstEdge, adjEdgeLink.bCounterDirected,  nextEdge_Start, adjEdge_Start) >= 0)
				{
					MoveEdgeLink(adjEdge, firstEdge, nextEdge);
				}
				*/
				MoveOrCopyEdgeLinkToNextEdge_IfNecessary(direction, firstEdge, adjEdge);
				adjEdgeIt = nextAdjEdgeIt;
			}				
		}
	} // EditableModel
} // Dv

#endif // _EditableModel_EdgeOperations_h_