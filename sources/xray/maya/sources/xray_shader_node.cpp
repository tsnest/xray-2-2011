////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray_shader_node.h"

const MTypeId xray_shader_node::typeId( shader_node_type_id );
const MString xray_shader_node::typeName( shader_node_type_name );

#define MAKE_INPUT_ATT(attr)			\
	CHK_STAT( attr.setKeyable(true) );  \
	CHK_STAT( attr.setStorable(true) );	\
	CHK_STAT( attr.setReadable(true) );	\
	CHK_STAT( attr.setWritable(true) );

void xray_shader_node::postConstructor( )
{
	setMPSafe( true );
}

MObject xray_shader_node::aXrayShaderName;
MObject xray_shader_node::aXrayDoubleSided;
//MObject  xray_shader_node::aTranslucenceCoeff;
//MObject  xray_shader_node::aDiffuseReflectivity;
MObject  xray_shader_node::aInTransparency;
MObject  xray_shader_node::aInTransR;
MObject  xray_shader_node::aInTransG;
MObject  xray_shader_node::aInTransB;
MObject  xray_shader_node::aColor;
MObject  xray_shader_node::aColorR;
MObject  xray_shader_node::aColorG;
MObject  xray_shader_node::aColorB;
//MObject  xray_shader_node::aIncandescence;
//MObject  xray_shader_node::aIncandescenceR;
//MObject  xray_shader_node::aIncandescenceG;
//MObject  xray_shader_node::aIncandescenceB;
MObject  xray_shader_node::aOutColor;
MObject  xray_shader_node::aOutColorR;
MObject  xray_shader_node::aOutColorG;
MObject  xray_shader_node::aOutColorB;
MObject  xray_shader_node::aOutTransparency;
MObject  xray_shader_node::aOutTransR;
MObject  xray_shader_node::aOutTransG;
MObject  xray_shader_node::aOutTransB;
MObject  xray_shader_node::aNormalCamera;
MObject  xray_shader_node::aNormalCameraX;
MObject  xray_shader_node::aNormalCameraY;
MObject  xray_shader_node::aNormalCameraZ;
MObject  xray_shader_node::aLightData;
MObject  xray_shader_node::aLightDirection;
MObject  xray_shader_node::aLightDirectionX;
MObject  xray_shader_node::aLightDirectionY;
MObject  xray_shader_node::aLightDirectionZ;
MObject  xray_shader_node::aLightIntensity; 
MObject  xray_shader_node::aLightIntensityR;
MObject  xray_shader_node::aLightIntensityG;
MObject  xray_shader_node::aLightIntensityB;
MObject  xray_shader_node::aLightAmbient;
MObject  xray_shader_node::aLightDiffuse;
MObject  xray_shader_node::aLightSpecular;
MObject  xray_shader_node::aLightShadowFraction;
MObject  xray_shader_node::aPreShadowIntensity;
MObject  xray_shader_node::aLightBlindData;


xray_shader_node::xray_shader_node( )
{
}

xray_shader_node::~xray_shader_node( )
{ 
}

void* xray_shader_node::creator()
{
	return CRT_NEW(xray_shader_node)();
}


MStatus xray_shader_node::initialize()
{
	MFnNumericAttribute		nAttr; 
	MFnLightDataAttribute	lAttr;

	MFnEnumAttribute		eAttr;
	MFnTypedAttribute		tAttr;


	MStatus status; 
	aXrayShaderName		= eAttr.create( shader_name_att, "xrs", 0, &status );
	CHK_STAT			( status );
	MAKE_INPUT_ATT		(eAttr);
	CHK_STAT(eAttr.addField		("default", 0));
	CHK_STAT(eAttr.addField		("High quality(not supported)", 1));
	CHK_STAT(eAttr.addField		("Medium quality(not supported)", 2));
	CHK_STAT(eAttr.addField		("Low quality(not supported)", 3));
	CHK_STAT			( eAttr.setDefault( 0 ));

	aXrayDoubleSided	= nAttr.create( doublesided_att_name, "xds", MFnNumericData::kBoolean, 0, &status );
	CHK_STAT			( status );
	MAKE_INPUT_ATT		( nAttr );
	nAttr.setDefault	( false );
	// Input Attributes
	aColorR = nAttr.create( "colorR", "cr",MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );
	CHK_STAT( nAttr.setDefault( 0.0f ) );

	aColorG = nAttr.create( "colorG", "cg", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );
	CHK_STAT( nAttr.setDefault( 0.58824f ) );

	aColorB = nAttr.create( "colorB", "cb",MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );
	CHK_STAT( nAttr.setDefault( 0.644f ) );

	aColor = nAttr.create( "color", "c", aColorR, aColorG, aColorB, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );
	CHK_STAT( nAttr.setDefault( 0.0f, 0.58824f, 0.644f ) );
	CHK_STAT( nAttr.setUsedAsColor( true ) );

	aInTransR = nAttr.create( "transparencyR", "itr", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );

	aInTransG = nAttr.create( "transparencyG", "itg", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );

	aInTransB = nAttr.create( "transparencyB", "itb", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status);
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );

	aInTransparency = nAttr.create( "transparency", "it", aInTransR, aInTransG, aInTransB, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setKeyable( true ) );
	CHK_STAT( nAttr.setStorable( true ) );
	CHK_STAT( nAttr.setDefault( 0.0f, 0.0f, 0.0f ) );
	CHK_STAT( nAttr.setUsedAsColor( true ) );

	
	// Output Attributes

	// Color Output
	aOutColorR = nAttr.create( "outColorR", "ocr", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	
	aOutColorG = nAttr.create( "outColorG", "ocg", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	
	aOutColorB = nAttr.create( "outColorB", "ocb", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	
	aOutColor = nAttr.create( "outColor", "oc", aOutColorR, aOutColorG, aOutColorB, &status );
	CHK_STAT( status );
	
	CHK_STAT( nAttr.setHidden( false ) );
	CHK_STAT( nAttr.setReadable( true ) );
	CHK_STAT( nAttr.setWritable( false ) );

	
	// Transparency Output
	//
	aOutTransR = nAttr.create( "outTransparencyR", "otr", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );

	aOutTransG = nAttr.create( "outTransparencyG", "otg", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );

	aOutTransB = nAttr.create( "outTransparencyB", "otb", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );

	aOutTransparency = nAttr.create( "outTransparency", "ot", aOutTransR, aOutTransG, aOutTransB, &status );
	CHK_STAT( status );

	CHK_STAT( nAttr.setHidden( false ) );
	CHK_STAT( nAttr.setReadable( true ) );
	CHK_STAT( nAttr.setWritable( false ) );


	// Camera Normals
	aNormalCameraX = nAttr.create( "normalCameraX", "nx", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setStorable( false ) );
	CHK_STAT( nAttr.setDefault( 1.0f ) );

	aNormalCameraY = nAttr.create( "normalCameraY", "ny", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setStorable( false ) );
	CHK_STAT( nAttr.setDefault( 1.0f ) );

	aNormalCameraZ = nAttr.create( "normalCameraZ", "nz", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setStorable( false ) );
	CHK_STAT( nAttr.setDefault( 1.0f ) );

	aNormalCamera = nAttr.create( "normalCamera", "n", aNormalCameraX, aNormalCameraY, aNormalCameraZ, &status );
	CHK_STAT( status );
	CHK_STAT( nAttr.setStorable( false ) );
	CHK_STAT( nAttr.setDefault( 1.0f, 1.0f, 1.0f ) );
	CHK_STAT( nAttr.setHidden( true ) );


	// Light Direction
	aLightDirectionX = nAttr.create( "lightDirectionX", "ldx", MFnNumericData::kFloat, 0, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f ) );

	aLightDirectionY = nAttr.create( "lightDirectionY", "ldy", MFnNumericData::kFloat, 0, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f ) );

	aLightDirectionZ = nAttr.create( "lightDirectionZ", "ldz", MFnNumericData::kFloat, 0, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f ) );

	aLightDirection = nAttr.create( "lightDirection", "ld", aLightDirectionX, aLightDirectionY, aLightDirectionZ, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT( nAttr.setDefault( 1.0f, 1.0f, 1.0f ) );


	// Light Intensity
	//
	aLightIntensityR = nAttr.create( "lightIntensityR", "lir", MFnNumericData::kFloat, 0, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f ) );

	aLightIntensityG = nAttr.create( "lightIntensityG", "lig", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT( nAttr.setDefault( 1.0f ) );

	aLightIntensityB = nAttr.create( "lightIntensityB", "lib", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f ) );

	aLightIntensity = nAttr.create( "lightIntensity", "li", aLightIntensityR, aLightIntensityG, aLightIntensityB, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f, 1.0f, 1.0f ) );

   
	// Light
	aLightAmbient = nAttr.create( "lightAmbient", "la", MFnNumericData::kBoolean, 0, &status );
	CHK_STAT ( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( true ) );

	aLightDiffuse = nAttr.create( "lightDiffuse", "ldf", MFnNumericData::kBoolean, 0, &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT( nAttr.setDefault( true ) );

	aLightSpecular = nAttr.create( "lightSpecular", "ls", MFnNumericData::kBoolean, 0, &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT( nAttr.setDefault( false ) );

	aLightShadowFraction = nAttr.create( "lightShadowFraction", "lsf", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT( nAttr.setDefault( 1.0f ) );

	aPreShadowIntensity = nAttr.create( "preShadowIntensity", "psi", MFnNumericData::kFloat, 0, &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );
	CHK_STAT ( nAttr.setDefault( 1.0f ) );

	aLightBlindData = nAttr.createAddr( "lightBlindData", "lbld", &status );
	CHK_STAT( status );
    CHK_STAT ( nAttr.setStorable(false) );
    CHK_STAT ( nAttr.setHidden(true) );
    CHK_STAT ( nAttr.setReadable(true) );
    CHK_STAT ( nAttr.setWritable(false) );

	aLightData = lAttr.create( "lightDataArray", "ltd", aLightDirection,
								aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular, 
								aLightShadowFraction, aPreShadowIntensity, aLightBlindData,
								&status );
	CHK_STAT( status );
	CHK_STAT( lAttr.setArray( true ) );
	CHK_STAT( lAttr.setStorable( false ) );
	CHK_STAT( lAttr.setHidden( true ) );
	CHK_STAT( lAttr.setDefault( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
								true, true, false, 1.0f, 1.0f, NULL ) );


	// Next we will add the attributes we have defined to the node
	CHK_STAT( addAttribute( aColor ) );
	CHK_STAT( addAttribute( aInTransparency ) );
	CHK_STAT( addAttribute( aOutColor ) );
	CHK_STAT( addAttribute( aOutTransparency ) );
	CHK_STAT( addAttribute( aNormalCamera ) );

	// Only add the parent of the compound
	CHK_STAT( addAttribute( aLightData ) );

	CHK_STAT( addAttribute( aXrayShaderName ) );
	CHK_STAT( addAttribute( aXrayDoubleSided ) );

	// The attributeAffects() method is used to indicate when the input
	// attribute affects the output attribute. This knowledge allows Maya
	// to optimize dependencies in the graph in more complex nodes where
	// there may be several inputs and outputs, but not all the inputs
	// affect all the outputs.
	CHK_STAT( attributeAffects( aColorR, aOutColor ) );
	CHK_STAT( attributeAffects( aColorG, aOutColor ) );
	CHK_STAT( attributeAffects( aColorB, aOutColor ) );
	CHK_STAT( attributeAffects( aColor, aOutColor ) );
	CHK_STAT( attributeAffects( aInTransR, aOutTransparency ) );
	CHK_STAT( attributeAffects( aInTransG, aOutTransparency ) );
	CHK_STAT( attributeAffects( aInTransB, aOutTransparency ) );
	CHK_STAT( attributeAffects( aInTransparency, aOutTransparency ) );
	CHK_STAT( attributeAffects( aInTransparency, aOutColor ) );
	CHK_STAT( attributeAffects( aLightIntensityR, aOutColor ) );
	CHK_STAT( attributeAffects( aLightIntensityB, aOutColor ) );
	CHK_STAT( attributeAffects( aLightIntensityG, aOutColor ) );
	CHK_STAT( attributeAffects( aLightIntensity, aOutColor ) );
	CHK_STAT( attributeAffects( aNormalCameraX, aOutColor ) );
	CHK_STAT( attributeAffects( aNormalCameraY, aOutColor ) );
	CHK_STAT( attributeAffects( aNormalCameraZ, aOutColor ) );
	CHK_STAT( attributeAffects( aNormalCamera, aOutColor ) );
	CHK_STAT( attributeAffects( aLightDirectionX, aOutColor ) );
	CHK_STAT( attributeAffects( aLightDirectionY, aOutColor ) );
	CHK_STAT( attributeAffects( aLightDirectionZ, aOutColor ) );
	CHK_STAT( attributeAffects( aLightDirection, aOutColor ) );
	CHK_STAT( attributeAffects( aLightAmbient, aOutColor ) );
	CHK_STAT( attributeAffects( aLightSpecular, aOutColor ) );
	CHK_STAT( attributeAffects( aLightDiffuse, aOutColor ) );
	CHK_STAT( attributeAffects( aLightShadowFraction, aOutColor ) );
	CHK_STAT( attributeAffects( aPreShadowIntensity, aOutColor ) );
	CHK_STAT( attributeAffects( aLightBlindData, aOutColor ) );
	CHK_STAT( attributeAffects( aLightData, aOutColor ) );

	return( MS::kSuccess );
}
