////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/bone_names.h>
#include <xray/animation/skeleton.h>

#include <boost/crc.hpp>

#ifndef	MASTER_GOLD
#include <xray/configs_lua_config.h>
#endif // MASTER_GOLD

namespace xray {
namespace animation {


bone_names::bone_names( ):
//bone_names_idx()( 0 ),
m_bone_count( u32(-1) ),
m_internal_memory_position ( size_t(-1) )
{

}

u32	bone_names::count_internal_memory_size	( u32 bones_count )
{
	return bones_count * sizeof( bone_name_index ); //excluding self memory 
}

void bone_names::create_internals_in_place(u32 bones_count, void* memory  )
{
	R_ASSERT( memory );
	
	m_internal_memory_position = size_t ( pbyte( memory ) - pbyte( this ) );

	m_bone_count = bones_count;

	//*( (bone_names*)memory ) = *this;
	
	//bone_names_idx() = (bone_name_index*) memory;

	for ( u32 i = 0; i < bones_count; ++i )
	{
		//bone_name_index idx = bone_name_index();
		bone_name_index * idx = bone_names_idx();

		new( & (idx[ i ]) ) bone_name_index();// = idx;


	}

}
void bone_names::create_internals_in_place	( const bone_names &names, void* memory )
{
	create_internals_in_place( names.bones_number(), memory );
	for ( u32 i = 0; i < bones_number(); ++i )
	{
		bone_name_index * my_idx = bone_names_idx();
		bone_name_index const * in_idx = names.bone_names_idx();
		my_idx[ i ] = in_idx[ i ];
	}
}

bone_name_index::bone_name_index(  u32 idx, pcstr aname ):
	index					( idx )
{
	boost::crc_32_type		processor;
	processor.process_block	( aname, aname + strings::length( aname ) );
	crc						= processor.checksum( );

	xray::strings::copy		( name, aname );
}

bone_name_index::bone_name_index( ):
	index					( u32(-1) ),
	crc						(  u32(-1) )
{
}

void	bone_names::set_name( bone_index_type bone_index, pcstr name )
{
	bone_names_idx()[bone_index] = bone_name_index( bone_index, name );
}

bool crc_cmp( const bone_name_index& l, const bone_name_index& r )
{
	return l.crc < r.crc;
}

void	bone_names::read( xray::configs::binary_config_value const &cfg )
{
	const bone_index_type size = cfg.size();
	//bone_names_idx().resize( size );
	R_ASSERT( size == m_bone_count );

	bone_name_index * my_idx = bone_names_idx();

	//dbg_idx  = my_idx;

	for ( bone_index_type i = 0; i < size; ++i )
	{
		my_idx[i] = bone_name_index ( i, pcstr ( static_cast<pcstr>( cfg[i] ) ) ) ;
	}

	std::sort( my_idx, my_idx + m_bone_count, crc_cmp );
}

void	bone_names::write( xray::configs::lua_config_value	&cfg )const
{

#ifndef	MASTER_GOLD
	const bone_index_type size = m_bone_count;
	for ( bone_index_type i = 0; i < size; ++i )
	{
		const bone_name_index &b = bone_names_idx()[i];
		cfg[ b.index ] = b.name;
		//cfg[i] = bone_names_idx()[i].c_str();
	}
#else
	XRAY_UNREFERENCED_PARAMETER					( cfg );
	NODEFAULT();
#endif // MASTER_GOLD
}

void	bone_names::write( stream &file )const
{
	const u32 size = m_bone_count;
	fwrite( &(size), sizeof(size), 1, file );
	fwrite( bone_names_idx(), sizeof(bone_name_index), size, file );
}

bone_index_type	bone_names::bone_index( pcstr name )const
{
	//u32 crc = string_crc( name );
	bone_name_index temp( u32(-1), name );

	vector<bone_name_index>::const_iterator r = std::lower_bound( bone_names_idx(), bone_names_idx() + m_bone_count, temp, crc_cmp );

	if ( temp.crc == r->crc &&
		name[ 0 ] == r->name[0] 
		)
		return r->index;

	return u32(-1);


	//vector < string_type >::const_iterator r =	 std::find( bone_names_idx().begin(), bone_names_idx().end(),  name  );
	//if ( r == bone_names_idx().end() )
	//	return ( u32(-1) );
	//return u32( r - bone_names_idx().begin() );
}

pcstr bone_names::bone_name		( bone_index_type index )const
{
	vector<bone_name_index>::const_iterator it = bone_names_idx();
	for (; it!=bone_names_idx() + m_bone_count; ++it)
		if (it->index == index)
			return it->name;

	return NULL;
//	return bone_names_idx()[index].name.c_str();
}

void bone_names::create_index	( const skeleton &skel, vector< bone_index_type > &index )const
{
	const bone_index_type sz = skel.get_bones_count();

	R_ASSERT( sz <= m_bone_count );

	index.resize( sz );

	for ( bone_index_type i = 0; i < sz; ++i )
		index[i] = bone_index( skel.get_bone( i ).id() );

}

} // namespace animation
} // namespace xray