////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_DRAWABLE_H_INCLUDED
#define TRANSFORM_DRAWABLE_H_INCLUDED

class transform_drawable {
public:
virtual void draw(M3dView& view, const MDagPath& path, 
		M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor colorRGB ) = 0;

private:

}; // class transform_drawable

#endif // #ifndef TRANSFORM_DRAWABLE_H_INCLUDED