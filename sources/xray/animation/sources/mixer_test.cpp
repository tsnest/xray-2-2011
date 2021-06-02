////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixer.h"
#include "bone_mixer.h"
#include "mixing_expression.h"
#include "instant_interpolator.h"
#include "fermi_dirac_interpolator.h"
#include "linear_interpolator.h"
#include "mixing_animation_lexeme.h"
#include "mixing_weight_lexeme.h"
#include "mixing_math.h"
#include "mixing_animation_clip.h"
#include "mixing_n_ary_tree_base_node.h"

using xray::animation::mixer;
using xray::animation::mixing::expression;
using xray::animation::instant_interpolator;
using xray::animation::mixing::animation_clip;
using xray::animation::mixing::animation_lexeme;
using xray::animation::mixing::weight_lexeme;
using xray::animation::fermi_dirac_interpolator;
using xray::animation::linear_interpolator;
using xray::animation::skeleton_animation;
using xray::animation::skeleton;

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	u32 xray::animation::mixing::g_binary_tree_node_constructors	= 0;
	u32 xray::animation::mixing::g_binary_tree_node_destructors		= 0;
	u32 xray::animation::mixing::g_n_ary_tree_node_constructors		= 0;
	u32 xray::animation::mixing::g_n_ary_tree_node_destructors		= 0;
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

static expression test_mixer_impl		(
		xray::mutable_buffer& buffer,
		instant_interpolator& interpolator,
		animation_clip& animation_a,
		animation_clip& animation_b,
		animation_clip& animation_c,
		animation_clip& animation_d
	)
{
	fermi_dirac_interpolator	fermi( 2.f, .01f );
	linear_interpolator			linear( 2.5f );

	animation_lexeme			a( buffer, &animation_a );
	animation_lexeme			b( buffer, &animation_b );
	animation_lexeme			c( buffer, &animation_c );
	animation_lexeme			d( buffer, &animation_d );

	weight_lexeme				Wa( buffer, .3f, &interpolator );
	weight_lexeme				Wab(buffer, .8f, &fermi );
	weight_lexeme				Wc( buffer, .4f, &linear );
	
	return						(a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab);
}

static void foo							( )
{
	char a[4096];
	memset						( a, 0, 4096 );
}

static void test_mixer_impl				( skeleton_animation const& skeleton_animation, skeleton const& skeleton )
{
	using namespace xray;
	using namespace xray::animation;

	mutable_buffer buffer		( ALLOCA(mixer::stack_buffer_size), mixer::stack_buffer_size );

	instant_interpolator		interpolator;
	fermi_dirac_interpolator	fermi( 2.f, .01f );
	linear_interpolator			linear( 2.5f );
//	instant_interpolator		fermi;
//	instant_interpolator		linear;

	animation_clip				animation_a( "a", &interpolator, &skeleton_animation, animation_clip::full );
	animation_clip				animation_b( "b", &fermi,		 &skeleton_animation, animation_clip::full );
	animation_clip				animation_c( "c", &interpolator, &skeleton_animation, animation_clip::full );
	animation_clip				animation_d( "d", &interpolator, &skeleton_animation, animation_clip::full );
	animation_clip				animation_e( "e", &interpolator, &skeleton_animation, animation_clip::full );
	animation_clip				animation_f( "f", &interpolator, &skeleton_animation, animation_clip::full );
	animation_clip				animation_g( "g", &interpolator, &skeleton_animation, animation_clip::additive );
	animation_clip				animation_h( "h", &interpolator, &skeleton_animation, animation_clip::additive );

	expression tree				= test_mixer_impl( buffer, interpolator, animation_a, animation_b, animation_c, animation_d );
	foo							( );

	animation_lexeme			a( buffer, &animation_a, .5f );
	animation_lexeme			b( buffer, &animation_b, .5f );
	animation_lexeme			c( buffer, &animation_c, .5f );
	animation_lexeme			d( buffer, &animation_d );

	weight_lexeme				Wa( buffer, .3f, &interpolator );
	weight_lexeme				Wab(buffer, .8f, &fermi );
	weight_lexeme				Wc( buffer, .4f, &linear );
	//weight_lexeme				Wa( buffer, .3f, &interpolator );
	//weight_lexeme				Wab(buffer, .8f, &interpolator );
	//weight_lexeme				Wc( buffer, .4f, &interpolator );

	animation_lexeme			e( buffer, &animation_e );
	animation_lexeme			f( buffer, &animation_f );
	animation_lexeme			g( buffer, &animation_g );
	animation_lexeme			h( buffer, &animation_h );

	weight_lexeme				We( buffer, .35f, &interpolator);
	weight_lexeme				Wef(buffer, .35f, &interpolator);
	weight_lexeme				Wg( buffer, .15f, &interpolator);
	weight_lexeme				Wgh(buffer, .25f, &interpolator);
	weight_lexeme				W_one(buffer, 1.f, &interpolator);
	weight_lexeme				W_zero(buffer, 0.f, &interpolator);

	expression tree2			= (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + tree*(1.f - Wef - Wgh);

	timing::timer				timer;
	timer.start					( );
#ifdef DEBUG
	u32 const count				= 1000;
#else // #ifdef DEBUG
	u32 const count				= 100000;
#endif // #ifdef DEBUG

	bone_mixer					bone_mixer( skeleton );
	mixer mixer_instance		( bone_mixer, *g_allocator );
//	mixer_instance.set_target	( a*(W_one + ((Wa+Wab-Wc)*We-Wg)*Wgh*W_zero), 0 );
	mixer_instance.set_target	( a, 100 );
//	mixer_instance.set_target	( b, 200 );
//	mixer_instance.set_target	( b, 300 );
	mixer_instance.set_target	( (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)*(1.f - Wef - Wgh), 400 );

	for (u32 i=0; i<count; ++i) {
		mutable_buffer local_buffer( buffer );
		animation::bone_mixer	bone_mixer( skeleton );
		mixer mixer_instance	( bone_mixer, *g_allocator );

		//mixer_instance.set_target	( e*We + f*(1.f - We) );
		//mixer_instance.set_target	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target	( e*We + f*(1.f - We) );
		//mixer_instance.set_target	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target	( e*We + f*(1.f - We) );
		//mixer_instance.set_target	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target	( e*We + f*(1.f - We) );
		//mixer_instance.set_target	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target	( e*We + f*(1.f - We) );
		//mixer_instance.set_target	( e*Wg + g*(1.f - Wg) );
		//mixer_instance.set_target	( e*We + f*(1.f - We) );
		//mixer_instance.set_target	( e*Wg + g*(1.f - Wg) );
//		mixer_instance.set_target	( tree );//(a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab) );
//		mixer_instance.set_target	( tree2 );//(e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)*(1.f - Wef - Wgh) );
//		mixer_instance.set_target	( (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab), 0 );
//		mixer_instance.set_target	( (e*We + f*(1.f - We))*Wef + (g*Wg + h*(1.f - Wg))*Wgh + (a*Wa + b*(1.f - Wa))*Wab + (c*Wc + d*(1.f - Wc))*(1.f - Wab)*(1.f - Wef - Wgh), 100 );
		mixer_instance.set_target	( a*Wa + b*(1.f - Wa), 0 );
		mixer_instance.set_target	( b*Wab + c*(1.f - Wab), 100 );
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

void xray::animation::test_mixer		( skeleton_animation const& skeleton_animation, skeleton const& skeleton )
{
	test_mixer_impl				( skeleton_animation, skeleton );

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
	R_ASSERT_CMP				( mixing::g_binary_tree_node_constructors, ==, mixing::g_binary_tree_node_destructors );
	R_ASSERT_CMP				( mixing::g_n_ary_tree_node_constructors, ==, mixing::g_n_ary_tree_node_destructors );
#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
}