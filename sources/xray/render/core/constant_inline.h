////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace render_dx10 {


constant_host::constant_host (shared_string name): 
name	(name),
type	(rc_INVALID)
{
	vs_slot.reset();
	ps_slot.reset();
	ZeroMemory( update_markers, sizeof(update_markers));
}

} // namespace render
} // namespace xray
