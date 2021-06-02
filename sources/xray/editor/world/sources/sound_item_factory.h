////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_ITEM_FACTORY_H_INCLUDED
#define SOUND_ITEM_FACTORY_H_INCLUDED

#include "sound_item_struct.h"

using namespace System;

typedef xray::editor::sound_item_struct::sound_item_properties sound_item_properties;
typedef xray::editor::sound_item_struct::sound_item_dependencies sound_item_dependencies;

namespace xray
{
	namespace editor
	{
		ref class sound_file_item_property;
		ref class sound_file_item_dependency;
		ref class sound_file_item;

		namespace controls
		{
			ref class dependency_editor;
		}

		#pragma region |  Structs  | 

		public ref struct sound_item_property_struct
		{
		public:
			sound_item_property_struct(sound_item_properties set_property_type, Type^ set_type):
			  property_type(set_property_type), type(set_type){}

		public:
			sound_item_properties		property_type;
			Type^						type;

		public:
			virtual String^ ToString() override
			{
				return property_type.ToString()->Replace("_", " ")+" •";
			}
		};

		public ref struct sound_item_dependency_struct
		{
		public:
			sound_item_dependency_struct(sound_item_dependencies set_dependency, Type^ set_type):
			  dependency_type(set_dependency), type(set_type){}

		public:
			sound_item_dependencies		dependency_type;
			Type^						type;

		public:
			virtual String^ ToString() override
			{
				return dependency_type.ToString()->Replace("_", " ")+" •";
			}
		};

		#pragma endregion

		ref class sound_item_factory
		{
		private:
			static sound_item_factory									();

		private:
			static Collections::Generic::List<sound_item_property_struct^>^					properties;
			static Collections::Generic::List<sound_item_dependency_struct^>^				dependencies;
			static Collections::Generic::Dictionary<Collections::Generic::KeyValuePair<Type^, Type^>, Type^>^		editors;

		public:
			static Collections::Generic::List<sound_item_property_struct^>^					GetProperties			();
			static Collections::Generic::List<sound_item_dependency_struct^>^				GetDependencies			();
			static sound_file_item_property^							CreateProperty			(sound_file_item^ parent);
			static sound_file_item_dependency^							CreateDependency		(sound_file_item_property^ parent);
			static controls::dependency_editor^							CreateDependencyEditor	(Type^ dependency, Type^ property);

		}; // class sound_item_factory
	}//namespace editor
}//namespace xray
#endif // #ifndef SOUND_ITEM_FACTORY_H_INCLUDED