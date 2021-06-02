////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXER_H_INCLUDED
#define MIXER_H_INCLUDED

#include "mixing_n_ary_tree.h"

namespace xray {

namespace render {
namespace debug {
	struct renderer;
} // namespace render
} // namespace xray

namespace animation {

class bone_mixer;

namespace mixing {
	class expression;
	struct n_ary_tree_base_node;
} // namespace mixing

class mixer : private boost::noncopyable {
private:
	enum {
		stack_buffer_size_base			= 2*4096,
	};

public:
	enum {
#if XRAY_PLATFORM_WINDOWS_64
		stack_buffer_size				= 2*stack_buffer_size_base,
#else // #if XRAY_PLATFORM_WINDOWS_64
		stack_buffer_size				= stack_buffer_size_base,
#endif // #if XRAY_PLATFORM_WINDOWS_64
	};

public:
	explicit inline	mixer				( bone_mixer& bone_mixer, memory::base_allocator& allocator  );
			void	set_target			( mixing::expression const& expression, u32 const current_time_in_ms );
			void	tick				( u32 current_time_in_ms );

private:
	inline	void	make_inactual		( );
			void	set_target_impl		( mixing::expression const& expression, u32 const current_time_in_ms );
			void	fill_bone_mixer_data( u32 const current_time_in_ms );

public:
			void	render				( render::debug::renderer& renderer, float4x4 const& matrix, float time );

private:
	mixing::n_ary_tree		m_mixing_tree;
	memory::base_allocator&	m_allocator;
	bone_mixer&				m_bone_mixer;
	u32						m_last_actual_time_ms;
}; // class mixer

class skeleton_animation;
class skeleton;

void test_mixer	( skeleton_animation const& skeleton_animation, skeleton const& skeleton );

} // namespace animation
} // namespace xray

#include "mixer_inline.h"

#endif // #ifndef MIXER_H_INCLUDED