#ifndef _EditableModel_TestPolyGenerator_h_
#define _EditableModel_TestPolyGenerator_h_

#include "../../../EditableModel_GeneratorBase.h"

namespace Dv
{
	namespace EditableModel
	{
		namespace Generator
		{
			namespace Test
			{				
				template<class VertDataIterator> class TestPolyGenerator : public GeneratorBase
				{
				public:
					TestPolyGenerator(VertDataIterator firstVert, VertDataIterator lastVert);

					void Generate() override;			

				protected:
					VertDataIterator _firstVert, _lastVert;
					int _materialIndex;
				};

				// Impl
				template<class VertDataIterator>
				void TestPolyGenerator<VertDataIterator>::Generate()
				{
					Model *pModel = &GetModel();
					ModelPolygonHandle poly = CreatePolygon(pModel, _materialIndex, _firstVert, _lastVert);
				}

				template<class VertDataIterator>
				TestPolyGenerator<VertDataIterator>::TestPolyGenerator(VertDataIterator firstVert, VertDataIterator lastVert) :
					_firstVert(firstVert), _lastVert(lastVert)
				{
				}
			} // Test
		} // Generator
	} // EditableModel
} // Dv

#endif // _EditableModel_TestPolyGenerator_h_