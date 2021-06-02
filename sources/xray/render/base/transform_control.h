////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_H_INCLUDED
#define TRANSFORM_CONTROL_H_INCLUDED

namespace xray {
namespace render {
namespace editor {

class transform_control 
{
public:
	virtual void	draw					( ) = 0;
	virtual void	draw_collision_object	( ) = 0;

	virtual void	set_transform			( const float4x4& transform) = 0;
	virtual const float4x4& get_transform	( ) = 0;

	virtual void	set_view_matrix			( const float4x4& transform) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( transform_control )
};

} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef TRANSFORM_CONTROL_H_INCLUDED