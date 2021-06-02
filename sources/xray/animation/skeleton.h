////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_SKELETON_H_INCLUDED
#define XRAY_ANIMATION_SKELETON_H_INCLUDED

#include <xray/animation/type_definitions.h>
#include <xray/animation/api.h>
#include <xray/animation/skeleton_bone.h>

namespace xray {
namespace animation {

class skeleton_bone;

class XRAY_ANIMATION_API skeleton : public resources::unmanaged_resource {
public:
	explicit						skeleton		( u32 bones_count );
									~skeleton		( );
			pcstr					get_bone_name	( bone_index_type index ) const;
			bone_index_type			get_bone_index	( pcstr name ) const;
	inline	u32						get_bone_index	( skeleton_bone const& bone ) const;
	inline	u32						get_bones_count	( ) const;
	inline	u32					get_root_bones_count( ) const;
	inline	u32				get_non_root_bones_count( ) const;
	inline	skeleton_bone const&	get_bone		( bone_index_type index ) const;
	inline	skeleton_bone const&	get_root		( ) const;
	inline	bool					is_root_bone	( bone_index_type index ) const;

private:
	inline	skeleton_bone&			get_root		( );


private:
	bone_index_type					m_bones_count;
}; // class skeleton

typedef	resources::resource_ptr<
	skeleton,
	resources::unmanaged_intrusive_base
> skeleton_ptr;

	bone_index_type			skeleton_bone_index	( skeleton const& skeleton, pcstr bone_name );

} // namespace animation
} // namespace xray

#include <xray/animation/skeleton_inline.h>

#endif // #ifndef XRAY_ANIMATION_SKELETON_H_INCLUDED
