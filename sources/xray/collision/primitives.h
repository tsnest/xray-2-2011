////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_PRIMITIVES_H_INCLUDED
#define XRAY_COLLISION_PRIMITIVES_H_INCLUDED

namespace xray {
namespace collision {

enum primitive_type
{
	primitive_sphere,
	primitive_box,
	primitive_cylinder,
	primitive_capsule,
	primitive_truncated_sphere,
	
	primitive_unknown = 99,
};

struct sphere {
	sphere( float r ): radius( r ){}
	float radius;
};

struct box {
	box( float3 const& hs): half_side( hs ){}
	float3 half_side;
};

struct cylinder{
	cylinder( float hl, float r ): half_length( hl ), radius( r ){}
	float half_length;
	float radius;
};

struct capsule{
	capsule( float hl, float r ): half_length( hl ), radius( r ){}
	float half_length;
	float radius;
};

struct truncated_sphere{
	truncated_sphere( float r ): radius( r ){}
	float radius;
};

struct primitive
{
	inline	primitive( ): type( primitive_unknown )	, data_( float3( 1, 1, 1 ) )	{}
	
	inline	primitive( sphere const&  p )	: type( primitive_sphere )	, data_( float3( p.radius, 0, 0 ) )				{}
	inline	primitive( box const&  p )		: type( primitive_box )		, data_( p.half_side )							{}
	inline	primitive( cylinder const&  p )	: type( primitive_cylinder ), data_( float3( p.half_length, p.radius, 0 ) )	{}
	inline	primitive( capsule const&  p )	: type( primitive_capsule )	, data_( float3( p.half_length, p.radius, 0 ) )	{}
	inline	primitive( truncated_sphere const&  p )	: type( primitive_truncated_sphere ), data_( float3( p.radius, 0, 0 ) )	{}

	inline	collision::sphere		sphere	() const { ASSERT( type == primitive_sphere );		return collision::sphere( data_[0] ); }
	inline	collision::box			box		() const { ASSERT( type == primitive_box );			return collision::box( data_ ); }
	inline	collision::cylinder		cylinder() const { ASSERT( type == primitive_cylinder );	return collision::cylinder( data_[1], data_[0] ); }
	inline	collision::capsule		capsule	() const { ASSERT( type == primitive_capsule );	return collision::capsule( data_[1], data_[0] ); }
	inline	collision::truncated_sphere		truncated_sphere() const { ASSERT( type == primitive_truncated_sphere );	return collision::truncated_sphere( data_[0] ); }

private:
	inline	primitive & operator = ( primitive const&){ NODEFAULT( return *this ); }

public:
	primitive_type	type;
	float3			data_;

};

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_PRIMITIVES_H_INCLUDED