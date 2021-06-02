////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/physics/rigid_body.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(physics_rigid_body_construction_info)

namespace xray{
namespace physics{

bt_rigid_body_construction_info::bt_rigid_body_construction_info( )
:m_mass(0.0f),
m_linearDamping(0.0f),
m_angularDamping(0.0f),
m_friction(0.5f),
m_restitution(0.0f),
m_linearSleepingThreshold(0.8f),
m_angularSleepingThreshold(1.0f),
m_additionalDamping(false),
m_additionalDampingFactor(0.005f),
m_additionalLinearDampingThresholdSqr(0.01f),
m_additionalAngularDampingThresholdSqr(0.01f),
m_additionalAngularDampingFactor(0.01f),
m_render_model_offset(0,0,0)
{}

bool bt_rigid_body_construction_info::load( configs::binary_config_value const& cfg )
{
	if(!cfg.value_exists("rigid_body"))
		return false;

	configs::binary_config_value const& v = cfg["rigid_body"];
	m_mass									= v["mass"];
	if(v.value_exists("mass_center"))
		m_render_model_offset				= v["mass_center"];

	m_linearDamping							= v["m_linearDamping"];
	m_angularDamping						= v["angularDamping"];
	m_friction								= v["friction"];
	m_restitution							= v["restitution"];
	m_linearSleepingThreshold				= v["linearSleepingThreshold"];
	m_angularSleepingThreshold				= v["angularSleepingThreshold"];
	m_additionalDamping						= v["additionalDamping"];
	m_additionalDampingFactor				= v["additionalDampingFactor"];
	m_additionalLinearDampingThresholdSqr	= v["additionalLinearDampingThresholdSqr"];
	m_additionalAngularDampingThresholdSqr	= v["additionalAngularDampingThresholdSqr"];
	m_additionalAngularDampingFactor		= v["additionalAngularDampingFactor"];
	return true;
}

#ifndef MASTER_GOLD

bool bt_rigid_body_construction_info::load( configs::lua_config_value const& cfg )
{
	if(!cfg.value_exists("rigid_body"))
		return false;

	configs::lua_config_value const& v		= cfg["rigid_body"];
	m_mass									= v["mass"];
	if(v.value_exists("mass_center"))
		m_render_model_offset				= v["mass_center"];

	m_linearDamping							= v["m_linearDamping"];
	m_angularDamping						= v["angularDamping"];
	m_friction								= v["friction"];
	m_restitution							= v["restitution"];
	m_linearSleepingThreshold				= v["linearSleepingThreshold"];
	m_angularSleepingThreshold				= v["angularSleepingThreshold"];
	m_additionalDamping						= v["additionalDamping"];
	m_additionalDampingFactor				= v["additionalDampingFactor"];
	m_additionalLinearDampingThresholdSqr	= v["additionalLinearDampingThresholdSqr"];
	m_additionalAngularDampingThresholdSqr	= v["additionalAngularDampingThresholdSqr"];
	m_additionalAngularDampingFactor		= v["additionalAngularDampingFactor"];
	return true;
}

void bt_rigid_body_construction_info::save( configs::lua_config_value& cfg )
{
	configs::lua_config_value v = cfg["rigid_body"];
	v["mass_center"]							= m_render_model_offset;
	v["mass"]									= m_mass;
	v["m_linearDamping"]						= m_linearDamping;
	v["angularDamping"]							= m_angularDamping;
	v["friction"]								= m_friction;
	v["restitution"]							= m_restitution;
	v["linearSleepingThreshold"]				= m_linearSleepingThreshold;
	v["angularSleepingThreshold"]				= m_angularSleepingThreshold;
	v["additionalDamping"]						= m_additionalDamping;
	v["additionalDampingFactor"]				= m_additionalDampingFactor;
	v["additionalLinearDampingThresholdSqr"]	= m_additionalLinearDampingThresholdSqr;
	v["additionalAngularDampingThresholdSqr"]	= m_additionalAngularDampingThresholdSqr;
	v["additionalAngularDampingFactor"]			= m_additionalAngularDampingFactor;
}

#endif


} // namespace physics
} // namespace xray