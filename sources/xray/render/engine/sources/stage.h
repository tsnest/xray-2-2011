////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include <boost/noncopyable.hpp>

namespace xray {
namespace render {

// This will temporary live here 
const u32 stencil_material_mask			= 0x03;
const u32 stencil_light_marker_shift	= 0x02;

class renderer_context;

class stage : boost::noncopyable
{
public:
				 stage				(renderer_context* context);
	virtual		 ~stage				() {}
	virtual void  execute			();
	virtual void  execute_disabled	() {}
	virtual	void  debug_render		() {}
	virtual pcstr stage_name		() const { return ""; };
			void  set_enabled		(bool enable);
	virtual bool  is_enabled		() const;
	
	// not sure if this need to live here
	static void  rm_near			();
	static void  rm_normal			();
	static void  rm_far				();
	
	// This need to be moved some utility class or namespace
		   void  u_compute_texgen_screen(float4x4& m_Texgen);
		   void  u_compute_texgen_jitter(float4x4& m_Texgen_J);
	
protected:
	renderer_context*				m_context;
	bool							m_enabled;
}; // class stage

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_H_INCLUDED