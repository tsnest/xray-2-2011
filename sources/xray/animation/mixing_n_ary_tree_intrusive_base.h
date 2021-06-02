////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_MIXING_N_ARY_TREE_INTRUSIVE_BASE_H_INCLUDED
#define XRAY_ANIMATION_MIXING_N_ARY_TREE_INTRUSIVE_BASE_H_INCLUDED

#include <xray/animation/api.h>

namespace xray {
namespace animation {
namespace mixing {

class XRAY_ANIMATION_API n_ary_tree_intrusive_base {
public:
	inline		n_ary_tree_intrusive_base	( );

	template <typename T>
	static	inline	void	destroy			( T& );

	inline	u32				reference_count	( ) const;

private:
	friend class threading::single_threading_policy;
	u32		m_reference_count;
}; // class n_ary_tree_intrusive_base

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_n_ary_tree_intrusive_base_inline.h>

#endif // #ifndef XRAY_ANIMATION_MIXING_N_ARY_TREE_INTRUSIVE_BASE_H_INCLUDED