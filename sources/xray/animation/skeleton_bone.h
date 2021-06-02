////////////////////////////////////////////////////////////////////////////
//	Created		: 11.08.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_SKELETON_BONE_H_INCLUDED
#define XRAY_ANIMATION_SKELETON_BONE_H_INCLUDED

namespace xray {
namespace animation {

class skeleton_bone : private boost::noncopyable {
public:
									skeleton_bone	(
										pcstr const id,
										skeleton_bone const* const parent,
										skeleton_bone const* const children_begin,
										skeleton_bone const* const children_end
									);

	inline	pcstr const				id				( ) const { return m_id; }
	inline	skeleton_bone const*	parent			( ) const { return m_parent; }
	inline	skeleton_bone const*	children_begin	( ) const { return m_children_begin; }
	inline	skeleton_bone const*	children_end	( ) const { return m_children_end; }

private:
	pcstr const						m_id;
	skeleton_bone const* const		m_parent;
	skeleton_bone const* const		m_children_begin;
	skeleton_bone const* const		m_children_end;
}; // class skeleton_bone

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_SKELETON_BONE_H_INCLUDED