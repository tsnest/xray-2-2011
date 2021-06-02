////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_DESCRIPTOR_H_INCLUDED
#define EFFECT_DESCRIPTOR_H_INCLUDED

#include <xray/render/core/utils.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/shader_configuration.h>

namespace xray {
namespace render {

typedef u64		class_id;
#define MK_CLSID(a,b,c,d,e,f,g,h) \
class_id((class_id(a)<<class_id(56))|(class_id(b)<<class_id(48))|(class_id(c)<<class_id(40))|(class_id(d)<<class_id(32))|(class_id(e)<<class_id(24))|(class_id(f)<<class_id(16))|(class_id(g)<<class_id(8))|(class_id(h)))

class effect_compiler;
//class custom_config_value;

class effect_descriptor
{
public:
	effect_descriptor				();
	virtual ~effect_descriptor		();
	
	virtual	void compile			(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_descriptor

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_DESCRIPTOR_H_INCLUDED