////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.01.2008
//	Author		: Konstantin Slipchenko
//	Description : incapsulation of space position
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_POSE_ANCHOR_H_INCLUDED
#define XRAY_PHYSICS_POSE_ANCHOR_H_INCLUDED
struct dxBody;
class pose_anchor
{
	float4x4	m;

public:
inline			float4x4&	matrix	( )						{ return m; }
inline	const	float4x4&	matrix	( )const				{ return m; }

inline	void	set					( const float4x4 &m_ )	{ m = m_; }

}; // class pose_anchor
#endif