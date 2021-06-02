////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray_material.h"

#define VERBOSE 0

hwPhongShaderBehavior::hwPhongShaderBehavior()
{}

hwPhongShaderBehavior::~hwPhongShaderBehavior()
{}

void *hwPhongShaderBehavior::creator()
{
	return CRT_NEW(hwPhongShaderBehavior)();
}

bool hwPhongShaderBehavior::shouldBeUsedFor(
	MObject & sourceNode, 
	MObject & destinationNode,
	MPlug   & /* sourcePlug */,
	MPlug   & /* destinationPlug */)
{
	bool result = false;

	// Handle dropping a hw shader on a maya shader
	//
	if ((MFnDependencyNode(sourceNode).typeName() == "hwPhongShader") &&
		destinationNode.hasFn(MFn::kLambert))
		result = true;

#if VERBOSE
	cerr << "shouldBeUsedFor "<<MFnDependencyNode(sourceNode).name().asChar() 
		 << " " << MFnDependencyNode(destinationNode).name().asChar()  
		 << (result?" true\n":" false\n");
#endif

	return result;
}

//
// Connect source plug to destination.
//
///////////////////////////////////////////////////////////////////////////////
static MStatus connectAttr(
	const MPlug &srcPlug, 
	const MPlug &destPlug, 
	bool		force)
{
	MStatus result = MS::kFailure;

	if(!srcPlug.isNull() && !destPlug.isNull())
	{
		MString cmd = "connectAttr ";
		if (force) cmd += "-f ";
		cmd += srcPlug.name() + " " + destPlug.name();
		result = MGlobal::executeCommand(cmd);

#if VERBOSE
		cerr << cmd.asChar()<<"\n";
#endif
	}
	return result;
}

//
//	Description:
//		Overloaded function from MPxDragAndDropBehavior
//	this method will handle the connection between the hwPhongShader
//	and the shader it is assigned to as well as any meshes that it is
//	assigned to.
//
///////////////////////////////////////////////////////////////////////////////
MStatus hwPhongShaderBehavior::connectNodeToNode( 
	MObject & sourceNode, 
	MObject & destinationNode, 
	bool      force )
{
	MStatus result = MS::kFailure;
	MFnDependencyNode src(sourceNode);

	if ((src.typeName() == "hwPhongShader") &&
		destinationNode.hasFn(MFn::kLambert))
	{
		MFnDependencyNode dest(destinationNode);
		result = connectAttr(src.findPlug("outColor"), dest.findPlug("hardwareShader"), force);
	}

#if VERBOSE
	if (result != MS::kSuccess)
		cerr << "connectNodeToNode "<<src.name().asChar() 
			 << " " << MFnDependencyNode(destinationNode).name().asChar()  
			 << " failed\n";
#endif

	return result;
}

//
//	Description:
//		Overloaded function from MPxDragAndDropBehavior
//	this method will assign the correct output from the hwPhong shader 
//	onto the given attribute.
//
///////////////////////////////////////////////////////////////////////////////
MStatus hwPhongShaderBehavior::connectNodeToAttr( 
	MObject & sourceNode,
	MPlug   & destinationPlug,
	bool      force )

{
	MStatus result = MS::kFailure;
	MFnDependencyNode src(sourceNode);

	//if we are dragging from a hwPhongShader
	//to a shader than connect the outColor
	//plug to the plug being passed in
	//	
	if ((src.typeName() == "hwPhongShader") &&
		destinationPlug.node().hasFn(MFn::kLambert))
	{
		result = connectAttr(src.findPlug("outColor"), destinationPlug, force);
	}

#if VERBOSE
	if (result != MS::kSuccess)
		cerr << "connectNodeToAttr "<<src.name().asChar() 
			 << " " << destinationPlug.name().asChar()  
			 << " failed\n";
#endif

	return result;
}

//
//
///////////////////////////////////////////////////////////////////////////////
MStatus hwPhongShaderBehavior::connectAttrToNode( 
	MPlug   & sourcePlug, 
	MObject & destinationNode,
	bool      force )
{
	MStatus result = MS::kFailure;
	MObject sourceNode = sourcePlug.node();
	MFnDependencyNode src(sourceNode);

	if ((src.typeName() == "hwPhongShader") &&
		destinationNode.hasFn(MFn::kLambert))
	{
		MFnDependencyNode dest(destinationNode);
		result = connectAttr(sourcePlug, dest.findPlug("hardwareShader"),
							 force);
	}

#if VERBOSE
	if (result != MS::kSuccess)
		cerr << "connectNodeAttrToNode "<<sourcePlug.name().asChar() 
			 << " " << MFnDependencyNode(destinationNode).name().asChar()  
			 << " failed\n";
#endif

	return result;
}

#undef ENABLE_TRACE_API_CALLS
#ifdef ENABLE_TRACE_API_CALLS
#define TRACE_API_CALLS(x) cerr << "hwPhongShader: "<<(x)<<"\n"
#else
#define TRACE_API_CALLS(x) 
#endif

#ifndef GL_EXT_texture_cube_map
# define GL_NORMAL_MAP_EXT                   0x8511
# define GL_REFLECTION_MAP_EXT               0x8512
# define GL_TEXTURE_CUBE_MAP_EXT             0x8513
# define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514
# define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A
# define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B
# define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C
#endif
///////////////////////////////////////////////////////////////////////////////
MStatus hwPhongShaderBehavior::connectAttrToAttr( 
	MPlug & sourcePlug, 
	MPlug & destinationPlug, 
	bool    force )
{
#if VERBOSE
	cerr << "In connectAttrToAttr "<<sourcePlug.name().asChar() 
		 << " " << destinationPlug.name().asChar()
		 << "\n";
#endif

	return connectAttr(sourcePlug, destinationPlug, force);
}


MTypeId hwPhongShader::id( 0x00105449 );

MObject  hwPhongShader::aColor;
MObject  hwPhongShader::aDiffuseColor;
MObject  hwPhongShader::aSpecularColor;
MObject  hwPhongShader::aShininess;

void hwPhongShader::postConstructor( )
{
	TRACE_API_CALLS("postConstructor");
	setMPSafe(false);
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////

void hwPhongShader::printGlError( const char *call )
{
 //   GLenum error;
	//while( (error = glGetError()) != GL_NO_ERROR ) {
	//    cerr << call << ":" << error << " is " << (const char *)gluErrorString( error ) << "\n";
	//}
}


MFloatVector hwPhongShader::Phong ( double cos_a )
{
	MFloatVector p;

	if ( cos_a < 0.0 ) cos_a = 0.0;

	p[0] = (float)(mSpecularColor[0]*pow(cos_a,double(mShininess[0])) +
				   mDiffuseColor[0]*cos_a + mAmbientColor[0]);
	p[1] = (float)(mSpecularColor[1]*pow(cos_a,double(mShininess[1])) + 
				   mDiffuseColor[1]*cos_a + mAmbientColor[1]);
	p[2] = (float)(mSpecularColor[2]*pow(cos_a,double(mShininess[2])) + 
				   mDiffuseColor[2]*cos_a + mAmbientColor[2]);

	if ( p[0] > 1.0f ) p[0] = 1.0f;
	if ( p[1] > 1.0f ) p[1] = 1.0f;
	if ( p[2] > 1.0f ) p[2] = 1.0f;

	return p;
}

#define PHONG_TEXTURE_RES 256

static const GLenum faceTarget[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
};

static void cubeToDir ( int face, double s, double t,
					  double &x, double &y, double &z )
//
// Description:
//		Map uv to cube direction
{
	switch ( face )
	{
		case 0:
			x = 1;
			y = -t;
			z = -s;
			break;
		case 1:
			x = -1;
			y = -t;
			z = s;
			break;
		case 2:
			x = s;
			y = 1;
			z = t;
			break;
		case 3:
			x = s;
			y = -1;
			z = -t;
			break;
		case 4:
			x = s;
			y = -t;
			z = 1;
			break;
		case 5:
			x = -s;
			y = -t;
			z = -1;
			break;
	}

	double invLen = 1.0 / sqrt ( x*x + y*y + z*z );
	x *= invLen;
	y *= invLen;
	z *= invLen;
}


void hwPhongShader::init_Phong_texture ( void )
//
// Description:
//		Set up a cube map for Phong lookup
//
{
	GLubyte * texture_data;

	// Always release the old texture id before getting a
	// new one.
	if (phong_map_id != 0)
		glDeleteTextures( 1, &phong_map_id );
	glGenTextures ( 1, &phong_map_id );

	glEnable ( GL_TEXTURE_CUBE_MAP_EXT );
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei ( GL_UNPACK_ROW_LENGTH, 0 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP_EXT, phong_map_id );

	texture_data = new GLubyte[3*PHONG_TEXTURE_RES*PHONG_TEXTURE_RES];

	for ( int face=0 ; face<6 ; face++ )
	{	
		int index = 0;
		for ( int j=0 ; j<PHONG_TEXTURE_RES ; j++ )
		{
			double t = 2*double(j)/(PHONG_TEXTURE_RES - 1) - 1;	// -1 to 1
			for ( int i=0 ; i<PHONG_TEXTURE_RES ; i++ )
			{
				double s = 2*double(i)/(PHONG_TEXTURE_RES - 1) - 1;	// -1 to 1
				double x = 0.0, y = 0.0, z = 0.0;
				cubeToDir ( face, s, t, x, y, z );

				MFloatVector intensity = Phong ( z );

				texture_data[index++] = (GLubyte)(255*intensity[0]);
				texture_data[index++] = (GLubyte)(255*intensity[1]);
				texture_data[index++] = (GLubyte)(255*intensity[2]);
			}
		}

		glTexImage2D ( faceTarget[face], 0, GL_RGB, PHONG_TEXTURE_RES, PHONG_TEXTURE_RES,
			0, GL_RGB, GL_UNSIGNED_BYTE, texture_data );

	}

	glDisable ( GL_TEXTURE_CUBE_MAP_EXT );

	delete [] texture_data;

	// Make sure to mark attributes "clean".
	mAttributesChanged = false;
}


hwPhongShader::hwPhongShader()
{
	TRACE_API_CALLS("hwPhongShader");
	attachSceneCallbacks();

	mAmbientColor[0] = mAmbientColor[1] = mAmbientColor[2] = 0.1f;
	mDiffuseColor[0] = mDiffuseColor[1] = mDiffuseColor[2] = 0.5f;
	mSpecularColor[0] = mSpecularColor[1] = mSpecularColor[2] = 0.5f;
	mShininess[0] = mShininess[1] = mShininess[2] = 100.0f;
	mAttributesChanged = false;

	phong_map_id = 0;
}

hwPhongShader::~hwPhongShader()
{
	TRACE_API_CALLS("~hwPhongShader");
	detachSceneCallbacks();
}

void hwPhongShader::releaseEverything()
{
    if (phong_map_id != 0) {
		M3dView view = M3dView::active3dView();

        // The M3dView class doesn't return the correct status if there isn't
        // an active 3D view, so we rely on the success of beginGL() which
        // will make the context current.
        //
        if (view.beginGL()) {
		    glDeleteTextures( 1, &phong_map_id );
        
            phong_map_id = 0;
        }

        view.endGL();
    }
}

void hwPhongShader::attachSceneCallbacks()
{
	fBeforeNewCB  = MSceneMessage::addCallback(MSceneMessage::kBeforeNew,  releaseCallback, this);
	fBeforeOpenCB = MSceneMessage::addCallback(MSceneMessage::kBeforeOpen, releaseCallback, this);
	fBeforeRemoveReferenceCB = MSceneMessage::addCallback(MSceneMessage::kBeforeRemoveReference, 
														  releaseCallback, this);
	fMayaExitingCB = MSceneMessage::addCallback(MSceneMessage::kMayaExiting, releaseCallback, this);
}

/*static*/
void hwPhongShader::releaseCallback(void* clientData)
{
	hwPhongShader *pThis = (hwPhongShader*) clientData;
	pThis->releaseEverything();
}

void hwPhongShader::detachSceneCallbacks()
{
	if (fBeforeNewCB)
		MMessage::removeCallback(fBeforeNewCB);
	if (fBeforeOpenCB)
		MMessage::removeCallback(fBeforeOpenCB);
	if (fBeforeRemoveReferenceCB)
		MMessage::removeCallback(fBeforeRemoveReferenceCB);
	if (fMayaExitingCB)
		MMessage::removeCallback(fMayaExitingCB);

	fBeforeNewCB = 0;
	fBeforeOpenCB = 0;
	fBeforeRemoveReferenceCB = 0;
	fMayaExitingCB = 0;
}


void * hwPhongShader::creator()
{
    return CRT_NEW(hwPhongShader)();
}

MStatus hwPhongShader::initialize()
{
	// Make sure that all attributes are cached internal for
	// optimal performance !

	TRACE_API_CALLS("initialize");
    MFnNumericAttribute nAttr; 

    // Create input attributes
    aColor = nAttr.createColor( "color", "c");
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setDefault(0.1f, 0.1f, 0.1f);
	nAttr.setCached( true );
	nAttr.setInternal( true );

    aDiffuseColor = nAttr.createColor( "diffuseColor", "dc" );
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setDefault(1.f, 0.5f, 0.5f);
	nAttr.setCached( true );
	nAttr.setInternal( true );

    aSpecularColor = nAttr.createColor( "specularColor", "sc" );
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setDefault(0.5f, 0.5f, 0.5f);
	nAttr.setCached( true );
	nAttr.setInternal( true );

	// This is defined as a point, so that users can easily enter
	// values beyond 1.
    aShininess = nAttr.createPoint( "shininess", "sh" );
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setDefault(100.0f, 100.0f, 100.0f);
	nAttr.setCached( true );
	nAttr.setInternal( true );


	// create output attributes here
	// outColor is the only output attribute and it is inherited
	// so we do not need to create or add it.
	//

	// Add the attributes here

    addAttribute(aColor);
	addAttribute(aDiffuseColor);
	addAttribute(aSpecularColor);
	addAttribute(aShininess);

    attributeAffects (aColor,			outColor);
    attributeAffects (aDiffuseColor,	outColor);
	attributeAffects (aSpecularColor,	outColor);
	attributeAffects (aShininess,		outColor);

    return MS::kSuccess;
}


// DESCRIPTION:
//
MStatus hwPhongShader::compute(
const MPlug&      plug,
      MDataBlock& block ) 
{ 
	TRACE_API_CALLS("compute");

    if ((plug != outColor) && (plug.parent() != outColor))
		return MS::kUnknownParameter;

	MFloatVector & color  = block.inputValue( aDiffuseColor ).asFloatVector();

    // set output color attribute
    MDataHandle outColorHandle = block.outputValue( outColor );
    MFloatVector& outColor = outColorHandle.asFloatVector();
	outColor = color;

    outColorHandle.setClean();
    return MS::kSuccess;
}

/* virtual */
bool hwPhongShader::setInternalValueInContext( const MPlug &plug, 
											   const MDataHandle &handle,
											   MDGContext & )
{
	bool handledAttribute = false;
	if (plug == aColor)
	{
		handledAttribute = true;
		float3 & val = handle.asFloat3();
		if (val[0] != mAmbientColor[0] || 
			val[1] != mAmbientColor[1] || 
			val[2] != mAmbientColor[2])
		{
			mAmbientColor[0] = val[0];
			mAmbientColor[1] = val[1];
			mAmbientColor[2] = val[2];
			mAttributesChanged = true;
		}
	}
	else if (plug == aDiffuseColor)
	{
		handledAttribute = true;
		float3 & val = handle.asFloat3();
		if (val[0] != mDiffuseColor[0] || 
			val[1] != mDiffuseColor[1] || 
			val[2] != mDiffuseColor[2])
		{
			mDiffuseColor[0] = val[0];
			mDiffuseColor[1] = val[1];
			mDiffuseColor[2] = val[2];
			mAttributesChanged = true;
		}
	}
	else if (plug == aSpecularColor)
	{
		handledAttribute = true;
		float3 & val = handle.asFloat3();
		if (val[0] != mSpecularColor[0] || 
			val[1] != mSpecularColor[1] || 
			val[2] != mSpecularColor[2])
		{
			mSpecularColor[0] = val[0];
			mSpecularColor[1] = val[1];
			mSpecularColor[2] = val[2];
			mAttributesChanged = true;
		}
	}
	else if (plug == aShininess)
	{
		handledAttribute = true;
		float3 & val = handle.asFloat3();
		if (val[0] != mShininess[0] || 
			val[1] != mShininess[1] || 
			val[2] != mShininess[2])
		{
			mShininess[0] = val[0];
			mShininess[1] = val[1];
			mShininess[2] = val[2];
			mAttributesChanged = true;
		}
	}

	return handledAttribute;
}

/* virtual */
bool
hwPhongShader::getInternalValueInContext( const MPlug& plug,
										 MDataHandle& handle,
										 MDGContext&)
{
	bool handledAttribute = false;
	if (plug == aColor)
	{
		handledAttribute = true;
		handle.set( mAmbientColor[0], mAmbientColor[1], mAmbientColor[2] );
	}
	else if (plug == aDiffuseColor)
	{
		handledAttribute = true;
		handle.set( mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2] );
	}
	else if (plug == aSpecularColor)
	{
		handledAttribute = true;
		handle.set( mSpecularColor[0], mSpecularColor[1], mSpecularColor[2] );
	}
	else if (plug == aShininess)
	{
		handledAttribute = true;
		handle.set( mShininess[0], mShininess[1], mShininess[2] );
	}
	return handledAttribute;
}

// #define _MAKE_HWSHADER_NODE_STATICLY_EVALUATE_

/* virtual */
MStatus	hwPhongShader::bind(const MDrawRequest& request, M3dView& view)

{
	TRACE_API_CALLS("bind");

	if (mAttributesChanged || (phong_map_id == 0))
	{
		init_Phong_texture ();
	}

	return MS::kSuccess;
}


/* virtual */
MStatus	hwPhongShader::glBind(const MDagPath&)
{
	TRACE_API_CALLS("glBind");

	if ( mAttributesChanged || (phong_map_id == 0))
	{
		init_Phong_texture ();
	}

	return MS::kSuccess;
}


/* virtual */
MStatus	hwPhongShader::unbind(const MDrawRequest& request, M3dView& view)
{
	TRACE_API_CALLS("unbind");

    // The texture may have been allocated by the draw; it's kept
    // around for use again. When scene new or open is performed this
    // texture will be released in releaseEverything().

	return MS::kSuccess;
}

/* virtual */
MStatus	hwPhongShader::glUnbind(const MDagPath&)
{
	TRACE_API_CALLS("glUnbind");

    // The texture may have been allocated by the draw; it's kept
    // around for use again. When scene new or open is performed this
    // texture will be released in releaseEverything().

	return MS::kSuccess;
}

/* virtual */
MStatus	hwPhongShader::geometry( const MDrawRequest& request,
							    M3dView& view,
                                int prim,
								unsigned int writable,
								int indexCount,
								const unsigned int * indexArray,
								int vertexCount,
								const int * vertexIDs,
								const float * vertexArray,
								int normalCount,
								const float ** normalArrays,
								int colorCount,
								const float ** colorArrays,
								int texCoordCount,
								const float ** texCoordArrays)
{
	TRACE_API_CALLS("geometry");
	MStatus stat = MStatus::kSuccess;

	stat = draw( prim, writable, indexCount, indexArray, vertexCount,
			vertexIDs, vertexArray, normalCount, normalArrays, colorCount, 
			colorArrays, texCoordCount, texCoordArrays);

	return stat;
}

/* virtual */
MStatus	hwPhongShader::glGeometry(const MDagPath & path,
                                int prim,
								unsigned int writable,
								int indexCount,
								const unsigned int * indexArray,
								int vertexCount,
								const int * vertexIDs,
								const float * vertexArray,
								int normalCount,
								const float ** normalArrays,
								int colorCount,
								const float ** colorArrays,
								int texCoordCount,
								const float ** texCoordArrays)
{
	TRACE_API_CALLS("glGeometry");
	MStatus stat = MStatus::kSuccess;

	stat = draw( prim, writable, indexCount, indexArray, vertexCount,
				vertexIDs, vertexArray, normalCount, normalArrays, colorCount, 
				colorArrays, texCoordCount, texCoordArrays);

	return stat;
}

/* virtual */
MStatus 
hwPhongShader::renderSwatchImage( MImage & outImage )
{
	MStatus status = MStatus::kFailure;

	// Get the hardware renderer utility class
	MHardwareRenderer *pRenderer = MHardwareRenderer::theRenderer();
	if (pRenderer)
	{
		const MString& backEndStr = pRenderer->backEndString();

		// Get geometry
		// ============
		unsigned int* pIndexing = 0;
		unsigned int  numberOfData = 0;
		unsigned int  indexCount = 0;

		MHardwareRenderer::GeometricShape gshape = MHardwareRenderer::kDefaultSphere;

		MGeometryData* pGeomData =
			pRenderer->referenceDefaultGeometry( gshape, numberOfData, pIndexing, indexCount );
		if( !pGeomData )
		{
			return MStatus::kFailure;
		}

		// Make the swatch context current
		// ===============================
		//
		unsigned int width, height;
		outImage.getSize( width, height );
		unsigned int origWidth = width;
		unsigned int origHeight = height;

		MStatus status2 = pRenderer->makeSwatchContextCurrent( backEndStr, width, height );

		if( status2 == MS::kSuccess )
		{
			glPushAttrib ( GL_ALL_ATTRIB_BITS );

			// Get camera
			// ==========
			{
				// Get the camera frustum from the API
				double l, r, b, t, n, f;
				pRenderer->getSwatchOrthoCameraSetting( l, r, b, t, n, f );

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho( l, r, b, t, n, f );

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				// Rotate the cube a bit so we don't see it head on
				if (gshape ==  MHardwareRenderer::kDefaultCube)
					glRotatef( 45, 1.0, 1.0, 1.0 );
				else if (gshape == MHardwareRenderer::kDefaultPlane)
					glScalef( 1.5, 1.5, 1.5 );
				else
					glScalef( 1.0, 1.0, 1.0 );
			}

			// Draw The Swatch
			// ===============
			drawTheSwatch( pGeomData, pIndexing, numberOfData, indexCount );

			// Read pixels back from swatch context to MImage
			// ==============================================
			pRenderer->readSwatchContextPixels( backEndStr, outImage );

			// Double check the outing going image size as image resizing
			// was required to properly read from the swatch context
			outImage.getSize( width, height );
			if (width != origWidth || height != origHeight)
			{
				status = MStatus::kFailure;
			}
			else
			{
				status = MStatus::kSuccess;
			}

			glPopAttrib();
		}
		else
		{
			pRenderer->dereferenceGeometry( pGeomData, numberOfData );
		}
	}
	return status;
}
	
void			
hwPhongShader::drawTheSwatch( MGeometryData* pGeomData,
								   unsigned int* pIndexing,
								   unsigned int  numberOfData,
								   unsigned int  indexCount )
{
	TRACE_API_CALLS("drwaTheSwatch");

	MHardwareRenderer *pRenderer = MHardwareRenderer::theRenderer();
	if( !pRenderer )	return;

	if ( mAttributesChanged || (phong_map_id == 0))
	{
		init_Phong_texture ();
	}


	// Get the default background color
	float r, g, b, a;
	MHWShaderSwatchGenerator::getSwatchBackgroundColor( r, g, b, a );
	glClearColor( r, g, b, a );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glDisable ( GL_LIGHTING );
	glDisable ( GL_TEXTURE_1D );
	glDisable ( GL_TEXTURE_2D );
	{
		glEnable ( GL_TEXTURE_CUBE_MAP_EXT );
		glBindTexture ( GL_TEXTURE_CUBE_MAP_EXT, phong_map_id );
		glEnable ( GL_TEXTURE_GEN_S );
		glEnable ( GL_TEXTURE_GEN_T );
		glEnable ( GL_TEXTURE_GEN_R );
		glTexGeni ( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexGeni ( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexGeni ( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		// Could modify the texture matrix here to do light tracking...
		glMatrixMode ( GL_TEXTURE );
		glPushMatrix ();
		glLoadIdentity ();
		glRotatef( 5.0, -1.0, 0.0, 0.0 );
		glRotatef( 10.0, 0.0, 1.0, 0.0 );
		glMatrixMode ( GL_MODELVIEW );
	}

	// Draw default geometry
	{
		if (pGeomData)
		{
			glPushClientAttrib ( GL_CLIENT_VERTEX_ARRAY_BIT );

			float *vertexData = (float *)( pGeomData[0].data() );
			if (vertexData)
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glVertexPointer ( 3, GL_FLOAT, 0, vertexData );
			}

			float *normalData = (float *)( pGeomData[1].data() );
			if (normalData)
			{
				glEnableClientState( GL_NORMAL_ARRAY );
				glNormalPointer (    GL_FLOAT, 0, normalData );
			}

			if (vertexData && normalData && pIndexing )
				glDrawElements ( GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, pIndexing );

			glPopClientAttrib();

			// Release data references
			pRenderer->dereferenceGeometry( pGeomData, numberOfData );
		}
	}

	{
		glMatrixMode ( GL_TEXTURE );
		glPopMatrix ();
		glMatrixMode ( GL_MODELVIEW );

		glDisable ( GL_TEXTURE_CUBE_MAP_EXT );
		glDisable ( GL_TEXTURE_GEN_S );
		glDisable ( GL_TEXTURE_GEN_T );
		glDisable ( GL_TEXTURE_GEN_R );

	}
}

void hwPhongShader::drawDefaultGeometry()
{
	TRACE_API_CALLS("drawDefaultGeometry");

	MHardwareRenderer *pRenderer = MHardwareRenderer::theRenderer();
	if (!pRenderer)
		return;


	glPushAttrib ( GL_ENABLE_BIT );

	glDisable ( GL_LIGHTING );
	glDisable ( GL_TEXTURE_1D );
	glDisable ( GL_TEXTURE_2D );

	{
		glEnable ( GL_TEXTURE_CUBE_MAP_EXT );
		glBindTexture ( GL_TEXTURE_CUBE_MAP_EXT, phong_map_id );
		glEnable ( GL_TEXTURE_GEN_S );
		glEnable ( GL_TEXTURE_GEN_T );
		glEnable ( GL_TEXTURE_GEN_R );
		glTexGeni ( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexGeni ( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexGeni ( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		glMatrixMode ( GL_TEXTURE );
		glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode ( GL_MODELVIEW );
	}

	// Get default geometry
	{
		unsigned int numberOfData = 0;
		unsigned int *pIndexing = 0;
		unsigned int indexCount = 0;

		MHardwareRenderer::GeometricShape gshape = MHardwareRenderer::kDefaultSphere;

		// Get data references
		MGeometryData * pGeomData =
			pRenderer->referenceDefaultGeometry( gshape, numberOfData, pIndexing, indexCount);

		if (pGeomData)
		{
			glPushClientAttrib ( GL_CLIENT_VERTEX_ARRAY_BIT );

			float *vertexData = (float *)( pGeomData[0].data() );
			if (vertexData)
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glVertexPointer ( 3, GL_FLOAT, 0, vertexData );
			}

			float *normalData = (float *)( pGeomData[1].data() );
			if (normalData)
			{
				glEnableClientState( GL_NORMAL_ARRAY );
				glNormalPointer (    GL_FLOAT, 0, normalData );
			}

			if (vertexData && normalData && pIndexing )
				glDrawElements ( GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, pIndexing );

			glPopClientAttrib();

			// Release data references
			pRenderer->dereferenceGeometry( pGeomData, numberOfData );
		}
	}


	{
		glMatrixMode ( GL_TEXTURE );
		glPopMatrix ();
		glMatrixMode ( GL_MODELVIEW );

		glDisable ( GL_TEXTURE_CUBE_MAP_EXT );
		glDisable ( GL_TEXTURE_GEN_S );
		glDisable ( GL_TEXTURE_GEN_T );
		glDisable ( GL_TEXTURE_GEN_R );

		glPopAttrib();
	}
}

MStatus	hwPhongShader::draw(int prim,
							unsigned int writable,
							int indexCount,
							const unsigned int * indexArray,
							int vertexCount,
							const int * vertexIDs,
							const float * vertexArray,
							int normalCount,
							const float ** normalArrays,
							int colorCount,
							const float ** colorArrays,
							int texCoordCount,
							const float ** texCoordArrays)
{
	TRACE_API_CALLS("draw");


#if !defined(_MAKE_HWSHADER_NODE_STATICLY_EVALUATE_)
	if ( prim != GL_TRIANGLES && prim != GL_TRIANGLE_STRIP)	{
        return MS::kFailure;
    }

    {
		// glPushAttrib ( GL_ALL_ATTRIB_BITS ); // This is overkill
		// Even this is too much as matrices and texture state are restored.
		// glPushAttrib ( GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_TRANSFORM_BIT );
		glPushAttrib ( GL_ENABLE_BIT );

		glDisable ( GL_LIGHTING );
		glDisable ( GL_TEXTURE_1D );
		glDisable ( GL_TEXTURE_2D );

		// Setup cube map generation
		glEnable ( GL_TEXTURE_CUBE_MAP_EXT );
		glBindTexture ( GL_TEXTURE_CUBE_MAP_EXT, phong_map_id );
		glEnable ( GL_TEXTURE_GEN_S );
		glEnable ( GL_TEXTURE_GEN_T );
		glEnable ( GL_TEXTURE_GEN_R );
		glTexGeni ( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexGeni ( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexGeni ( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT );
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		// Could modify the texture matrix here to do light tracking...
		glMatrixMode ( GL_TEXTURE );
		glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode ( GL_MODELVIEW );
    }
#endif
	// Draw the surface.
	//
    {
		glPushClientAttrib ( GL_CLIENT_VERTEX_ARRAY_BIT );
		// GL_VERTEX_ARRAY does not necessarily need to be
		// enabled, as it should be enabled before this routine
		// is valled.
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );

		glVertexPointer ( 3, GL_FLOAT, 0, &vertexArray[0] );
		glNormalPointer (    GL_FLOAT, 0, &normalArrays[0][0] );
		
		glDrawElements ( prim, indexCount, GL_UNSIGNED_INT, indexArray );
		
		// The client attribute is already being popped. You
		// don't need to reset state here.
		//glDisableClientState( GL_NORMAL_ARRAY );
		//glDisableClientState( GL_VERTEX_ARRAY );
		glPopClientAttrib();
    }
#if !defined(_MAKE_HWSHADER_NODE_STATICLY_EVALUATE_)
	{
		glMatrixMode ( GL_TEXTURE );
		glPopMatrix ();
		glMatrixMode ( GL_MODELVIEW );

		glDisable ( GL_TEXTURE_CUBE_MAP_EXT );
		glDisable ( GL_TEXTURE_GEN_S );
		glDisable ( GL_TEXTURE_GEN_T );
		glDisable ( GL_TEXTURE_GEN_R );

		glPopAttrib();
	}
#endif
	return MS::kSuccess;
}

/* virtual */
int	hwPhongShader::normalsPerVertex()
{
	TRACE_API_CALLS("normalsPerVertex");
#if defined(_TEST_CYCLING_NORMALS_PER_VERTEX_)
	static int normCnt = 1;
	if (normCnt == 3)
		normCnt = 1;
	else
		normCnt++;
	return normCnt;
#else
	return 1;
#endif
}

/* virtual */
int	hwPhongShader::texCoordsPerVertex()
{
	TRACE_API_CALLS("texCoordsPerVertex");
#if defined(_TEST_CYCLING_UVSETS_PER_VERTEX_)
	static int uvCnt = 1;
	if (uvCnt == 3)
		uvCnt = 1;
	else
		uvCnt++;
	return uvCnt;
#else
	return 0;
#endif
}

/* virtual */ 
int	hwPhongShader::getTexCoordSetNames(MStringArray& names)
{
	return 0;
}
