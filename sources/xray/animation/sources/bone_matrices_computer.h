////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BONE_MATRICES_COMPUTER_H_INCLUDED
#define BONE_MATRICES_COMPUTER_H_INCLUDED

namespace xray {
namespace animation {

namespace mixing {
	struct animation_state;
} // namespace mixing

class skeleton;
class skeleton_bone;
struct bone_transform;
enum animation_types_enum;
class bone_animation;

class bone_matrices_computer : private boost::noncopyable {
public:
						bone_matrices_computer			(
							skeleton const* skeleton,
							mixing::animation_state* animations,
							u32 const animations_count
						);
						~bone_matrices_computer			( );
			void		compute_bones_matrices			( float4x4* const begin, float4x4* const end ) const;
			float4x4	get_object_transform			( ) const;
#ifndef MASTER_GOLD
			float4x4	computed_bone_matrix			( skeleton_bone const& bone ) const;
#endif // #ifndef MASTER_GOLD

private:
		bone_transform	computed_local_bone_transform	( skeleton_bone const& bone, u32 animation_layer_id ) const;
			float4x4	computed_local_bone_matrix		( skeleton_bone const& bone ) const;
			void		compute_skeleton_branch			( skeleton_bone const& bone, float4x4* const result, float4x4 const& parent ) const;


private:
	skeleton const*					m_skeleton;
	mixing::animation_state* const	m_animations;
	u32 const						m_animations_count;
	u32								m_layers_count;
}; // class bone_matrices_computer

} // namespace animation
} // namespace xray

#endif // #ifndef BONE_MATRICES_COMPUTER_H_INCLUDED