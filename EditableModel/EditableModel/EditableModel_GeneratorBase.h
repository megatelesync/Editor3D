#ifndef _EditableModel_GeneratorBase_h_
#define _EditableModel_GeneratorBase_h_

#include "EditableModel.h"

namespace Dv
{
	namespace EditableModel
	{
		namespace Generator
		{
			class GeneratorBase
			{
			public:
				const Model&							GetModel()				const			{ return _model; }
				Model&									GetModel()								{ return _model; }
				virtual void							Generate()				=0;
				virtual									~GeneratorBase() {}

			private:
				Model _model;
			};
		} // Generator
	} // EditableModel
} // Dv

#endif // _EditableModel_GeneratorBase_h_