////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLEND_FUNCTION_H_INCLUDED
#define BLEND_FUNCTION_H_INCLUDED

namespace xray {
namespace animation {

class blend_function {

public:
 inline float	factor( float time, float blending_time )const
 {
	 
//#ifdef FOOT_SHIFT_DEBUG
//	 return 1;
//#endif

	 const float epsilon = 0.005f;
	 const float d = blending_time / ( 2.f * math::log( 1.f/epsilon +1.f ) ); 

	 const float magnitude = 1.f + 2.f * epsilon;
	 const float t0 = blending_time / 2.f;

	 const float gaus = magnitude /( 1.f + math::exp( - ( time - t0 ) / d ) );

	 float ret = gaus - epsilon;

	 math::clamp( ret, 0.f, 1.f  );

	 return ret;
 }


//private:
//	inline static float	blend_time( )	{ return 0.490f; }

}; // class blend_function

}  // namespace animation
}  // namespace xray

#endif // #ifndef BLEND_FUNCTION_H_INCLUDED