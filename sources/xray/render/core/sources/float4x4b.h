////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FLOAT4X4B_H_INCLUDED
#define FLOAT4X4B_H_INCLUDED

namespace xray {
namespace render_dx10 {

class float4x4b
{
public:
	float4x4b( float4x4b const & other ): m_matrix( other)	{ update_transpose(); }
	float4x4b( math::float4x4 const & other ): m_matrix( other)	{ update_transpose(); }

	operator math::float4x4 const &		() const		{ return m_matrix; }
	math::float4x4 const & get			() const		{ return m_matrix;}


	void operator *=	( math::float4x4::type value )	{ m_matrix *= value; update_transpose();}
	void operator /=	( math::float4x4::type value )	{ m_matrix /= value; update_transpose();}
	void operator =		( math::float4x4 const & value )	{ m_matrix = value; update_transpose();}
	
	bool			try_invert		( math::float4x4 const& other )	{ bool res = m_matrix.try_invert(other); update_transpose(); return res;}
	math::float4x4&		identity		( )							{ m_matrix.identity(); update_transpose(); return m_matrix;}

	void		mul4x4			( math::float4x4 const & left, math::float4x4 const & right)	{ *this = math::mul4x4( left, right); update_transpose(); }
	void		mul4x4			( float4x4b const & left, float4x4b const & right)	{ *this = math::mul4x4( left, right); update_transpose(); }
	void		mul4x3			( math::float4x4 const & left, math::float4x4 const & right)	{ *this = math::mul4x3( left, right); update_transpose(); }
	void		mul4x3			( float4x4b const & left, float4x4b const & right)	{ *this = math::mul4x3( left, right); update_transpose(); }

	math::float4x4 const& transpose() const	{ return m_transpose;}
	

private:

	void update_transpose() 
	{ 
		m_transpose = math::transpose(m_matrix);
	}

private:

	math::float4x4 m_transpose;
	math::float4x4 m_matrix;

}; // class float4x4b

} // namespace render
} // namespace xray

#endif // #ifndef FLOAT4X4B_H_INCLUDED