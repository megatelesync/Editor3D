#ifndef _EditableModel_TestSplittedEdgePolyGenerator_h_
#define _EditableModel_TestSplittedEdgePolyGenerator_h_

#include "../../../EditableModel_GeneratorBase.h"

namespace Dv
{
	namespace EditableModel
	{
		namespace Generator
		{
			namespace Test
			{
				template<class EdgeIterator>
				class TestSplittedEdgePolyGenerator : public GeneratorBase
				{
				public:
					TestSplittedEdgePolyGenerator
					(
						ModelPolygonHandle polyIn,
						EdgeIterator firstEdgeItIn, EdgeIterator lastEdgeItIn
					) : _poly(polyIn), _firstEdge(firstEdgeItIn), _lastEdge(lastEdgeItIn) {}

					void Generate() override;			

				protected:
					ModelPolygonHandle _poly;
					EdgeIterator _firstEdge, _lastEdge;
				};

				// Impl
				template<class EdgeIterator> 
				void TestSplittedEdgePolyGenerator<EdgeIterator>::Generate()
				{
					SplitPolyEdges(_firstEdge, _lastEdge);
				}
			} // Test
		} // Generator
	} // EditableModel
} // Dv

#endif // _EditableModel_TestSplittedEdgePolyGenerator_h_