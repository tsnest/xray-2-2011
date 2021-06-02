////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray_material.h"
#include "node_identifiers.h"
#include <maya/MRenderUtil.h>

// Static data
const MString xray_material_node::Name	( material_node_type_name );
MTypeId const xray_material_node::id	( material_node_type_id );

// Attributes 
MObject xray_material_node::aColor;
MObject xray_material_node::aTranslucenceCoeff;
MObject xray_material_node::aDiffuseReflectivity;
MObject xray_material_node::aIncandescence;
MObject xray_material_node::aOutColor;
MObject xray_material_node::aPointCamera;
MObject xray_material_node::aNormalCamera;
MObject xray_material_node::aLightData;
MObject xray_material_node::aLightDirection;
MObject xray_material_node::aLightIntensity;
MObject xray_material_node::aLightAmbient;
MObject xray_material_node::aLightDiffuse;
MObject xray_material_node::aLightSpecular;
MObject xray_material_node::aLightShadowFraction;
MObject xray_material_node::aPreShadowIntensity;
MObject xray_material_node::aLightBlindData;
MObject xray_material_node::aPower;
MObject xray_material_node::aSpecularity;

MObject xray_material_node::aRayOrigin;
MObject xray_material_node::aRayDirection;
MObject xray_material_node::aObjectId;
MObject xray_material_node::aRaySampler;
MObject xray_material_node::aRayDepth;

MObject xray_material_node::aReflectGain;

MObject xray_material_node::aTriangleNormalCamera;


void xray_material_node::postConstructor( )
{
	setMPSafe(true);
}

xray_material_node::xray_material_node()
{}

xray_material_node::~xray_material_node()
{}

void * xray_material_node::creator()
{
	return CRT_NEW(xray_material_node)();
}

MStatus xray_material_node::initialize()
{
    MFnNumericAttribute nAttr; 
    MFnLightDataAttribute lAttr;

    aTranslucenceCoeff = nAttr.create("translucenceCoeff", "tc", 
									  MFnNumericData::kFloat);
    make_input_att(nAttr);

    aDiffuseReflectivity = nAttr.create("diffuseReflectivity", "drfl",
										MFnNumericData::kFloat);
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setDefault(0.8f) );

    aColor = nAttr.createColor( "color", "c" );
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setDefault(0.0f, 0.58824f, 0.644f) );

    aIncandescence = nAttr.createColor( "incandescence", "ic" );
    make_input_att(nAttr);

    aOutColor = nAttr.createColor( "outColor", "oc" );
    make_oupput_att(nAttr);

    aPointCamera = nAttr.createPoint( "pointCamera", "pc" );
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setDefault(1.0f, 1.0f, 1.0f) );
    CHK_STAT_R ( nAttr.setHidden(true) );

    aPower = nAttr.create( "power", "pow", MFnNumericData::kFloat);
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setMin(0.0f) );
    CHK_STAT_R ( nAttr.setMax(200.0f) );
    CHK_STAT_R ( nAttr.setDefault(10.0f) );

    aSpecularity = nAttr.create( "specularity", "spc", MFnNumericData::kFloat);
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setMin(0.0f) );
    CHK_STAT_R ( nAttr.setMax(1.0f) ) ;
    CHK_STAT_R ( nAttr.setDefault(0.5f) );

    aReflectGain = nAttr.create( "reflectionGain", "rg", MFnNumericData::kFloat);
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setMin(0.0f) );
    CHK_STAT_R ( nAttr.setMax(1.0f) );
    CHK_STAT_R ( nAttr.setDefault(0.5f) );

    aNormalCamera = nAttr.createPoint( "normalCamera", "n" );
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setDefault(1.0f, 1.0f, 1.0f) );
    CHK_STAT_R ( nAttr.setHidden(true) );

    aTriangleNormalCamera = nAttr.createPoint( "triangleNormalCamera", "tn" );
    make_input_att(nAttr);
    CHK_STAT_R ( nAttr.setDefault(1.0f, 1.0f, 1.0f));
    CHK_STAT_R ( nAttr.setHidden(true));

    aLightDirection = nAttr.createPoint( "lightDirection", "ld" );
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );
    CHK_STAT_R ( nAttr.setDefault(1.0f, 1.0f, 1.0f) );

    aLightIntensity = nAttr.createColor( "lightIntensity", "li" );
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );
    CHK_STAT_R ( nAttr.setDefault(1.0f, 1.0f, 1.0f) );

    aLightAmbient = nAttr.create( "lightAmbient", "la",
								  MFnNumericData::kBoolean);
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );

    aLightDiffuse = nAttr.create( "lightDiffuse", "ldf", 
								  MFnNumericData::kBoolean);
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );

    aLightSpecular = nAttr.create( "lightSpecular", "ls", 
								   MFnNumericData::kBoolean);
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );

    aLightShadowFraction = nAttr.create("lightShadowFraction", "lsf",
										MFnNumericData::kFloat);
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );

    aPreShadowIntensity = nAttr.create("preShadowIntensity", "psi",
									   MFnNumericData::kFloat);
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );

    aLightBlindData = nAttr.createAddr("lightBlindData", "lbld");
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(true) );
    CHK_STAT_R ( nAttr.setWritable(false) );

    aLightData = lAttr.create( "lightDataArray", "ltd", 
                               aLightDirection, aLightIntensity, aLightAmbient,
                               aLightDiffuse, aLightSpecular, 
							   aLightShadowFraction,
                               aPreShadowIntensity,
                               aLightBlindData);
    CHK_STAT_R ( lAttr.setArray(true) );
    CHK_STAT_R ( lAttr.setStorable(false) );
    CHK_STAT_R ( lAttr.setHidden(true) );
    CHK_STAT_R ( lAttr.setDefault(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, true,
					 false, 0.0f, 1.0f, NULL) );

	// rayOrigin
	MObject RayX = nAttr.create( "rayOx", "rxo", MFnNumericData::kFloat, 0.0 );
	MObject RayY = nAttr.create( "rayOy", "ryo", MFnNumericData::kFloat, 0.0 );
	MObject RayZ = nAttr.create( "rayOz", "rzo", MFnNumericData::kFloat, 0.0 );
	aRayOrigin = nAttr.create( "rayOrigin", "rog", RayX, RayY, RayZ );
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(false) );

	// rayDirection 
	RayX = nAttr.create( "rayDirectionX", "rdx", MFnNumericData::kFloat, 1.0 );
	RayY = nAttr.create( "rayDirectionY", "rdy", MFnNumericData::kFloat, 0.0 );
	RayZ = nAttr.create( "rayDirectionZ", "rdz", MFnNumericData::kFloat, 0.0 );
	aRayDirection = nAttr.create( "rayDirection", "rad", RayX, RayY, RayZ );
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(false) );

	// objectId
	aObjectId = nAttr.createAddr( "objectId", "oi" );
    CHK_STAT_R ( nAttr.setStorable(false) ); 
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(false) );

	// raySampler
	aRaySampler = nAttr.createAddr("raySampler", "rtr");
    CHK_STAT_R ( nAttr.setStorable(false));
    CHK_STAT_R ( nAttr.setHidden(true) );
    CHK_STAT_R ( nAttr.setReadable(false) );

	// rayDepth
	aRayDepth = nAttr.create( "rayDepth", "rd", MFnNumericData::kShort, 0.0 );
    CHK_STAT_R ( nAttr.setStorable(false) );
    CHK_STAT_R  (nAttr.setHidden(true) ) ;
    CHK_STAT_R ( nAttr.setReadable(false) );

    CHK_STAT_R ( addAttribute(aTranslucenceCoeff) );
    CHK_STAT_R ( addAttribute(aDiffuseReflectivity) );
    CHK_STAT_R ( addAttribute(aColor) );
    CHK_STAT_R ( addAttribute(aIncandescence) );
    CHK_STAT_R ( addAttribute(aPointCamera) );
    CHK_STAT_R ( addAttribute(aNormalCamera) );
    CHK_STAT_R ( addAttribute(aTriangleNormalCamera) );

    CHK_STAT_R ( addAttribute(aLightData) );

    CHK_STAT_R ( addAttribute(aPower) );
    CHK_STAT_R ( addAttribute(aSpecularity) );
    CHK_STAT_R ( addAttribute(aOutColor) );

	CHK_STAT_R ( addAttribute(aRayOrigin) );
	CHK_STAT_R ( addAttribute(aRayDirection) );
	CHK_STAT_R ( addAttribute(aObjectId) );
	CHK_STAT_R ( addAttribute(aRaySampler) );
	CHK_STAT_R ( addAttribute(aRayDepth) );
    CHK_STAT_R ( addAttribute(aReflectGain) );

    CHK_STAT_R ( attributeAffects (aTranslucenceCoeff, aOutColor));
    CHK_STAT_R ( attributeAffects (aDiffuseReflectivity, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightIntensity, aOutColor));
    CHK_STAT_R ( attributeAffects (aIncandescence, aOutColor));
    CHK_STAT_R ( attributeAffects (aPointCamera, aOutColor));
    CHK_STAT_R ( attributeAffects (aNormalCamera, aOutColor));
    CHK_STAT_R ( attributeAffects (aTriangleNormalCamera, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightData, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightAmbient, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightSpecular, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightDiffuse, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightDirection, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightShadowFraction, aOutColor));
    CHK_STAT_R ( attributeAffects (aPreShadowIntensity, aOutColor));
    CHK_STAT_R ( attributeAffects (aLightBlindData, aOutColor));
    CHK_STAT_R ( attributeAffects (aPower, aOutColor));
    CHK_STAT_R ( attributeAffects (aSpecularity, aOutColor));
    CHK_STAT_R ( attributeAffects (aColor, aOutColor));

	CHK_STAT_R ( attributeAffects (aRayOrigin,aOutColor));
	CHK_STAT_R ( attributeAffects (aRayDirection,aOutColor));
	CHK_STAT_R ( attributeAffects (aObjectId,aOutColor));
	CHK_STAT_R ( attributeAffects (aRaySampler,aOutColor));
	CHK_STAT_R ( attributeAffects (aRayDepth,aOutColor));
    CHK_STAT_R ( attributeAffects (aReflectGain,aOutColor) );

    return MS::kSuccess;
}

MStatus xray_material_node::compute( const MPlug& plug, MDataBlock& block ) 
{ 
    if ((plug != aOutColor) && (plug.parent() != aOutColor))
		return MS::kUnknownParameter;

    MFloatVector resultColor(0.0,0.0,0.0);

    // get sample surface shading parameters
    MFloatVector& surfaceNormal = block.inputValue( aNormalCamera ).asFloatVector();
    MFloatVector& cameraPosition = block.inputValue( aPointCamera ).asFloatVector();

	// use for raytracing api enhancement below
	MFloatVector point = cameraPosition;
	MFloatVector normal = surfaceNormal;

    MFloatVector& surfaceColor  = block.inputValue( aColor ).asFloatVector();
    MFloatVector& incandescence = block.inputValue( aIncandescence ).asFloatVector();
    float diffuseReflectivity = block.inputValue( aDiffuseReflectivity ).asFloat();
    // float translucenceCoeff   = block.inputValue( aTranslucenceCoeff ).asFloat();
	// User-defined Reflection Color Gain
	float reflectGain = block.inputValue( aReflectGain ).asFloat();
  
    // Phong shading attributes
    float power = block.inputValue( aPower ).asFloat();
    float spec = block.inputValue( aSpecularity ).asFloat();

    float specularR, specularG, specularB;
    float diffuseR, diffuseG, diffuseB;
    diffuseR = diffuseG = diffuseB = specularR = specularG = specularB = 0.0;

    // get light list
    MArrayDataHandle lightData = block.inputArrayValue( aLightData );
    int numLights = lightData.elementCount();
    
    // iterate through light list and get ambient/diffuse values
    for( int count=1; count <= numLights; count++ )
    {
        MDataHandle currentLight = lightData.inputValue();
        MFloatVector& lightIntensity = currentLight.child(aLightIntensity).asFloatVector();
        
        // Find the blind data
        void*& blindData = currentLight.child( aLightBlindData ).asAddr();
     
        // find ambient component
        if( currentLight.child(aLightAmbient).asBool() ) {
            diffuseR += lightIntensity[0];
            diffuseG += lightIntensity[1];
            diffuseB += lightIntensity[2];
        }

        MFloatVector& lightDirection = currentLight.child(aLightDirection).asFloatVector();
        
        if ( blindData == NULL )
        {
			// find diffuse and specular component
			if( currentLight.child(aLightDiffuse).asBool() )
			{			
			    float cosln = lightDirection * surfaceNormal;;				
			    if( cosln > 0.0f )  // calculate only if facing light
			    {
			         diffuseR += lightIntensity[0] * ( cosln * diffuseReflectivity );
			         diffuseG += lightIntensity[1] * ( cosln * diffuseReflectivity );
			         diffuseB += lightIntensity[2] * ( cosln * diffuseReflectivity );
			    }

			    CHK_STAT_R( cameraPosition.normalize() );
			    			
				if( cosln > 0.0f ) // calculate only if facing light
				{				
				    float RV = ( ( (2*surfaceNormal) * cosln ) - lightDirection ) * cameraPosition;
				    if( RV > 0.0 ) RV = 0.0;
				    if( RV < 0.0 ) RV = -RV;

				    if ( power < 0 ) power = -power;

				    float s = spec * powf( RV, power );

				    specularR += lightIntensity[0] * s; 
				    specularG += lightIntensity[1] * s; 
				    specularB += lightIntensity[2] * s; 
				}
			}    
        }
        else
        {
			float cosln = MRenderUtil::diffuseReflectance( blindData, lightDirection, point, surfaceNormal, true );
			if( cosln > 0.0f )  // calculate only if facing light
			{
			     diffuseR += lightIntensity[0] * ( cosln * diffuseReflectivity );
			     diffuseG += lightIntensity[1] * ( cosln * diffuseReflectivity );
			     diffuseB += lightIntensity[2] * ( cosln * diffuseReflectivity );
			}

			CHK_STAT_R ( cameraPosition.normalize() );
			
			if ( currentLight.child(aLightSpecular).asBool() )
			{
				MFloatVector specLightDirection = lightDirection;
				MDataHandle directionH = block.inputValue( aRayDirection );
				MFloatVector direction = directionH.asFloatVector();
				float lightAttenuation = 1.0;
						
				specLightDirection = MRenderUtil::maximumSpecularReflection( blindData,
										lightDirection, point, surfaceNormal, direction );
				lightAttenuation = MRenderUtil::lightAttenuation( blindData, point, surfaceNormal, false );		

				// Are we facing the light
				if ( specLightDirection * surfaceNormal > 0.0f )
				{			
					float power2 = block.inputValue( aPower ).asFloat();
					MFloatVector rv = 2 * surfaceNormal * ( surfaceNormal * direction ) - direction;
					float s = spec * powf( rv * specLightDirection, power2 );
						
					specularR += lightIntensity[0] * s * lightAttenuation; 
					specularG += lightIntensity[1] * s * lightAttenuation; 
					specularB += lightIntensity[2] * s * lightAttenuation;
				}
			 }
       }
       if( !lightData.next() ) break;
    }

    // factor incident light with surface color and add incandescence
    resultColor[0] = ( diffuseR * surfaceColor[0] ) + specularR + incandescence[0];
    resultColor[1] = ( diffuseG * surfaceColor[1] ) + specularG + incandescence[1];
    resultColor[2] = ( diffuseB * surfaceColor[2] ) + specularB + incandescence[2];

	// add the reflection color
	if (reflectGain > 0.0) {

		MStatus status;

		// required attributes for using raytracer
		// origin, direction, sampler, depth, and object id.
		//
		MDataHandle originH = block.inputValue( aRayOrigin, &status);
		MFloatVector origin = originH.asFloatVector();

		MDataHandle directionH = block.inputValue( aRayDirection, &status);
		MFloatVector direction = directionH.asFloatVector();

		MDataHandle samplerH = block.inputValue( aRaySampler, &status);
		void*& samplerPtr = samplerH.asAddr();

		MDataHandle depthH = block.inputValue( aRayDepth, &status);
		short depth = depthH.asShort();

		MDataHandle objH = block.inputValue( aObjectId, &status);
		void*& objId = objH.asAddr();

		MFloatVector reflectColor;
		MFloatVector reflectTransparency;

		MFloatVector& triangleNormal = block.inputValue( aTriangleNormalCamera ).asFloatVector();

		// compute reflected ray
		MFloatVector l = -direction;
		float dot = l * normal;
		if( dot < 0.0 ) dot = -dot;
		MFloatVector refVector = 2 * normal * dot - l; 	// reflection ray
		float dotRef = refVector * triangleNormal;
		if( dotRef < 0.0 ) {
		    const float s = 0.01f;
			MFloatVector mVec = refVector - dotRef * triangleNormal;
			mVec.normalize();
			refVector = mVec + s * triangleNormal;
		}
		CHK_STAT_R ( refVector.normalize() );

		status = MRenderUtil::raytrace(
				point,    	//  origin
				refVector,  //  direction
				objId,		//  object id
				samplerPtr, //  sampler info
				depth,		//  ray depth
				reflectColor,	// output color and transp
				reflectTransparency);

		// add in the reflection color
		resultColor[0] += reflectGain * (reflectColor[0]);
		resultColor[1] += reflectGain * (reflectColor[1]);
		resultColor[2] += reflectGain * (reflectColor[2]);
	}

    // set ouput color attribute
    MDataHandle outColorHandle = block.outputValue( aOutColor );
    MFloatVector& outColor = outColorHandle.asFloatVector();
    outColor = resultColor;
    outColorHandle.setClean();

    return MS::kSuccess;
}

// #define MAKE_INPUT make_input_att
// #define MAKE_OUTPUT make_oupput_att
