////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/scene_render.h>
#include <xray/render/core/effect_manager.h>

// Effect descriptors
#include <xray/render/engine/effect_deffer.h>
#include <xray/render/engine/effect_deffer_aref.h>
#include <xray/render/engine/effect_deffer_materials.h>
#include <xray/render/engine/effect_terrain.h>
#include <xray/render/engine/effect_tree.h>
#include <xray/render/engine/effect_forward_system.h>

// This file is temporary it is needed to find a correct place for effect descriptor initialization.

namespace xray {
namespace render_dx10 {

void scene_render::register_effect_descriptors()
{
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_aref",			NEW( effect_deffer_aref)( true));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_aref_v",		NEW( effect_deffer_aref)( false));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_objects_lod",	NEW( effect_deffer_materials)(1));//NEW( effect_tree));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_trans",			NEW( effect_deffer_aref)( true));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_trans_v",		NEW( effect_deffer_aref)( false));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_trans_v_tuchi",	NEW( effect_deffer_aref)( false));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\def_vertex",		NEW( effect_deffer_materials)(1));
	effect_manager::ref().register_effect_desctiptor( "def_shaders\\lod_old",			NEW( effect_deffer_aref)( false));
	effect_manager::ref().register_effect_desctiptor( "default",						NEW( effect_deffer_materials)(1));
	effect_manager::ref().register_effect_desctiptor( "effects\\lightplanes",			NEW( effect_deffer_materials)(1));
	effect_manager::ref().register_effect_desctiptor( "flora\\leaf_wave",				NEW( effect_deffer_materials)(1));//NEW( effect_tree));
	effect_manager::ref().register_effect_desctiptor( "flora\\trunk_wave",				NEW( effect_deffer_materials)(1));//NEW( effect_tree));
	effect_manager::ref().register_effect_desctiptor( "selflight",						NEW( effect_deffer_materials)(1));
	effect_manager::ref().register_effect_desctiptor( "#terrain",						NEW( effect_terrain));
	effect_manager::ref().register_effect_desctiptor( "#forward_system",				NEW( effect_forward_system));//NEW( effect_tree));

// 	// Blenders by texture 
// 	effect_manager::ref().register_effect_desctiptor_by_textyre(  "grnd/grnd_zemlya",	NEW( effect_deffer_materials)(1));
// 	effect_manager::ref().register_effect_desctiptor_by_textyre(  "briks/briks_br2",	NEW( effect_deffer_materials)(1));
// 	effect_manager::ref().register_effect_desctiptor_by_textyre(  "veh/veh_traktor_br",	NEW( effect_deffer_materials)(1));
// 	effect_manager::ref().register_effect_desctiptor_by_textyre(  "veh/veh_tanchik",	NEW( effect_deffer_materials)(1));
// 	effect_manager::ref().register_effect_desctiptor_by_textyre(  "veh/veh_zil",		NEW( effect_deffer_materials)(1));
// 	effect_manager::ref().register_effect_desctiptor_by_textyre(  "veh/veh_zaz968",		NEW( effect_deffer_materials)(1));
}


} // namespace render 
} // namespace xray 
