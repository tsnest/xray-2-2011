////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_UTILS_H_INCLUDED
#define MATH_UTILS_H_INCLUDED

namespace xray {
namespace math {

template<class Params>
struct float_sorted
{
	Params	*_0;
	Params	*_1;
	Params	*_2;

	inline	float_sorted( Params& a0, Params &a1, Params &a2, float f0, float f1, float f2 ):
	_0(0),_1(0),_2(0)
	{
		if( f0 > f1 )
		{
			if( f0 > f2 )
			{// f0  > (f1,f2)
				_0 = &a0;
				if( f1 > f2 )	{ _1 = &a1; _2 = &a2; }
				else			{ _1 = &a2; _2 = &a1; }

			} else // f0  > f1; f0 < f2
				{ _0	= &a2; _1	= &a0; _2	= &a1; }
		} else {// f1 > f0

			if( f1 > f2 )
			{
				_0 = &a1;
				if( f2 > f0 )	{ _1 = &a2; _2 = &a0; }
				else			{ _1 = &a0; _2 = &a2; }
			} else // f1 > f0 ; f1 < f2
				{  _0	= &a2; _1	= &a1; _2	= &a0;  }

		}
		
	}
inline		Params	&r0(){ return *_0 ;}
inline		Params	&r1(){ return *_1 ;}
inline		Params	&r2(){ return *_2 ;}
};
}//math
}//xray


#endif // #ifndef MATH_UTILS_H_INCLUDED