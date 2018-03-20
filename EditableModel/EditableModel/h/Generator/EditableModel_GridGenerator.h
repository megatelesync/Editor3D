#include "../../EditableModel_GeneratorBase.h"

namespace Dv
{
	namespace EditableModel
	{
		namespace Generator
		{
			class GridGenerator : public GeneratorBase
			{
			public:
				GridGenerator();
				virtual ~GridGenerator() {}
				virtual void Generate() override;
			}; 
		} // Generator 
	} // EditableModel
} // Dv