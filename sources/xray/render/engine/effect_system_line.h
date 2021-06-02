////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SYSTEM_LINE_H_INCLUDED
#define EFFECT_SYSTEM_LINE_H_INCLUDED

#include "xray/render/core/effect_descriptor.h"

namespace xray {
namespace render_dx10 {

class effect_system_line: public effect
{
public:
	effect_system_line () : effect() {}
	~effect_system_line() {}

	enum techniques{ z_enabled, z_disabled };

	virtual void compile( effect_compiler& compiler, const effect_compilation_options& options);

private:

};

} // namespace render_dx10
} // namespace xray

#endif // #ifndef EFFECT_SYSTEM_LINE_H_INCLUDED