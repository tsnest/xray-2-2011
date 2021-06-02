////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_TEXTURE_SLOT_H_INCLUDED
#define XRAY_RENDER_CORE_TEXTURE_SLOT_H_INCLUDED

#include <xray/render/core/res_common.h>
#include <xray/render/core/res_texture.h>

namespace xray {
namespace render {

class res_texture;
class resource_intrusive_base;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class texture_slot
{
public:
	inline texture_slot(): slot_id( enum_slot_ind_null), texture( NULL){}

	name_string_type	name;
	u32					slot_id;
	ref_texture			texture;
};

typedef		fixed_vector<texture_slot,D3D_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT>	texture_slots;

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_TEXTURE_SLOT_H_INCLUDED