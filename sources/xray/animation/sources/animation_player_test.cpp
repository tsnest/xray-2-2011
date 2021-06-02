////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/animation_player.h>
#include <xray/animation/mixing_expression.h>
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/fermi_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/mixing_animation_lexeme.h>
#include <xray/animation/mixing_weight_lexeme.h>
#include <xray/animation/mixing_math.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include "mixing_n_ary_tree_base_node.h"

using xray::animation::animation_player;
using xray::animation::mixing::expression;
using xray::animation::instant_interpolator;
using xray::animation::mixing::animation_lexeme;
using xray::animation::mixing::weight_lexeme;
using xray::animation::fermi_interpolator;
using xray::animation::linear_interpolator;

#ifndef MASTER_GOLD
using xray::animation::skeleton_animation;
#endif //#ifndef MASTER_GOLD

using xray::animation::skeleton;
using xray::animation::skeleton_animation_ptr;
using xray::animation::mixing::animation_lexeme_parameters;

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	xray::animation::mixing::tree_stats volatile xray::animation::mixing::g_binary_tree_stats;
	xray::animation::mixing::tree_stats volatile xray::animation::mixing::g_n_ary_tree_stats;
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

static expression test_animation_mixer_impl	(
		xray::mutable_buffer& buffer,
		instant_interpolator& interpolator,
		skeleton_animation_ptr const& animation_a,
		skeleton_animation_ptr const& animation_b,
		skeleton_animation_ptr const& animation_c,
		skeleton_animation_ptr const& animation_d
	)
{
	fermi_interpolator			fermi( 2.f, .01f );
	linear_interpolator			linear( 2.5f );
	linear_interpolator			linear2( 1.f );

	animation_lexeme			a( animation_lexeme_parameters( buffer, "a", animation_a ).weight_interpolator(linear2) );
	animation_lexeme			b( animation_lexeme_parameters( buffer, "b", animation_b ).weight_interpolator(linear2) );
	animation_lexeme			c( animation_lexeme_parameters( buffer, "c", animation_c ).weight_interpolator(linear2) );
	animation_lexeme			d( animation_lexeme_parameters( buffer, "d", animation_d ).weight_interpolator(linear2) );

	weight_lexeme				Wa( buffer, .3f, interpolator );
	weight_lexeme				Wab(buffer, .8f, fermi );
	weight_lexeme				Wc( buffer, .4f, linear );
	
	return						(a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab);
}

static void foo							( )
{
	char a[4096];
	memset						( a, 0, 4096 );
}

static void animation_test_mixer_impl	( skeleton_animation_ptr skeleton_animation, skeleton const& skeleton )
{
	XRAY_UNREFERENCED_PARAMETER	( skeleton );

	using namespace xray;
	using namespace xray::animation;

	mutable_buffer buffer		( ALLOCA(animation_player::stack_buffer_size), animation_player::stack_buffer_size );

	instant_interpolator		interpolator;
	fermi_interpolator			fermi( 2.f, .01f );
	linear_interpolator			linear( 2.5f );
	linear_interpolator			linear2( 1.f );
//	instant_interpolator		fermi;
//	instant_interpolator		linear;

	expression tree				= test_animation_mixer_impl( buffer, interpolator, skeleton_animation, skeleton_animation, skeleton_animation, skeleton_animation );
	foo							( );

	animation_lexeme			a( animation_lexeme_parameters(buffer, "a", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );
	animation_lexeme			b( animation_lexeme_parameters(buffer, "b", skeleton_animation ).weight_interpolator(fermi).time_scale_interpolator(linear2) );
	animation_lexeme			c( animation_lexeme_parameters(buffer, "c", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );
	animation_lexeme			d( animation_lexeme_parameters(buffer, "d", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );

	weight_lexeme				Wa( buffer, .3f, interpolator );
	weight_lexeme				Wab(buffer, .8f, fermi );
	weight_lexeme				Wc( buffer, .4f, linear );
	//weight_lexeme				Wa( buffer, .3f, interpolator );
	//weight_lexeme				Wab(buffer, .8f, interpolator );
	//weight_lexeme				Wc( buffer, .4f, interpolator );

	animation_lexeme			e( animation_lexeme_parameters(buffer, "e", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );
	animation_lexeme			f( animation_lexeme_parameters(buffer, "f", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );
	animation_lexeme			g( animation_lexeme_parameters(buffer, "g", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );
	animation_lexeme			h( animation_lexeme_parameters(buffer, "h", skeleton_animation ).weight_interpolator(interpolator).time_scale_interpolator(linear2) );

	weight_lexeme				We( buffer, .35f, interpolator);
	weight_lexeme				Wef(buffer, .35f, interpolator);
	weight_lexeme				Wg( buffer, .15f, interpolator);
	weight_lexeme				Wgh(buffer, .25f, interpolator);
	weight_lexeme				W_one(buffer, 1.f, interpolator);
	weight_lexeme				W_zero(buffer, 0.f, interpolator);

//	expression tree2			= (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + tree*(1.f - Wef - Wgh);

	timing::timer				timer;
	timer.start					( );
#ifdef DEBUG
	u32 const count				= 1;//000;
#else // #ifdef DEBUG
	u32 const count				= 100000;
#endif // #ifdef DEBUG

//	bone_mixer					bone_mixer( skeleton );
//	animation_player mixer_instance		( bone_mixer, *g_allocator );
//	mixer_instance.set_target_and_tick	( a*(W_one + ((Wa+Wab-Wc)*We-Wg)*Wgh*W_zero), 0 );
//	mixer_instance.set_target_and_tick	( a, 100 );
//	mixer_instance.set_target_and_tick	( tree, 200 );
//	mixer_instance.set_target_and_tick	( b, 200 );
//	mixer_instance.set_target_and_tick	( b, 300 );
//	mixer_instance.set_target_and_tick	( a, 400 );
//	mixer_instance.set_target_and_tick	( (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + Wa/**(a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)/**/*(1.f - Wef - Wgh), 400 );
//	mixer_instance.set_target_and_tick	( (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)*(1.f - Wef - Wgh), 400 );

	for (u32 j=0; j<count; ++j) {
		mutable_buffer local_buffer( buffer );
		animation_player mixer_instance;

		//mixer_instance.set_target_and_tick	( e*We + f*(1.f - We) );
		//mixer_instance.set_target_and_tick	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target_and_tick	( e*We + f*(1.f - We) );
		//mixer_instance.set_target_and_tick	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target_and_tick	( e*We + f*(1.f - We) );
		//mixer_instance.set_target_and_tick	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target_and_tick	( e*We + f*(1.f - We) );
		//mixer_instance.set_target_and_tick	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target_and_tick	( e*We + f*(1.f - We) );
		//mixer_instance.set_target_and_tick	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target_and_tick	( e*We + f*(1.f - We) );
		//mixer_instance.set_target_and_tick	( e*Wg + g*(1.f - Wg) );
//		mixer_instance.set_target_and_tick	( tree );//(a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab) );
//		mixer_instance.set_target_and_tick	( tree2 );//(e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)*(1.f - Wef - Wgh) );
//		mixer_instance.set_target_and_tick	( (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab), 0 );
//		mixer_instance.set_target_and_tick	( d*(1.f - Wef - Wgh), 100 );

//		mixer_instance.set_target_and_tick	( (a*Wa + b*(1.f - Wa)), 100 );
//		mixer_instance.set_target_and_tick	( ((a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)), 100 );

//		mixer_instance.set_target_and_tick	( ( a*(1.f - Wef - Wgh) ) + (e)*Wef + (g)*Wgh, 100 );

//		mixer_instance.set_target_and_tick	( ( a*(1.f - Wef - Wgh) ) + (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh, 100 );

//		mixer_instance.set_target_and_tick	( (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab), 100 );
		mixer_instance.set_target_and_tick	( (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*(1.f - Wef), 100 );
//		mixer_instance.set_target_and_tick	( ((a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab))*(1.f - Wef - Wgh) + (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh, 100 );
//		mixer_instance.set_target_and_tick	( ((a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab))*(1.f - Wef - Wgh), 100 );
//		mixer_instance.set_target_and_tick	( (a*Wa*Wab + c*Wc*(1.f - Wab))*(1.f - Wef - Wgh), 100 );
//		mixer_instance.set_target_and_tick	( (a*Wa + c*Wab)*Wa, 100 );
		//mixer_instance.set_target_and_tick	( a*Wa + b*(1.f - Wa), 0 );
		//mixer_instance.set_target_and_tick	( b*Wab + c*(1.f - Wab), 100 );
		for (u32 i=200; i<=2000; i+=100 ) {
			mixer_instance.tick	( i );
		}

		mixer_instance.tick		( 2100 );
		mixer_instance.tick		( 2200 );
		mixer_instance.tick		( 2300 );
		buffer					= local_buffer;
	}
	LOG_INFO					( "%f: %f", timer.get_elapsed_sec(), timer.get_elapsed_sec()/count );
}

static void animation_test_mixer_impl2			(
		skeleton_animation_ptr skeleton_animation0,
		skeleton_animation_ptr skeleton_animation1,
		skeleton const& skeleton
	)
{
	XRAY_UNREFERENCED_PARAMETER	( skeleton );

	using namespace xray;
	using namespace xray::animation;

	mutable_buffer buffer		( ALLOCA(animation_player::stack_buffer_size), animation_player::stack_buffer_size );

	//instant_interpolator		interpolator;
	fermi_interpolator			fermi( .3f, .005f );
	linear_interpolator			linear2( 1.f );
	instant_interpolator		instant;

//	animation_lexeme			a( buffer, &animation_a, &linear2, animation_a.length(), 0.f, .5f );
//	animation_lexeme			b( buffer, &animation_b, &linear2, animation_b.length(), 0.f, .5f );
//	animation_lexeme			c( buffer, &animation_c, &linear2, animation_c.length(), 0.f, .5f );
//	animation_lexeme			d( buffer, &animation_d, &linear2, animation_d.length() );
//
//	weight_lexeme				w0( buffer, .5f, linear2);
//	weight_lexeme				w1( buffer, .3f, linear2);
//	weight_lexeme				w2( buffer, .7f, linear2);
//
//	bone_mixer					bone_mixer( skeleton );
//	animation_player mixer_instance		( bone_mixer, *g_allocator );
//
//	mixer_instance.set_target_and_tick	( a, 0 );
////	mixer_instance.set_target_and_tick	( b,  100 );
//	mixer_instance.set_target_and_tick	( b,  1000 );
////	mixer_instance.set_target_and_tick	( c*w1 + d*w2,  1100 );
//	mixer_instance.set_target_and_tick	( a, 1100 );

	//animation_lexeme			a( buffer, &animation_a, &linear2, animation_a.length(), 0.f, .5f );
	//animation_lexeme			a_( buffer, &animation_a, &fermi, animation_a.length(), 0.f, 1.f );

	//mixer_instance.set_target_and_tick	( a,  1000 );
	//mixer_instance.set_target_and_tick	( a_, 1000 );
	//mixer_instance.set_target_and_tick	( a_, 1050 );
	//mixer_instance.set_target_and_tick	( a_, 1075 );
	//mixer_instance.set_target_and_tick	( a_, 1100 );
	//mixer_instance.set_target_and_tick	( a_, 1125 );
	//mixer_instance.set_target_and_tick	( a_, 1503 );
	//mixer_instance.set_target_and_tick	( a_, 2000 );
//	mixing::expression* expression = static_cast<mixing::expression*>( ALLOCA( sizeof(mixing::expression) ) );

	{
		animation_player mixer_instance;

		//animation_lexeme			A( buffer, &animation_a, &fermi, animation_a.length(), 0.f, 1.f, u32(-1) );
		//animation_lexeme			B( buffer, &animation_b, &fermi, animation_b.length(), 0.f, 1.f, u32(-1) );

		//mixer_instance.set_target_and_tick	( A, 0 );
		//mixer_instance.set_target_and_tick	( B, 1000 );

//		animation_lexeme			c( buffer, &animation_a, &linear2, animation_a.length(), 0.f, 1.f, 0 );
		animation_lexeme			b( animation_lexeme_parameters( buffer, "b", skeleton_animation1 ).weight_interpolator(instant) );
//		animation_lexeme			a( buffer, &animation_a, &linear2, animation_a.length(), 0.f, 1.f, 0 );
		animation_lexeme			a( animation_lexeme_parameters( buffer, "a", skeleton_animation0, b).weight_interpolator(instant) );
		animation_lexeme			c( animation_lexeme_parameters( buffer, "c", skeleton_animation0, a).weight_interpolator(linear2) );

		weight_lexeme				w0( buffer, .5f, linear2);
		weight_lexeme				w1( buffer, .5f, linear2);
//		weight_lexeme				w2( buffer, .7f, linear2);

		//bone_mixer					bone_mixer( skeleton );
		//animation_player mixer_instance		( bone_mixer, *g_allocator );

		//mixer_instance.set_target_and_tick	( c, 0 );
		//mixer_instance.set_target_and_tick	( b, 100 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w0, 200 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w0, 300 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w0, 1000 );

		animation_lexeme			aa( animation_lexeme_parameters( buffer, "a", skeleton_animation0, b).weight_interpolator(instant).time_scale_interpolator(linear2) );
		animation_lexeme			bb( animation_lexeme_parameters( buffer, "b", skeleton_animation1, a).weight_interpolator(instant) );
		animation_lexeme			cc( animation_lexeme_parameters( buffer, "b", skeleton_animation0, a).weight_interpolator(linear2) );

		// 1
		//mixer_instance.set_target_and_tick	( b, 0 );
		//mixer_instance.set_target_and_tick	( aa, 100 );

		// 2
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 0 );
		//mixer_instance.set_target_and_tick	( aa*w0 + cc*w1, 1000 );

		// 3
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 0 );
		//mixer_instance.set_target_and_tick	( aa*w0 + bb*w1, 1000 );

		// 4
		mixer_instance.set_target_and_tick	( c*w0 + a*w0 + b*w1, 1000 );
//		mixer_instance.set_target_and_tick	( c, 0 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 1000 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 1100 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 1200 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 1300 );

//		mixer_instance.set_target_and_tick	( c, 0 );
//		mixer_instance.set_target_and_tick	( a*w0 + b*w1, 0 );
//		mixer_instance.set_target_and_tick	( aa*w0 + cc*w1, 1000 );
//		mixer_instance.set_target_and_tick	( aa*w0 + bb*w1, 1000 );
		//mixer_instance.set_target_and_tick	( b, 0 );
		//mixer_instance.set_target_and_tick	( aa, 100 );
		//mixer_instance.set_target_and_tick	( c, 0 );
		//mixer_instance.set_target_and_tick	( a*w0 + b*w1, 1000 );
//		mixer_instance.set_target_and_tick	( c*(w0 + w1), 0 );

//		new (expression) mixing::expression( a*w0 + b*w0 );
	}

//	expression->~expression			( );

//	animation_lexeme			a( buffer, &animation_a, &linear2, animation_a.length(), 0.f, 1.f, 0 );
//	animation_lexeme			a2( buffer, &animation_a, &linear2, animation_a.length(), 0.f, .7f, 0 );
////	animation_lexeme			a_shtrich( buffer, &animation_a, &linear2, animation_a.length(), 0.f, 5.f/3.f, u32(-1) );
////	animation_lexeme			b_shtrich( buffer, &animation_b, &linear2, animation_b.length(), 0.f, .6f, u32(-1), .3f );
////	animation_lexeme			b( buffer, &animation_b, &linear2, animation_b.length(), 0.f, 1.f, .3f );
//	animation_lexeme			b( buffer, &animation_b, &linear2, animation_b.length(), 0.f, 1.3f, 0 );
//
//	bone_mixer					bone_mixer( skeleton );
//	animation_player mixer_instance		( bone_mixer, *g_allocator );
//
//	weight_lexeme				w( buffer, 1.f, linear2 );
//
//	mixer_instance.set_target_and_tick	( a*w, 0 );
//	mixer_instance.set_target_and_tick	( a2*w, 200 );
////	mixer_instance.set_target_and_tick	( a_shtrich*w, 500 );
////	mixer_instance.set_target_and_tick	( b_shtrich*w, 500 );
//	mixer_instance.set_target_and_tick	( b*w, 500 );
//	mixer_instance.set_target_and_tick	( b*w, 1000 );
}

static void test_animation_mixer_impl3	(
		skeleton_animation_ptr skeleton_animation0,
		skeleton_animation_ptr skeleton_animation1,
		skeleton const& skeleton
	)
{
	XRAY_UNREFERENCED_PARAMETER	( skeleton );

	using namespace xray;
	using namespace xray::animation;

	mutable_buffer buffer		( ALLOCA(animation_player::stack_buffer_size), animation_player::stack_buffer_size );

	//fermi_interpolator			fermi( .3f, .005f );
	//linear_interpolator			linear2( 1.f );
	instant_interpolator		instant;

	animation_player			mixer_instance;

	animation_lexeme			a( animation_lexeme_parameters( buffer, "a", skeleton_animation0 ).weight_interpolator(instant).time_scale_interpolator(instant) );
	animation_lexeme			b( animation_lexeme_parameters( buffer, "b", skeleton_animation1 ).weight_interpolator(instant).time_scale_interpolator(instant) );
	weight_lexeme				w0( buffer, .5f, instant);
	weight_lexeme				w1( buffer, .3f, instant);
	weight_lexeme				w2( buffer, .7f, instant);

	mixer_instance.set_target_and_tick	( a*w0, 0 );
	mixer_instance.set_target_and_tick	( b*w1, 0 );
}

namespace xray {
	template <typename T>
	inline T const & identity( T const volatile & value ) { return value; }
	template <typename T>
	inline T const & identity( T volatile & value ) { return value; }
} // namespace xray

void xray::animation::test_animation_player_new	(
		skeleton_animation_ptr skeleton_animation0,
		skeleton_animation_ptr skeleton_animation1,
		skeleton const& skeleton
	)
{
#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	mixing::tree_stats before_binary_stats	= mixing::g_binary_tree_stats;
	mixing::tree_stats before_n_ary_stats	= mixing::g_n_ary_tree_stats;
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

	test_animation_mixer_impl3	( skeleton_animation0, skeleton_animation1, skeleton );
	&animation_test_mixer_impl2;
	&animation_test_mixer_impl;
	//for ( u32 i=0; i<1024; ++i )
	//	test_animation_mixer_impl ( skeleton_animation0, skeleton );

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	mixing::tree_stats difference_binary_stats	= mixing::g_binary_tree_stats;
	mixing::tree_stats difference_n_ary_stats	= mixing::g_n_ary_tree_stats;

	difference_binary_stats		-= before_binary_stats;
	difference_n_ary_stats		-= before_n_ary_stats;

	R_ASSERT_CMP				( difference_binary_stats.constructors_count, ==, difference_binary_stats.destructors_count );
	R_ASSERT_CMP				( difference_binary_stats.difference, ==, 0 );
	R_ASSERT_CMP				( difference_n_ary_stats.constructors_count, ==, difference_n_ary_stats.destructors_count );
	R_ASSERT_CMP				( difference_n_ary_stats.difference, ==, 0 );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
}
