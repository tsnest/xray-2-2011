////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_EFFECT_MANAGER_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_EFFECT_MANAGER_INLINE_H_INCLUDED

namespace xray {
namespace render {

template<class effect_descriptor_class> 
inline ref_effect effect_manager::create_effect()
{
	pcvoid					  mem	= ALLOCA(Kb);
	effect_options_descriptor empty_desc(mem, Kb);
	
	return create_effect<effect_descriptor_class>(empty_desc);
}

template<class effect_descriptor_class> 
inline ref_effect effect_manager::create_effect(render::effect_options_descriptor const& desc)
{
	static effect_descriptor_class	descriptor_object;
	u32 crc							= 0;
	return create_new_effect		(
		descriptor_object, 
		render::create_custom_config(desc, crc, false), 
		desc.get_crc()
	);
}

template<class effect_descriptor_class> 
inline void effect_manager::create_effect(ref_effect* out_effect, render::effect_options_descriptor const& desc)
{
	static effect_descriptor_class	descriptor_object;
	u32 crc							= desc.get_crc();
	if (force_sync)
	{
		*out_effect					= create_new_effect(
			descriptor_object, 
			render::create_custom_config(desc, crc, false), 
			crc
		);
	}else
	{
		create_new_effect	( out_effect, 
							&descriptor_object, 
							render::create_custom_config(desc, crc, false), 
							crc );
	}
}

template<class effect_descriptor_class> 
inline void effect_manager::create_effect(effect_loader* loader, render::effect_options_descriptor const& desc)
{
	static effect_descriptor_class	descriptor_object;
	u32 crc							= desc.get_crc();
	create_new_effect	( loader, 
						&descriptor_object, 
						render::create_custom_config(desc, crc, false), 
						crc );
}

template<class effect_descriptor_class> 
inline void effect_manager::create_effect(ref_effect* out_effect)
{
	static effect_descriptor_class	descriptor_object;
	
	pcvoid					  mem	= ALLOCA(Kb);
	effect_options_descriptor empty_desc(mem, Kb);
	
	u32 crc							= 0;
	
	if (force_sync)
		*out_effect					= create_new_effect(
			descriptor_object, 
			render::create_custom_config(empty_desc, crc, false), 
			crc
		);
	else
		create_new_effect				(out_effect, &descriptor_object, render::create_custom_config(empty_desc, crc, false), crc);
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_EFFECT_MANAGER_INLINE_H_INCLUDED