////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_PRE_LIGHTING_H_INCLUDED
#define STAGE_PRE_LIGHTING_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render{

class stage_pre_lighting: public stage
{
public:
	stage_pre_lighting	( scene_context* context): stage( context){}
	~stage_pre_lighting	( ){}

	void execute( );

}; // class stage_pre_lighting

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_PRE_LIGHTING_H_INCLUDED