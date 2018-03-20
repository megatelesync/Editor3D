#pragma once

namespace Dv
{
	namespace Util
	{
		class IdGenerator
		{
		public:
			size_t                 GenerateID                    () { return _nextId++; }

		private:
			size_t                 _nextId = 0;
		};
	}
}