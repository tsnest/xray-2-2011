////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_CONSTANT_H_INCLUDED
#define RES_CONSTANT_H_INCLUDED

#include "constant_defines.h"

namespace xray {
namespace render_dx10 {

class res_constant_buffer;

enum	{ slot_dest_buffer_null = static_cast<u8>( -1)};	// the 255th index is considered the NULL buffer index
													// slot with NULL buffer index is disabled

//////////////////////////////////////////////////////////////////////////
class constant_slot
{
public:
	// Default constructor not acceptable as this class is used in union.
	// The user supposed to initialize it.
	void reset() 
	{
		cls				= static_cast<u8> ( -1);
		buffer_index	= slot_dest_buffer_null;
		slot_index		= static_cast<u16> ( -1);
	}

	inline bool equal( constant_slot const & other) const
	{
		return ( slot_index==other.slot_index) && ( cls == other.cls) && ( buffer_index == other.buffer_index);
	}

public:
	// The ordering is made with consideration struct alignment to 32 bits
	u16	cls;			// element class ( enum_constant_class)
	u16	buffer_index;	// constant buffer index 
	u32	slot_index;		// linear index ( pixel)
};

//////////////////////////////////////////////////////////////////////////
class constant_source
{
public:
	inline constant_source() : ptr( NULL), size( 0) {}
	inline constant_source( void * ptr, u32 size) : ptr( ptr), size( size) {}

	bool equal( constant_source const & other) const { return ( ptr == other.ptr && size == other.size);}

public:
	void*	ptr;
	u32		size;
};

//////////////////////////////////////////////////////////////////////////
class constant_host
{
public:

	inline constant_host (shared_string name); 

	void set( float);
	void set( math::float2);
	void set( math::float3);
	void set( math::float4);
	void set( math::float4x4);
	void set( int);
	void set( math::int2);

public:
	bool operator == ( shared_string const& other) const	{ return name == other;}
	bool operator < ( shared_string const& other) const		{ return name < other;}
	bool operator == ( constant_host const& other) const	{ return name == other.name;}
	bool operator < ( constant_host const& other) const		{ return name < other.name;}


	shared_string		name;				// HLSL-name
	enum_constant_type	type;				
	constant_source		source;

#pragma warning( push)
#pragma warning( disable:4201)
	union 
	{
		struct 
		{ 	
			constant_slot	vs_slot;	
			constant_slot	ps_slot;		
		};
		constant_slot	shader_slots[2];
	};
#pragma warning( pop)

	u32	update_markers[2];

	//constant_source source;	
};


class constant;
typedef fastdelegate::FastDelegate1<constant const*>	const_setup_cb;


class constant
{
public:
	constant( constant_host * host): 
		host	( host),
		type	( host->type),
		name	( host->name)
	{
		ASSERT( host);
		ASSERT( type != rc_INVALID);

		slot.reset();
// 		ps_slot.reset();
// 		vs_slot.reset();
	};

	inline bool equal( constant const & other) const
	{

		return ( name == other.name) 
				&& ( type == other.type)
//				&& ( m_loader == other.m_loader)
				&& source.equal( other.source)
				&& slot.equal( other.slot);
//  				&& ps_slot.equal	( other.ps_slot) 
//  				&& vs_slot.equal	( other.vs_slot) ;
	}

	inline bool equal( constant* other) {return equal( *other);}
	
	inline u32				get_type() const { return type;}
	inline char const *		get_name() const { return name.c_str();}

public:
	
	// Members need to be reordered to gain minimum structure size

	shared_string		name;				// HLSL-name
	enum_constant_type	type;				// float=0/integer=1/boolean=2
	constant_slot		slot;		
	constant_source		source;
	constant_host *		host;

};// class constant

//typedef	intrusive_ptr<constant, constant, threading::single_threading_policy> constant *;

} // namespace render
} // namespace xray

#include "constant_inline.h"

#endif // #ifndef RES_CONSTANT_H_INCLUDED