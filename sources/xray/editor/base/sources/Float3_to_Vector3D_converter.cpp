////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Float3_to_Vector3D_converter.h"

namespace xray{
namespace editor_base{
namespace type_converters{

Object^ Float3_to_Vector3D_converter::convert ( Object^ obj )
{
	Float3 vec = (Float3)obj;
	return Vector3D( vec.x, vec.y, vec.z );
}

Object^ Float3_to_Vector3D_converter::convert_back ( Object^ obj )
{
	Vector3D vec = (Vector3D)obj;
	return Float3( float3( (float)vec.X, (float)vec.Y, (float)vec.Z ) );
}

} // namespace type_converters
} // namespace editor_base
} // namespace xray