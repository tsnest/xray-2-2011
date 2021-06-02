////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEPENDENCY_EDITOR_TYPE_H_INCLUDED
#define DEPENDENCY_EDITOR_TYPE_H_INCLUDED

namespace xray
{
	namespace editor
	{
		typedef System::Collections::Generic::List<float>	list_of_float;

		namespace controls
		{
			enum class dependency_editor_type
			{
				bool_to_bool,
				bool_to_float,
				float_to_bool,
				flot_to_float
			}; // class dependency_editor_type
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DEPENDENCY_EDITOR_TYPE_H_INCLUDED