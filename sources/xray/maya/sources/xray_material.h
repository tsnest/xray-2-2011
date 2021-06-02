////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATERIAL_H_INCLUDED
#define XRAY_MATERIAL_H_INCLUDED

#include <maya/MPxHwShaderNode.h>
#include <maya/MGeometryData.h>

class xray_material_node : public MPxNode
{
	public:
                      xray_material_node	( );
    virtual           ~xray_material_node	( );

    virtual MStatus   compute				( const MPlug&, MDataBlock& );
	virtual void      postConstructor		( );

    static void *     creator				( );
    static MStatus    initialize			( );

    static const MTypeId    id;
	static const MString	Name;

	private:
	static MObject aColor;
    static MObject aTranslucenceCoeff;
    static MObject aDiffuseReflectivity;
	static MObject aIncandescence;
	static MObject aPointCamera;
	static MObject aNormalCamera;
	static MObject aLightDirection;
	static MObject aLightIntensity;
    static MObject aPower;
    static MObject aSpecularity;
    static MObject aLightAmbient;
    static MObject aLightDiffuse;
    static MObject aLightSpecular;
    static MObject aLightShadowFraction;
    static MObject aPreShadowIntensity;
    static MObject aLightBlindData;
    static MObject aLightData;

    static MObject aRayOrigin;
    static MObject aRayDirection;

    static MObject aObjectId;
    static MObject aRaySampler;
    static MObject aRayDepth;

    static MObject aReflectGain;

	static MObject aTriangleNormalCamera;

	static MObject aOutColor;
};


class hwPhongShader : public MPxHwShaderNode
{
public:
                    hwPhongShader();
    virtual         ~hwPhongShader();
	void			releaseEverything();

    virtual MStatus compute( const MPlug&, MDataBlock& );
	virtual void    postConstructor();

	// Internally cached attribute handling routines
	virtual bool getInternalValueInContext( const MPlug&,
											  MDataHandle&,
											  MDGContext&);
    virtual bool setInternalValueInContext( const MPlug&,
											  const MDataHandle&,
											  MDGContext&);

	// Interactive overrides
	virtual MStatus		bind( const MDrawRequest& request,
							  M3dView& view );
	virtual MStatus		unbind( const MDrawRequest& request,
								M3dView& view );
	virtual MStatus		geometry( const MDrawRequest& request,
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
								const float ** texCoordArrays);

	// Batch overrides
	virtual MStatus	glBind(const MDagPath& shapePath);
	virtual MStatus	glUnbind(const MDagPath& shapePath);
	virtual MStatus	glGeometry( const MDagPath& shapePath,
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
							  const float ** texCoordArrays);

	MStatus			draw(
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
							  const float ** texCoordArrays);

	void			drawDefaultGeometry();

	// Overridden to draw an image for swatch rendering.
	///
	virtual MStatus renderSwatchImage( MImage & image );
	void			drawTheSwatch( MGeometryData* pGeomData,
								   unsigned int* pIndexing,
								   unsigned int  numberOfData,
								   unsigned int  indexCount );

	virtual int		normalsPerVertex();
	virtual int		texCoordsPerVertex();
	virtual int		getTexCoordSetNames(MStringArray& names);

    static  void *  creator();
    static  MStatus initialize();

	MFloatVector	Phong ( double cos_a );
	void			init_Phong_texture ( void );
	void			printGlError( const char *call );

    static  MTypeId id;

	protected:

	void attachSceneCallbacks();
	void detachSceneCallbacks();

	static void releaseCallback(void* clientData);

	private:

	// Attributes
	static MObject  aColor;
	static MObject  aDiffuseColor;
	static MObject  aSpecularColor;
	static MObject  aShininess;

	// Internal data
	GLuint			phong_map_id;

	MPoint			cameraPosWS;

	float3			mAmbientColor;
	float3			mDiffuseColor;
	float3			mSpecularColor;
	float3			mShininess;
	bool			mAttributesChanged; // Keep track if any attributes changed

	// Callbacks that we monitor so we can release OpenGL-dependant
	// resources before their context gets destroyed.
	MCallbackId fBeforeNewCB;
	MCallbackId fBeforeOpenCB;
	MCallbackId fBeforeRemoveReferenceCB;
	MCallbackId fMayaExitingCB;
};

class hwPhongShaderBehavior : public MPxDragAndDropBehavior
{

	public:

	hwPhongShaderBehavior();
	virtual ~hwPhongShaderBehavior();

	virtual bool shouldBeUsedFor(MObject &sourceNode, MObject &destinationNode,
								 MPlug &sourcePlug, MPlug &destinationPlug);

	virtual MStatus connectNodeToNode(MObject &sourceNode, 
									  MObject &destinationNode, bool force );

	virtual MStatus connectNodeToAttr(MObject &sourceNode,
									  MPlug &destinationPlug, bool force );

	virtual MStatus connectAttrToNode(MPlug &sourceNode, 
									  MObject &destinationNode, bool force );

	virtual MStatus connectAttrToAttr(MPlug &sourceNode,
									  MPlug &destinationPlug, bool force );

	static void *creator();

	protected:

	MObject findShadingEngine(MObject &node);

	private:
};

#endif // #ifndef XRAY_MATERIAL_H_INCLUDED