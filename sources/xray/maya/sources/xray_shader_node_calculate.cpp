////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray_shader_node.h"


// The compute() method does the actual work of the node using the inputs
// of the node to generate its output. 
//
// Compute takes two parameters: plug and data.
// - Plug is the the data value that needs to be recomputed
// - Data provides handles to all of the nodes attributes, only these
//   handles should be used when performing computations.
//
MStatus xray_shader_node::compute( const MPlug& plug, MDataBlock& block ) 
{ 
	// The plug parameter will allow us to determine which output attribute
	// needs to be calculated.
	if( plug == aOutColor
			|| plug == aOutColorR
			|| plug == aOutColorG
			|| plug == aOutColorB
			|| plug == aOutTransparency
			|| plug == aOutTransR
			|| plug == aOutTransG
			|| plug == aOutTransB )
	{
		MStatus					status;
		MFloatVector resultColor( 0.0, 0.0, 0.0 ); 

	
		// Get surface shading parameters from input block
		//
		MFloatVector& surfaceNormal = block.inputValue( aNormalCamera, &status ).asFloatVector();
		CHK_STAT					( status );

		MFloatVector& surfaceColor	= block.inputValue( aColor, &status ).asFloatVector();
		CHK_STAT					( status );

		//MFloatVector& incandescence = block.inputValue( aIncandescence, &status ).asFloatVector();
		//CHK_STAT					( status );

		//float diffuseReflectivity	= block.inputValue(aDiffuseReflectivity, &status ).asFloat();
		//CHK_STAT					( status );
	
// 		float translucenceCoeff		= block.inputValue( aTranslucenceCoeff, &status ).asFloat();
// 		CHK_STAT					( status );
  

		// Get light list
		//
		MArrayDataHandle lightData	= block.inputArrayValue( aLightData, &status );
		CHK_STAT					( status );

		int numLights				= lightData.elementCount( &status );
		CHK_STAT					( status );


		// Calculate the effect of the lights in the scene on the color
		// Iterate through light list and get ambient/diffuse values
		for( int count=1; count <= numLights; count++ )
		{
			// Get the current light out of the array
			//
			MDataHandle currentLight = lightData.inputValue( &status ); 
			CHK_STAT				( status );


			// Get the intensity of that light
			MFloatVector& lightIntensity = currentLight.child( aLightIntensity ).asFloatVector();

	 
			// Find ambient component
			if ( currentLight.child( aLightAmbient ).asBool() )
				resultColor += lightIntensity;


			// Find diffuse component
			//
			if ( currentLight.child( aLightDiffuse ).asBool() )
			{
				MFloatVector& lightDirection = currentLight.child( aLightDirection ).asFloatVector();
				float cosln = lightDirection * surfaceNormal;
   
			   if ( cosln > 0.0f ) 
					resultColor += lightIntensity * ( cosln*0.8f /* *diffuseReflectivity*/ );
			}


			// Advance to the next light.
			//
			if ( count < numLights ) 
			{
				status		= lightData.next();
				CHK_STAT	( status );
			}
		}


		// Factor incident light with surface color and add incandescence
		//
		resultColor[0] = resultColor[0] * surfaceColor[0];// + incandescence[0];
		resultColor[1] = resultColor[1] * surfaceColor[1];// + incandescence[1];
		resultColor[2] = resultColor[2] * surfaceColor[2];// + incandescence[2];


		// Set ouput color attribute
		if ( plug == aOutColor || plug == aOutColorR || plug == aOutColorG || plug == aOutColorB)
		{
			// Get the handle to the attribute
			MDataHandle outColorHandle	= block.outputValue( aOutColor, &status );
			CHK_STAT					( status );
			MFloatVector& outColor		= outColorHandle.asFloatVector();
		
			outColor					= resultColor;// Set the output value
			outColorHandle.setClean		( ); // Mark the output value as clean
		}


		// Set ouput transparency
		if ( plug == aOutTransparency || plug == aOutTransR || plug == aOutTransG || plug == aOutTransB )
		{
			MFloatVector& transparency	= block.inputValue( aInTransparency, &status ).asFloatVector();
			CHK_STAT					( status );

			// Get the handle to the attribute
			MDataHandle outTransHandle	= block.outputValue( aOutTransparency, &status );
			CHK_STAT					( status );
			MFloatVector& outTrans		= outTransHandle.asFloatVector();
			outTrans					= transparency;   // Set the output value
			outTransHandle.setClean		( ); // Mark the output value as clean
		}
	} 
	else
	{
		return( MS::kUnknownParameter ); // We got an unexpected plug
	}

	return( MS::kSuccess );
}

