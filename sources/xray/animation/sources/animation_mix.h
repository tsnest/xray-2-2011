////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_MIX_H_INCLUDED
#define ANIMATION_MIX_H_INCLUDED

#include "animation_layer.h"
#include "blend_function.h"

namespace xray {

namespace render {
namespace debug {
	struct renderer;
} // namespace debug
} // namespace render

namespace animation {

class		skeleton_animation;
class		skeleton;

class animation_mix:
	private boost::noncopyable
{
private:

public:
						animation_mix		( const skeleton& skel, pvoid buffer, u32 max_count );
						animation_mix		( const skeleton& skel );
						~animation_mix		( );
public:		
	inline	void		add					( const animation_layer &blend );




	inline	u32			bone_count			( ) const { return m_skeleton.bone_count(); }
			void		bone_matrix			( bone_index_type bone, float time, math::float4x4 &matrix )const;
			void		calculate_bones		( float time, buffer_vector< float4x4 > &matrices )const;
			//float		max_time			( )const;

			void		clear				( );
			void		insert				( const animation_vector_type &animations );
			u32			animations_count	( )const { return m_blend_vector.size(); }
			
			void		get_weights			( buffer_vector< float > &weights )const;

			void		reset_start_time	( );

public:
	inline	void		set_root_start		( const frame &f );

public:
			void		render				( render::debug::renderer	&renderer, const float4x4 &matrix, float time );
inline	skeleton const&	get_skeleton		() const { return m_skeleton; }



private:
	inline	void		get_bone_matrix		( float4x4 &m, bone_index_type bone, float time, const float3 &parent_scale_, float3 &scale_ )const;

private:
	animation_vector_type					m_blend_vector;
	
	skeleton			const&				m_skeleton;

	const				bool				m_external_buffer;



#ifdef	DEBUG

private:
	typedef	deque< std::pair<float3, float> >	dbg_times;

private:
	static void	render_animation_time_line( render::debug::renderer	&r, const float3 &p, dbg_times &times, float anim_time );

private:
	vector< dbg_times >							m_dbg_times;

#endif

}; // class animation_mix

inline void get_bone_matrix( float4x4 &m, const animation_vector_type &blends,  bone_index_type bone, float time, const float3 &parent_scale_, float3 &scale_ );

}  // namespace animation
}  // namespace xray



#include "animation_mix_inline.h"

#endif // #ifndef ANIMATION_MIX_H_INCLUDED