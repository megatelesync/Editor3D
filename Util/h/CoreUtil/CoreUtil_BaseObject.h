#pragma once

#include <string>

namespace Dv
{
	namespace Util
	{
		class BaseObject
		{
		public:
			/****************************************
			* ctor
			****************************************/
			// default ctor: 
			// create object with default name
			inline BaseObject();
			inline BaseObject(const std::string& InName);
			
	
			/****************************************
			* getters
			****************************************/
			const std::string& GetName() const { return _name; }

		private:
			std::string _name;			
		};





		/****************************************
		* IMPLEMENTATION
		****************************************/
		inline BaseObject::BaseObject() :
			_name("<UNNAMED>")
		{
		}

		inline BaseObject::BaseObject(const std::string& InName) :
			_name(InName)
		{
		}
	} // Util
} // Dv