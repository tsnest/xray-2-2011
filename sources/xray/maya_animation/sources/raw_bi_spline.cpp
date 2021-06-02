////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/maya_animation/raw_bi_spline.h>
#include "maya_animation_world.h"
#include "sisl/sisl.h"

void s1961(
			sisl_double ep[],	//Array (length idim*im) containing the points to be
							//approximated
			int im,			//The no. of data points.
			int idim,		//The dimension of the euclidean space in which the
							//data points lie, i.e. the number of components of
							//each data point.
			int ipar,		//Flag indicating the type of parameterization to be
							//used:
							//= 1 : Paramterize by accumulated cord length.
							//(Arc length parametrization for the piecewise
							//linear interpolant.)
							//= 2 : Uniform parameterization.
							//= 3 : Parametrization given by epar.
							//If ipar < 1 or ipar > 3, it will be set to 1.

			sisl_double epar[],	//Array (length im) containing a parametrization of the
							//given data.
			sisl_double eeps[],	//Array (length idim) containing the tolerance to be
							//used during the data reduction stage. The nal ap-
							//proximation to the data will deviate less than eeps
							//from the piecewise linear interpolant in each of the
							//idim components.
			int ilend,		//The no. of derivatives that are not allowed to change
							//at the left end of the curve. The 0(ilend 1)
							//derivatives willbekeptxed. Ifilend< 0, thisroutine
							//will set it to 0. If ilend < ik, this routine will set it
							//to ik.
			int irend,		//The no. of derivatives that are not allowed to change
							//at the right end of the curve. The 0(irend
							//1) derivatives will be kept xed. If irend < 0, this
							//routine will set it to 0. If irend < ik, this routine will
							//set it to ik.

			int iopen,		//Open/closed parameter
							//= 1 : Produce open curve.
							//= 0 : Produce closed, non-periodic curve if possible.
							//=1 : Produce closed, periodic curve if possible.
							//If a closed or periodic curve is to be produced and the
							//start- and endpoint is more distant than the length of
							//the tolerance, a new point is added. Note that if the
							//parametrization is given as input, the parametriza-
							//tion if the last point will be arbitrary.
			sisl_double afctol,	//Number indicating how the tolerance is to be shared
							//between the two data reduction stages. For the linear
							//reduction, a tolerance of afctol eeps will be used,
							//while a tolerance of(1afctol)eeps willbe used dur-
							//ing the final data reduction. (Similarly for edgeps.)

			int itmax,		//Max. no. of iterations in the data-reduction routine.
			int ik,			//The polynomial order of the approximation.
			//Output Arguments:
			SISLCurve **rc, //Pointer to curve.
			sisl_double emxerr[],//Array (length idim) (allocated outside this routine.)
							//containing for each component an upper bound on
							//the max. deviation of the nal approximation from
							//the initial piecewise linear interpolant.
			int *jstat		//Status messages
							//> 0 : Warning.
							//= 0 : Ok.
							//< 0 : Error.
			);


void s1227(
SISLCurve *curve,	//Pointer to the curve for which position and derivatives
					//are to be computed.
int der,			//The number of derivatives to compute.
					//< 0 : Error.
					//= 0 : Compute position.
					//= 1 : Compute position and derivative.
sisl_double parvalue,	//The parameter value at which to compute position
					//and derivatives.
int *leftknot,		//Pointer to the interval in the knot vector where par-
					//value is located. Ifet[] is the knotvector, the relation:
					//et[leftknot] < parvalue et[leftknot+1]
					//should hold. (If parvalue et[ik1]) then leftknot
					//should be ik-1. Here ik is the order of the curve.)
					//Ifleftknotdoesnothave therightvaluewhenentering
					//the routine, its value will be changed to the value
					//satisfying the above condition.
sisl_double derive[],	//Double array of dimension (der +1)*dim contain-
					//ing the position and derivative vectors. (dim is the
					//dimension of the Euclidean space in which the curve
					//lies.) These vectors are stored in the following order:
					//first the components of the position vector, then the
					//dim components of the tangent vector, then the dim
					//components of the second derivative vector, and so
					//on. (The C declaration of derive as a two dimensional
					//array would therefore be derive[der +1][dim].)
int *stat			//Status messages
					//> 0 : warning
					//= 0 : ok
					//< 0 : error
);

namespace xray {
namespace maya_animation {

typedef raw_bi_spline< float, 1 > bi_spline_1d_impl;
typedef raw_bi_spline< float3, 3 > bi_spline_3d_impl;

raw_bi_spline<float,1>	*maya_animation_world::create_1d_bi_spline( )const
{
	return NEW( bi_spline_1d_impl )();
}

raw_bi_spline<float3,3>	*maya_animation_world::create_3d_bi_spline( )const
{
		return NEW( bi_spline_3d_impl )();
}

void	maya_animation_world::destroy( raw_bi_spline< float, 1 >	* &p )const
{
	DELETE( p );
}

void	maya_animation_world::destroy(  raw_bi_spline< float3, 3 >	* &p )const
{
	DELETE( p );
}



template< typename Point_type, int Dimension >
inline Point_type point( const sisl_double *dp )
{
	ASSERT( dp );
	Point_type p;
	for(int i = 0; i < Dimension; ++i )
		p[i] = float ( dp[ i ]);
	return p;
}

static const float invalide_bi_spline_float_value = -1000000000.f;

template <>
inline float point < float,1 >( const sisl_double *dp )
{
	ASSERT( dp );
	return float( dp[0] );
}

template< typename Point_type, int Dimension >
inline Point_type invalide_point( )
{
	Point_type p;
	for(int i = 0; i < Dimension; ++i )
		p[i] = invalide_bi_spline_float_value;
	return p;
}

template <>
inline float invalide_point< float,1 >( )
{
	return float( invalide_bi_spline_float_value );
}

template< typename Point_type, int Dimension >
void to_array( const Point_type &p, sisl_double double_array[  ] )
{
	//sisl_double double_array[ Dimension ];
	for( u32 i =0 ; i < Dimension ; ++i)
		double_array[i] = p[i];
	
}

template<>
void to_array< float,1 >( const float &p, sisl_double double_array[  ] )
{
	double_array[ 0 ] =  p ;
}

template< typename Point_type, int Dimension >
raw_bi_spline< Point_type, Dimension >::~raw_bi_spline()
{
	if( m_curve )
		freeCurve( m_curve );
	m_curve = 0;
}

template< typename Point_type, int Dimension >
void raw_bi_spline< Point_type, Dimension >::evaluate( float time, Point_type &point_ ) const
{
		const int der = 0;
		sisl_double derive[(der + 1) * Dimension];
		int stat = -1; 
		int leftknot = -1;
		s1227(m_curve, der, sisl_double( time  ), &leftknot, derive, &stat);
		point_ = point<Point_type,Dimension>( derive );
#pragma message( XRAY_TODO( "check stat" ) )
}

template< typename Point_type, int Dimension >
void raw_bi_spline< Point_type, Dimension >::get_data	( animation::bi_spline_data<Point_type> &data ) const
{
	for( int i = 0; i < m_curve->in ; ++i )//+ m_curve->ik
	{
		std::pair< float, Point_type > p;
		p.first = float(m_curve->et[i]);
		p.second = point<Point_type,Dimension>( m_curve->ecoef + i*Dimension );
		data.add_point( p );
	}
		std::pair< float, Point_type > p;
		p.first = float(m_curve->et[m_curve->in]);
		p.second =invalide_point<Point_type,Dimension>();
		data.add_point( p );
	;
}

template< typename Point_type, int Dimension >
void raw_bi_spline< Point_type, Dimension >::build_approximation( const float* points, u32 number, const Point_type &max_epsilon  )
{
	ASSERT( number > 0 );
	int 		im		= number;//30;//sz	
	int 		idim	= Dimension;	
	int 		ipar	= 2;//2:uniform;//3 : Parametrization given by epar.
	sisl_double	epar[]	= { 0 };//10

	sisl_double		eeps[Dimension]	; //to_array( max_epsilon ); //{ 0.0005, 0.0005, 0.0005 };
	to_array<Point_type,Dimension>( max_epsilon, eeps );

	int			ilend	=0;		
	int			irend	=0;	
	int			iopen	=1;//-1;//0;//1
	sisl_double	afctol	=sisl_double(0.01);
	int			itmax	=1000000000;		
	int			ik		=4;//bi_spline order; poly order = ik-1	

	//Output Arguments

	sisl_double		emxerr[] = {-1,-1,-1};
	int			jstat	 = -1;

	if( m_curve )
		freeCurve( m_curve );
	m_curve = 0;

	sisl_double * p = (sisl_double*) ALLOCA( Dimension * number * sizeof( sisl_double ) );

	for( u32 i = 0; i <  Dimension * number ; ++i )
		p[i] = points[i];

	s1961( p, im, idim, ipar, epar, eeps, ilend, irend, iopen, afctol, itmax,
		ik, &m_curve, emxerr, &jstat);
	ASSERT( m_curve );
}

template class XRAY_MAYA_ANIMATION_API raw_bi_spline< float, 1 >;
template class XRAY_MAYA_ANIMATION_API raw_bi_spline< float3, 3 >;

} // namespace maya_animation
} // namespace xray