////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_ENGINE_BUILDING

#define XRAY_MAYA_BUILDING

#define XRAY_LOG_MODULE_INITIATOR	"maya"
#include <xray/extensions.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOMB
#undef NOGDI
#undef NOTEXTMETRIC
#include <xray/os_include.h>

#include <xray/render/engine/model_format.h>

#pragma warning(push)
#pragma warning(disable: 4995)
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning(pop)

namespace std {
	class ostream;
	class istream;
} // namespace std

// what this is for?
#define NT_PLUGIN

#pragma warning(disable:4995)
#include <maya/MGlobal.h> 
#include <maya/MPxLocatorNode.h> 
#include <maya/MPxDragAndDropBehavior.h>
#include <maya/MString.h> 
#include <maya/MCommandResult.h> 
#include <maya/MTypeId.h> 
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDagNode.h>
#include <maya/MPxManipContainer.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlugArray.h>
#include <maya/MAnimUtil.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDag.h>
#include <maya/MFnSet.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnFreePointTriadManip.h>
#include <maya/MFnDistanceManip.h>
#include <maya/MMatrix.h>
#include <maya/MPxCommand.h>
#include <maya/MPxTransform.h>
#include <maya/MDGModifier.h>
#include <maya/MDagModifier.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MNodeMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MFnExpression.h>
#include <maya/MFnIkJoint.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItGeometry.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MArgList.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MFileObject.h>
#include <maya/MEulerRotation.h>
#include <maya/MAnimControl.h>
#include <maya/MFnLightDataAttribute.h> 
#include <maya/MFnMatrixData.h> 
#include <maya/MFnStringData.h> 
#include <maya/MSceneMessage.h> 
#include <maya/MHWShaderSwatchGenerator.h>
#include <maya/MHardwareRenderer.h>

#include <maya/MFnStringArrayData.h> 

#include <xray/strings_stream.h>
#include <xray/resources_fs.h>

#include "maya_memory.h"

#include "node_identifiers.h"


template <typename T>
inline T const & identity_( T const & value ) { return value; }

#undef R_ASSERT_U
#define R_ASSERT_U( expression, msg )	\
{									\
	if( !( identity_(expression) ) ) 			\
	{								\
		MString err	= __FILE__;			\
		err			+= " ";				\
		err			+= __LINE__;		\
		err			+= " ";				\
		err			+= msg;				\
		err			+= " ";				\
		display_warning(err);		\
		LOG_ERROR(err.asChar());		\
	}									\
}



#define CHK_ST(_status, ret)		\
{									\
	if(_status.error()) 					\
	{								\
		R_ASSERT_U( false, _status.errorString() );\
		ret;							\
	}								\
}

//#define CHK_STAT(_status)			\
//CHK_ST(_status, {})

#define CHK_STAT_R(_status)			\
CHK_ST(_status, return _status)

#define CHK_STAT_RT(_status, _ret_val)	\
CHK_ST(_status, return _ret_val)

#define CHK_STAT_RV(_status)		\
CHK_ST(_status, return)

#define LOG_MAYA_WARNING(expression, warning_string)		\
{									\
	if(!expression)					\
	{								\
	R_ASSERT_U( false, warning_string );\
	}								\
}

void display_info		( MString str );
void display_info		( pcstr str );
void display_warning	( MString str );
void display_warning	( pcstr str );
void display_error		( MString str );
bool is_batch_mode		();

//using xray::math::float2;
//using xray::math::float3;
using xray::maya::vector;
using xray::maya::map;
using xray::maya::set;

typedef vector<u16>					u16vec;
typedef vector<u32>					u32vec;
typedef vector<xray::math::float3>		float3vec;
typedef vector<xray::math::float2>		float2vec;

#include "helpers.h"


#endif // #ifndef PCH_H_INCLUDED