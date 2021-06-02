////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SHADER_NODE_H_INCLUDED
#define XRAY_SHADER_NODE_H_INCLUDED

class xray_shader_node : public MPxNode
{
public:
					xray_shader_node			( );
	virtual			~xray_shader_node		( );

	static  void *	creator			( );
	virtual MStatus	compute			( const MPlug&, MDataBlock& );
	static  MStatus	initialize		( );

	virtual void	postConstructor(); 

    static const MTypeId	typeId;
    static const MString	typeName;

protected:
	static MObject		aXrayShaderName;
	static MObject		aXrayDoubleSided;
   // components of surface color
   static MObject  aColorR;
   static MObject  aColorG;
   static MObject  aColorB;
   // Surface color
   static MObject  aColor;

   // components of surface transparency
   static MObject  aInTransR;
   static MObject  aInTransG;
   static MObject  aInTransB;
   // Surface transparency
   static MObject  aInTransparency;

   // components of output color
   static MObject  aOutColorR;
   static MObject  aOutColorG;
   static MObject  aOutColorB;
   // Output color
   static MObject  aOutColor;

   // components of output transparency
   static MObject  aOutTransR;
   static MObject  aOutTransG;
   static MObject  aOutTransB;
   // Output transparency
   static MObject  aOutTransparency;

   // components of surface normal
   static MObject  aNormalCameraX;
   static MObject  aNormalCameraY;
   static MObject  aNormalCameraZ;
   // Surface normal
   static MObject  aNormalCamera;

   // components of light direction vector
   static MObject  aLightDirectionX;
   static MObject  aLightDirectionY;
   static MObject  aLightDirectionZ;
   // Light direction vector
   static MObject  aLightDirection;


   // components of light intensity
   static MObject  aLightIntensityR;
   static MObject  aLightIntensityG;
   static MObject  aLightIntensityB;
   // Light Intensity vector
   static MObject  aLightIntensity;

   
   static MObject  aLightAmbient;// Ambient flag
   static MObject  aLightDiffuse;// Diffuse flag
   static MObject  aLightSpecular;// Specular flag
   static MObject	aLightShadowFraction;// Shadow Fraction flag
   static MObject	aPreShadowIntensity;// Pre Shadow Intensity
   static MObject	aLightBlindData;   // Light blind data
   static MObject	aLightData;// Light data array
 
}; //  class lambert

#endif // #ifndef XRAY_SHADER_NODE_H_INCLUDED