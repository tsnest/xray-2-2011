////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_dumper.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/fermi_interpolator.h>

using xray::animation::mixing::n_ary_tree_dumper;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_n_ary_operation_node;

using xray::animation::instant_interpolator;
using xray::animation::linear_interpolator;
using xray::animation::fermi_interpolator;

void n_ary_tree_dumper::visit			( n_ary_tree_animation_node& node )
{
	u32 const buffer_size	= (m_offset + 4 + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset + 4 );
	string[m_offset]		= 0;

	if ( !node.driving_animation() )
		LOG_DEBUG				(
			"%sanimation: %s, to zero[%s], intervals[%d]",
			string,
			node.identifier(),
			node.is_transitting_to_zero() ? "+" : "-",
			node.animation_intervals_count()
		);
	else
		LOG_DEBUG				(
			"%sanimation: %s, synchronized with[%s],intervals[%d]",
			string,
			node.identifier(),
			node.driving_animation()->identifier(),
			node.animation_intervals_count()
		);

	string[m_offset]		= ' ';
	string[m_offset + 4]	= 0;
	LOG_DEBUG				( "%sinterpolator:", string );

	m_offset				+= 4;
	node.interpolator().accept( *this );
	m_offset				-= 4;

	propagate				( node );
}

void n_ary_tree_dumper::visit			( n_ary_tree_weight_node& node )
{
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%sweight: %f", string, node.weight() );

	node.interpolator().accept	( *this );
}

void n_ary_tree_dumper::visit			( n_ary_tree_time_scale_node& node )
{
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%stime scale: %f", string, node.time_scale() );

	node.interpolator().accept	( *this );
}

void n_ary_tree_dumper::visit			( n_ary_tree_weight_transition_node& node )
{
	u32 const buffer_size	= (m_offset + 4 + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset + 4 );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%sweight transition", string );

	string[m_offset]		= ' ';
	string[m_offset + 4]	= 0;

	m_offset				+= 4;

	node.from().accept		( *this );
	node.to().accept		( *this );

	LOG_DEBUG				( "%sinterpolator:", string );
	node.interpolator().accept	( *this );

	m_offset				-= 4;
}

void n_ary_tree_dumper::visit			( n_ary_tree_time_scale_transition_node& node )
{
	u32 const buffer_size	= (m_offset + 4 + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset + 4 );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%stime scale transition", string );

	string[m_offset]		= ' ';
	string[m_offset + 4]	= 0;

	m_offset				+= 4;

	node.from().accept		( *this );
	node.to().accept		( *this );

	LOG_DEBUG				( "%sinterpolator:", string );
	node.interpolator().accept	( *this );

	m_offset				-= 4;
}

void n_ary_tree_dumper::visit			( n_ary_tree_addition_node& node )
{
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%soperator +", string );

	propagate				( node );
}

void n_ary_tree_dumper::visit			( n_ary_tree_subtraction_node& node )
{
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%soperator -", string );

	propagate				( node );
}

void n_ary_tree_dumper::visit			( n_ary_tree_multiplication_node& node )
{
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	LOG_DEBUG				( "%soperator *", string );

	propagate				( node );
}

template < typename T >
inline void n_ary_tree_dumper::propagate	( T& node )
{
	m_offset				+= 4;

	n_ary_tree_base_node** i		= node.operands( sizeof(T) );
	n_ary_tree_base_node** const e	= i + node.operands_count( );
	for ( ; i != e; ++i )
		(*i)->accept		( *this );

	m_offset				-= 4;
}

void n_ary_tree_dumper::visit				( instant_interpolator const& interpolator )
{
	XRAY_UNREFERENCED_PARAMETER	( interpolator );

	m_offset				+= 4;
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	m_offset				-= 4;

	LOG_DEBUG				( "%stype            : instant", string );
	LOG_DEBUG				( "%stransition time : %f", string, interpolator.transition_time() );
}

void n_ary_tree_dumper::visit				( linear_interpolator const& interpolator )
{
	m_offset				+= 4;
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	m_offset				-= 4;

	LOG_DEBUG				( "%stype            : linear", string );
	LOG_DEBUG				( "%stransition time : %f", string, interpolator.transition_time() );
}

void n_ary_tree_dumper::visit				( fermi_interpolator const& interpolator )
{
	m_offset				+= 4;
	u32 const buffer_size	= (m_offset + 1);
	pstr const string		= static_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	xray::memory::fill8		( string, buffer_size, ' ', m_offset );
	string[m_offset]		= 0;
	m_offset				-= 4;

	LOG_DEBUG				( "%stype            : fermi", string );
	LOG_DEBUG				( "%stransition time : %f", string, interpolator.transition_time() );
	LOG_DEBUG				( "%sepsilon         : %f", string, interpolator.epsilon() );
}