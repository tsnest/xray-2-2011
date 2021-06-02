////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_track.h"
#include "sisl/sislP.h"

//template<>
//inline float point<float,1>( int index, const SISLCurve &cv )
//{
//	ASSERT( index >= 0 );
//	ASSERT( index < cv.in + cv.ik );
//	return float(cv.ecoef[ index  ]);
//	
//
//}
//template< typename Point_type, int Dimension >
//void sisl_curve_to_bi_spline_data( const SISLCurve &cv, bi_spline_data<Point_type> & data)
//{
//	data.data.resize( cv.in + cv.ik );
//
//	for( u32 i = 0; i < data.data.size(); ++i )
//	{
//		data.data[i].first	= float (cv.et[i]);
//		data.data[i].second	= point<Point_type,Dimension>(i,cv);
//	}
//}

/*
void test_track( float *p, u32 size, float max_error  )
{
	const u32 from = 0;
	const u32 to = size/100;
	//const u32 sz = size;
	const u32 sz =  to -  from;
	
	vector <double> v;
	v.resize( sz );
	
	float lmin = infinity, lmax = -infinity;
	for(u32 i = 0; i< sz; ++i)
	{
		v[i] = double(p[i]);
		lmin = min( lmin, p[i] );
		lmax = max( lmax, p[i] );
	}

	const float dif = lmax - lmin;

	if(dif==0.f)
	{
		LOG_DEBUG( "empty" );
		return;
	}

	//float max_ratio_error = max_error/dif;
	
	//const float min_max_error = epsilon_7;
	//const float max_max_error = 0.01f;
	//float max_error = max( dif * max_ratio_error, min_max_error ) ;
	//max_error = min( max_max_error, max_error ) ;
	
	int 		im		= sz;//sz;//30;//sz	
	int 		idim	= 1;	
	int 		ipar	= 2;//2:uniform;//3 : Parametrization given by epar.
	double		eeps[]	= {max_error};
	double		epar[]	= {0,1,2,3,4,5,6,7,8,9};
	int			ilend	=0;		
	int			irend	=0;	
	int			iopen	=1;//-1;//0;	//1
	double		afctol	=0.01;
	int			itmax	=1000000000;		
	int			ik		=4;	

	SISLCurve*  curve = 0;
	double		emxerr[] = {-1,0,0};
	int			jstat	 = -1;

	s1961( v.m_p, im, idim, ipar, epar, eeps, ilend, irend, iopen, afctol, itmax,
		ik, &curve, emxerr, &jstat);
	
	
	bi_spline_data<float> d;
	sisl_curve_to_bi_spline_data<float,1>( *curve, d );
	poly_curve< poly_curve_order3_domain<float,1> > spline;

	spline.create( d , 0 );

	float max_delta = -xray::math::infinity;

	for( u32 i = 0; i < sz  ; ++i )
	{
		float s= spline.value( float( i ) );
		float point = float( p[i] );
		float d = xray::math::abs( s -point  );
		max_delta = max( max_delta, d );
	}

	

	//LOG_DEBUG( "max_error: in %f out %f, sisle out %f, in size %d, out size %d ", max_error, max_delta, emxerr[0], sz, spline.num_points() );

	LOG_DEBUG( "max_error: in %f out %f, sisle out %f, in size %d, out size %d, range: %f ", max_error, max_delta, emxerr[0], sz, spline.num_points(), dif );

	freeCurve( curve );
}

*/
