////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_item_factory.h"
#include "sound_file_item_property.h"
#include "sound_file_item_dependency.h"
#include "dependency_editor_bool_bool.h"
#include "dependency_editor_float_bool.h"
#include "dependency_editor_bool_float.h"
#include "dependency_editor_float_float.h"

using namespace System::Collections::Generic;
using namespace xray::editor::controls;

namespace xray
{
	namespace editor
	{
		static											sound_item_factory::sound_item_factory		()
		{
			properties			= gcnew List<sound_item_property_struct^>();
			dependencies		= gcnew List<sound_item_dependency_struct^>();
			editors				= gcnew Dictionary<KeyValuePair<Type^, Type^>, Type^>();

			//register properties	// Must be the same order as in sound_item_properties enum
			properties->Add		(gcnew sound_item_property_struct	(sound_item_properties::precondition,	Boolean::typeid));
			properties->Add		(gcnew sound_item_property_struct	(sound_item_properties::volume,			Single::typeid));

			//register dependencies		// Must be the same order as in sound_item_dependencies enum
			dependencies->Add	(gcnew sound_item_dependency_struct	(sound_item_dependencies::day_time,		Single::typeid));
			dependencies->Add	(gcnew sound_item_dependency_struct	(sound_item_dependencies::distance,		Single::typeid));
			dependencies->Add	(gcnew sound_item_dependency_struct	(sound_item_dependencies::self,			Boolean::typeid));

			//register editors for type pairs
			
			editors->Add		(KeyValuePair<Type^, Type^>(Boolean::typeid, Boolean::typeid),	dependency_editor_bool_bool::typeid);
			editors->Add		(KeyValuePair<Type^, Type^>(Single::typeid, Boolean::typeid),	dependency_editor_float_bool::typeid);
			editors->Add		(KeyValuePair<Type^, Type^>(Boolean::typeid, Single::typeid),	dependency_editor_bool_float::typeid);
			editors->Add		(KeyValuePair<Type^, Type^>(Single::typeid, Single::typeid),	dependency_editor_float_float::typeid);
		}

		List<sound_item_property_struct^>^				sound_item_factory::GetProperties			()
		{
			return sound_item_factory::properties;
		}

		List<sound_item_dependency_struct^>^			sound_item_factory::GetDependencies			()
		{
			return sound_item_factory::dependencies;
		}

		sound_file_item_property^						sound_item_factory::CreateProperty			(sound_file_item^ parent)
		{
			sound_file_item_property^ item_property = gcnew sound_file_item_property();
			item_property->parent_sound = parent;

			for each(sound_item_property_struct^ prop in properties)
			{
				item_property->combobox_items->Add(prop);
			}

			return item_property;
		}
		
		sound_file_item_dependency^						sound_item_factory::CreateDependency		(sound_file_item_property^ parent)
		{
			sound_file_item_dependency^ item_dependency = gcnew xray::editor::sound_file_item_dependency();
			item_dependency->parent_property = parent;

			for each(sound_item_dependency_struct^ dep in dependencies)
			{
				item_dependency->combobox_items->Add(dep);
			}

			return item_dependency;
		}

		controls::dependency_editor^					sound_item_factory::CreateDependencyEditor	(Type^ dependency, Type^ property)
		{
		
				return  safe_cast<controls::dependency_editor^>( Activator::CreateInstance(editors[KeyValuePair<Type^, Type^>(dependency, property)]));
		
			
		}
	}//namespace editor
}//namespace xray