#include "EditableModel_GeneratorBase.h"

namespace Dv
{
	namespace EditableModel
	{
		namespace Generator
		{
			class PrismGenerator : public GeneratorBase
			{
			public:
				PrismGenerator();
				virtual ~PrismGenerator() {}
				virtual void Generate() override;
			};
		}
	} // EditableModel
} // Dv