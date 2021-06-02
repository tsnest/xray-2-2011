////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FAKE_DRAWING_LOCATOR_H_INCLUDED
#define FAKE_DRAWING_LOCATOR_H_INCLUDED

class fake_drawing_locator : public MPxLocatorNode
{
public:
    fake_drawing_locator();
    virtual ~fake_drawing_locator(); 

    virtual MStatus compute(const MPlug& plug, MDataBlock& data);
    virtual void draw(M3dView& view, const MDagPath& path, 
		M3dView::DisplayStyle style, M3dView::DisplayStatus status);
    
	virtual bool isBounded() const { return false; }
    
    static void* creator();
    static MStatus initialize();

    static const MTypeId typeId;
    static const MString typeName;

    // Attributes
    static MObject aEnabled;
};

#endif // #ifndef FAKE_DRAWING_LOCATOR_H_INCLUDED