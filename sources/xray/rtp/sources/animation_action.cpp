////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_action.h"
#include <xray/console_command.h>

namespace xray {
namespace rtp {

bool g_transform_depends_from_previous_action  = false;
static console_commands::cc_bool cmd_transform_depends_from_previous_action( "rtp_transform_depends_from_previous_action", g_transform_depends_from_previous_action, true, console_commands::command_type_engine_internal );

bool g_force_recalculate_action_transforms  = false;
static console_commands::cc_bool cmd_force_recalculate_action_transforms( "rtp_force_recalculate_action_transforms", g_force_recalculate_action_transforms, true, console_commands::command_type_user_specific );

bool g_save_cached_data  = false;
static console_commands::cc_bool cmd_save_cached_data( "rtp_save_cached_data", g_save_cached_data, true, console_commands::command_type_user_specific );



 void generate_weights_count	(
		u32 const samples_count,
		u32 const current_weight_id,
		u32 const current_sum,
		u32& count
	)
{
	if ( !current_weight_id ) {
		++count;
		return;
	}
	
	for ( u32 i=0; i < samples_count - current_sum ; ++i )
		generate_weights_count	( samples_count, current_weight_id - 1, current_sum + i, count );
}

void generate_weights	(
		u32* temp_indices,
		float*& output,
		u32 const animations_count,
		u32 const samples_count,
		u32 const current_weight_id,
		u32 const current_sum
	)
{
	float const quant						= 1.f / ( samples_count - 1 );

	if ( !current_weight_id ) {
		temp_indices[0]						= samples_count - 1 - current_sum;
		for ( u32 i=0; i<animations_count; ++i )
			*output++						= temp_indices[i] * quant;
		return;
	}
	
	for ( u32 i=0; i < samples_count - current_sum ; ++i ) {
		temp_indices[ current_weight_id ]	= i;
		generate_weights					( temp_indices, output, animations_count, samples_count, current_weight_id - 1, current_sum + i );
	}
}






u32 generate_weights_recursion( buffer_vector< float > &weights, float quant, u32 animations_count, u32 samples_per_animation, u32 id )
{
	
	if ( animations_count == 2 && id > samples_per_animation - 1 )
			return id - samples_per_animation + 1;
	
	//float quant = 1.f/float( start_samples_per_animation );
	if ( animations_count == 2 && id <= samples_per_animation - 1 )
	{
		weights[1] = ( id ) * quant;
		weights[0] = ( samples_per_animation - id - 1  ) * quant;
		return 0;
	}
	
	for ( u32 i = 0; i < samples_per_animation; ++i )
	{
		weights[ animations_count - 1 ] = i * quant;
		id = generate_weights_recursion( weights, quant, animations_count - 1, samples_per_animation - i, id );
		if( id == 0 )
			return 0;
		--id;

	}
	

	return 0;
}

void generate_weights( buffer_vector< float > &weights, u32 animations_count, u32 samples_per_animation, u32 id )
{
	ASSERT( weights.size() == animations_count );
	float quant = 1.f/float( samples_per_animation - 1 );
	
	generate_weights_recursion( weights, quant, animations_count, samples_per_animation, id );
}

} // namespace rtp
} // namespace xray