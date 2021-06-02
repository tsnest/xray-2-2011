////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SYSTEM_LINE_H_INCLUDED
#define EFFECT_SYSTEM_LINE_H_INCLUDED

#include "xray/render/core/effect_descriptor.h"

namespace xray {
namespace render {

class effect_system_line: public effect_descriptor
{
public:
	effect_system_line () : effect_descriptor() {}
	~effect_system_line() {}

	enum techniques{ z_enabled, z_disabled };

	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
	//virtual	void cache	(custom_config_value const& config);
private:

};

} // namespace render
} // namespace xray

#endif // #ifndef EFFECT_SYSTEM_LINE_H_INCLUDED