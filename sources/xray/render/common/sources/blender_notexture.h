////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_NOTEXTURE_H_INCLUDED
#define BLENDER_NOTEXTURE_H_INCLUDED

#include "blender_deffer_base.h"

namespace xray {
namespace render {


class blender_notexture: public blender
{
public:
	blender_notexture():blender(){}
	~blender_notexture(){}

	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);

private:

}; // class blender_notexture

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_NOTEXTURE_H_INCLUDED