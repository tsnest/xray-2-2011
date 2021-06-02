////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_layer.h"

#include <xray/console_command.h>

namespace xray {
namespace animation {

struct mix_method_name_pair
{
	
	rotation_mixing_method method;
	pcstr name;

} ; // mix_method_name_pair

mix_method_name_pair mix_methods[ ] = 
{
	{ rotation_mixing_angles,							"angles"		},
	{ rotation_mixing_angles_representation_reduce,		"angles_r"		},
	{ rotation_mixing_qslim,							"qslim"			},
	{ rotation_mixing_sasquatch,						"sasquatch"		},
	{ rotation_mixing_multilinear,						"multilinear"	},
	
	//rotation_mixing_max,
};

rotation_mixing_method find_method( pcstr args )
{
	if ( !args )
		return rotation_mixing_max;
	
	u32 size = sizeof( mix_methods ) / sizeof( mix_methods[0] );

	for( u32 it = 0; it < size; ++it )
		if( strings::compare(  mix_methods[it].name, args ) == 0  )
				return  mix_methods[it].method;
	return rotation_mixing_max;
}

pcstr find_name( rotation_mixing_method method )
{

	u32 size = sizeof( mix_methods ) / sizeof( mix_methods[0] );

	for( u32 it = 0; it < size; ++it )
		if( mix_methods[it].method == method )
			return  mix_methods[it].name;

	return 0;
}

void all_methods_names ( string512 names )
{
	
	names[0] = '\0';
	
	u32 size = sizeof( mix_methods ) / sizeof( mix_methods[0] );

	for( u32 it = 0; it < size; ++it )
	{
		strings::join( names, sizeof(string512), names, mix_methods[it].name, ", " );
	}


}

static class mixing_type_command	:public console_commands::console_command
{
	typedef console_commands::console_command		super;
public:
	mixing_type_command		( ): super(	"animation_mixing_type" )
								,value( rotation_mixing_angles_representation_reduce ) //rotation_mixing_sasquatch // rotation_mixing_angles
								 {} 

	virtual void		execute					(pcstr args)
	{

		rotation_mixing_method method = find_method( args );
		
		if( method == rotation_mixing_max )
		{
			on_invalid_syntax( args );
			return;
		}

		value = method;
		
	}

	virtual void		status					(status_str& dest) const
	{
		pcstr name = find_name( value );
		if( !name )
				return;
		strings::copy( dest, name );
	} 

	virtual void		info					(info_str& dest) const
	{
		all_methods_names( dest );
	}

	virtual void		syntax					(syntax_str& dest) const
	{
		all_methods_names( dest );
	}

public:
	 rotation_mixing_method mixing_rotation( ) { return rotation_mixing_method( value ); }

private:
	rotation_mixing_method			value;

} s_mixing_type_command; // mixing_type_command



rotation_mixing_method mixing_rotation()
{
	return s_mixing_type_command.mixing_rotation( );
}

}  // namespace animation
}  // namespace xray



