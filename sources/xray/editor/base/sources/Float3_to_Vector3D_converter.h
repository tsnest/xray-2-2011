////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FLOAT3_TO_VECTOR3D_CONVERTER_H_INCLUDED
#define FLOAT3_TO_VECTOR3D_CONVERTER_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor_base
	{
		namespace type_converters
		{

			using editor::wpf_controls::property_editors::attributes::type_converter_base; 
			using System::Windows::Media::Media3D::Vector3D;

			public ref class Float3_to_Vector3D_converter: type_converter_base
			{
			public:
				virtual property	Type^		base_type		
				{
					Type^ get( ) override { return Float3::typeid; }
				}
				virtual property	Type^		converted_type	
				{
					Type^ get( ) override { return Vector3D::typeid; }
				}

				virtual				Object^		convert			( Object^ obj ) override;
				virtual				Object^		convert_back	( Object^ obj ) override;

			}; // class float3_to_vector3d_converter

		} // type_converters
	} // namespace editor_base
} // namespace xray

#endif // #ifndef FLOAT3_TO_VECTOR3D_CONVERTER_H_INCLUDED