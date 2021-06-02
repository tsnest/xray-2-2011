////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_COLOR_H_INCLUDED
#define XRAY_MATH_COLOR_H_INCLUDED

namespace xray {
namespace math {
/*
	class color;
	class float3;

	class color_pod {
	public:
		typedef float		type;

	public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
		union {
			struct {
				type		r;
				type		g;
				type		b;
				type		a;
			};
			float			elements[4];
		};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

	public:
		inline	color&		operator -		( );

		inline	color&		operator +=		( color_pod const& other );
		inline	color&		operator +=		( type value );

		inline	color&		operator -=		( color_pod const& other );
		inline	color&		operator -=		( type value );

		inline	color&		operator *=		( type value );
		inline	color&		operator /=		( type value );

		inline	float3&		rgb				( );
		inline	float3 const& rgb			( ) const;

		inline	type&		operator [ ]	( int index );
		inline	const type&	operator [ ]	( int index )const;
		inline	color&		set				( type vr, type vg, type vb, type va );

		inline	color&		normalize		( );
		inline	type		normalize_r		( ); // returns old length

		inline	color&		normalize_safe	( color_pod const& result_in_case_of_zero );
		inline	type		normalize_safe_r( color_pod const& result_in_case_of_zero ); // returns old length

		inline	color&		abs				( );

		inline	color&		min				( color_pod const& other );
		inline	color&		max				( color_pod const& other );

		inline	type		length		( ) const;
		inline	type		squared_length( ) const;

		inline	bool		similar			( color_pod const& other, float epsilon = epsilon_5 ) const;

		inline	bool		valid			( ) const;
	}; // struct color_pod

	class color : public color_pod {
	public:
		inline				color			( );
		inline				color			( type r, type g, type b, type a = 1.f);
		inline				color			( float3 const& rgb, type a );
		inline				color			( color_pod const& other );
		inline				color			( float4 const& other );
		inline				color			( u32 r, u32 g, u32 b, u32 a = 0xff );
		inline				color			( u32 argb );
		inline u32			get_d3dcolor	( ) const;
	}; // class color

	inline color			operator +		( color_pod const& left, color_pod const& right );
	inline color			operator +		( color_pod const& left, color_pod::type const& right );
	inline color			operator -		( color_pod const& left, color_pod const& right );
	inline color			operator -		( color_pod const& left, color_pod::type const& right );
	inline color			operator *		( color_pod const& left, color_pod::type const& right );
	inline color			operator *		( color_pod::type const& left, color_pod const& right );
	inline color			operator /		( color_pod const& left, color_pod::type const& right );
	inline bool				operator <		( color_pod const& left, color_pod const& right );
	inline bool				operator <=		( color_pod const& left, color_pod const& right );
	inline bool				operator >		( color_pod const& left, color_pod const& right );
	inline bool				operator >=		( color_pod const& left, color_pod const& right );
	inline bool				operator !=		( color_pod const& left, color_pod const& right );
	inline bool				operator ==		( color_pod const& left, color_pod const& right );
	inline color			normalize		( color_pod const& object );
	inline color			normalize_safe	( color_pod const& object, color_pod const& result_in_case_of_zero = color( 0.f, 0.f, 0.f, 1.f ) );
*/
	inline u32	color_rgba	(u32 r, u32 g, u32 b, u32 a)	{	return ((a&0xff)<<24)|((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff);	}
	inline u32	color_xrgb	(u32 r, u32 g, u32 b)			{	return color_rgba(r, g, b, 255);	}

	inline u32	color_rgba	(float r, float g, float b, float a) 
	{ return color_rgba( u32(math::floor(r*255) & 0xff), u32(math::floor(g*255) & 0xff), u32(math::floor(b*255) & 0xff), u32(math::floor(a*255) & 0xff) ); }

	inline u32	color_get_A		( u32 c )	{ return (((c) >> 24) & 0xff);	}
	inline u32	color_get_R		( u32 c )	{ return (((c) >> 16) & 0xff);	}
	inline u32	color_get_G		( u32 c )	{ return (((c) >> 8)  & 0xff);	}
	inline u32	color_get_B		( u32 c )	{ return ((c) & 0xff); }

	class color
	{
	public:
		inline		color		( ):m_value(0xffffffff){}
		
		inline	explicit color	( u32 value ):m_value(value){} 
		inline			 color	( u32 r, u32 g, u32 b, u32 a ){ set(r,g,b,a); } 
		inline			 color	( u32 r, u32 g, u32 b ){ set(r,g,b,255); } 
		inline			 color	( int r, int g, int b, int a ){ set(r,g,b,a); } 
		inline			 color	( float3 const& color ){ m_value = color_rgba(color.x,color.y,color.z,1.f); } 
		inline			 color	( float r, float g, float b, float a ){ m_value = color_rgba(r,g,b,a); } 

		inline void	set			( u32 r, u32 g, u32 b, u32 a )	{ m_value = color_rgba(r,g,b,a); }
		inline color operator *	( float const intensity ) const { return color( math::floor(r*intensity), math::floor(g*intensity), math::floor(b*intensity), math::floor(a*intensity)); }

		inline u32	get_A		( )	const { return color_get_A(m_value);	}
		inline u32	get_R		( )	const { return color_get_R(m_value);	}
		inline u32	get_G		( )	const { return color_get_G(m_value);	}
		inline u32	get_B		( )	const { return color_get_B(m_value);	}

		inline float	get_Af		( )	const { return color_get_A(m_value)/255.0f;	}
		inline float	get_Rf		( )	const { return color_get_R(m_value)/255.0f;	}
		inline float	get_Gf		( )	const { return color_get_G(m_value)/255.0f;	}
		inline float	get_Bf		( )	const { return color_get_B(m_value)/255.0f;	}

		inline void get_RGBA	( u32& r, u32& g, u32& b, u32& a){ r=get_R(); g=get_G(); b=get_B(); a=get_A(); }
		inline void get_RGBA	( float& r, float& g, float& b, float& a){ r=get_R()/255.0f; g=get_G()/255.0f; b=get_B()/255.0f; a=get_A()/255.0f; }

		inline void set_A		( u32 val ) { a=val&0xff;}
		inline void set_R		( u32 val ) { r=val&0xff;}
		inline void set_G		( u32 val ) { g=val&0xff;}
		inline void set_B		( u32 val ) { b=val&0xff;}

	#if defined(_MSC_VER)
	#	pragma warning(push)
	#	pragma warning(disable:4201)
	#endif // #if defined(_MSC_VER)
		union{
			struct{
				u8	b;
				u8	g;
				u8	r;
				u8	a;
			};
			u32				m_value;
		};
	#if defined(_MSC_VER)
	#	pragma warning(pop)
	#endif // #if defined(_MSC_VER)
	};// class color

} // namespace math
} // namespace xray

//#include <xray/math_color_inline.h>

#endif // #ifndef XRAY_MATH_COLOR_H_INCLUDED