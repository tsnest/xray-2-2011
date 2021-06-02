#include "pch.h"

//#include <xray/animation/anim_track.h>
//#include <xray/maya_animation/data_header.h>
//#include "anim_export_util.h"
//
//#define kMaxTan 5729577.9485111479f
//using namespace xray::maya_animation;
//using namespace xray::animation;
//
//MStatus export_animated_plugs	( anm_track& track, const MPlugArray &animatedPlugs );
//MStatus export_animated_plugs	( MObject joint, FILE* f );
//MStatus export_animated_plugs	( MSelectionList &all_joints, FILE* f );
//
//struct EtReadKey 
//{
//	float			time;
//	float			value;
//	MFnAnimCurve::TangentType	inTangentType;
//	MFnAnimCurve::TangentType	outTangentType;
//	float			inAngle;
//	float			inWeightX;
//	float			inWeightY;
//	float			outAngle;
//	float			outWeightX;
//	float			outWeightY;
//	struct EtReadKey *	next;
//}; // struct EtReadKey 
//
//
///*
////	Function Name:
////		assembleAnimCurve
////
////	Description:
////		A static helper function to assemble an EtCurve animation curve
////	from a linked list of heavy-weights keys
////
////  Input Arguments:
////		EtReadKey *firstKey			The linked list of keys
////		int numKeys				The number of keyss
////		bool isWeighted		Whether or not the curve has weighted tangents
////		bool useOldSmooth		Whether or not to use pre-Maya2.0 smooth
////									tangent computation
////
////  Return Value:
////		EtCurve *animCurve			The assembled animation curve
////
////	Note:
////		This function will also free the memory used by the heavy-weight keys
//*/
//;
//
//int assembleAnimCurve (EtReadKey* firstKey, int numKeys, bool isWeighted, EtCurve* src_and_dest)
//{
//	EtReadKey*	prevKey			= 0;
//	EtReadKey*	key				= 0;
//	EtReadKey*	nextKey			= 0;
//	int		index;
//	EtKey*		thisKey;
//	float		py, ny, dx;
//	bool	hasSmooth;
//	float		length;
//	float		inTanX = xray::memory::uninitialized_value<float>(); 
//	float		inTanY = xray::memory::uninitialized_value<float>(); 
//	float		outTanX = xray::memory::uninitialized_value<float>(); 
//	float		outTanY = xray::memory::uninitialized_value<float>();
//	float		inTanXs, inTanYs, outTanXs, outTanYs;
//
//	// make sure we have useful information 
//	if ((firstKey == 0) || (numKeys == 0)) 
//		return (0);
//
//	/* allocate some memory for the animation curve */
//	//src_and_dest->create_keys	(numKeys);
//	src_and_dest->keyList.resize(numKeys);
//
//	/* initialise the animation curve parameters */
////	src_and_dest->numKeys		= numKeys;
//	src_and_dest->isWeighted	= isWeighted;
//	src_and_dest->isStatic		= true;
//	src_and_dest->preInfinity	= kInfinityConstant;
//	src_and_dest->postInfinity	= kInfinityConstant;
//
//	/* initialise the cache */
//	src_and_dest->lastKey		= 0;
//	src_and_dest->lastIndex		= -1;
//	src_and_dest->lastInterval	= -1;
//	src_and_dest->isStep		= false;
//	src_and_dest->isStepNext	= false;
//
//	/* compute tangents */
//	nextKey			= firstKey;
//	index			= 0;
//	while(nextKey != 0) 
//	{
//		if (prevKey != 0) 
//			FREE(prevKey);
//
//		prevKey		= key;
//		key			= nextKey;
//		nextKey		= nextKey->next;
//
//		/* construct the final EtKey (light-weight key) */
//		thisKey			= &(src_and_dest->keyList[index++]);
//		thisKey->time	= key->time;
//		thisKey->value	= key->value;
//
//		/* compute the in-tangent values */
//		/* kTangentClamped */
//		if ((key->inTangentType == MFnAnimCurve::kTangentClamped) && (prevKey != 0)) 
//		{
//			py = prevKey->value - key->value;
//			if (py < 0.0) 
//				py = -py;
//
//			ny = (nextKey == 0 ? py : nextKey->value - key->value);
//			
//			if (ny < 0.0) 
//				ny = -ny;
//
//			if ((ny <= 0.05) || (py <= 0.05)) 
//				key->inTangentType = MFnAnimCurve::kTangentFlat;
//		}
//		hasSmooth	= false;
//		switch (key->inTangentType) 
//		{
//		case MFnAnimCurve::kTangentFixed:
//			inTanX		= key->inWeightX * cos (key->inAngle) * 3.0f ;
//			inTanY		= key->inWeightY * sin (key->inAngle) * 3.0f ;
//			break;
//
//		case MFnAnimCurve::kTangentLinear:
//			if (prevKey == 0) 
//			{
//				inTanX	= 1.0f;
//				inTanY	= 0.0f;
//			}else 
//			{
//				inTanX	= key->time - prevKey->time;
//				inTanY	= key->value - prevKey->value;
//			}
//			break;
//
//		case MFnAnimCurve::kTangentFlat:
//			if (prevKey == 0) 
//			{
//				inTanX	= (nextKey == 0 ? 0.0f : nextKey->time - key->time);
//				inTanY	= 0.0f;
//			}else 
//			{
//				inTanX	= key->time - prevKey->time;
//				inTanY	= 0.0f;
//			}
//			break;
//
//		case MFnAnimCurve::kTangentStep:
//			inTanX		= 0.0f;
//			inTanY		= 0.0f;
//			break;
//
//		case MFnAnimCurve::kTangentStepNext:
//			inTanX		= FLT_MAX;
//			inTanY		= FLT_MAX;
//			break;
//
//		case MFnAnimCurve::kTangentSlow:
//		case MFnAnimCurve::kTangentFast:
//			key->inTangentType = MFnAnimCurve::kTangentSmooth;
//			if (prevKey == 0) 
//			{
//				inTanX	= 1.0;
//				inTanY	= 0.0;
//			}else 
//			{
//				inTanX	= key->time - prevKey->time;
//				inTanY	= key->value - prevKey->value;
//			}
//			break;
//
//		case MFnAnimCurve::kTangentSmooth:
//		case MFnAnimCurve::kTangentClamped:
//			key->inTangentType	= MFnAnimCurve::kTangentSmooth;
//			hasSmooth			= true;
//			break;
//		}
//
//		/* compute the out-tangent values */
//		/* kTangentClamped */
//		if ((key->outTangentType == MFnAnimCurve::kTangentClamped) && (nextKey != 0)) 
//		{
//			ny = nextKey->value - key->value;
//			if (ny < 0.0f) 
//				ny = -ny;
//
//			py = (prevKey == 0 ? ny : prevKey->value - key->value);
//			
//			if (py < 0.0f) 
//				py = -py;
//
//			if ((ny <= 0.05f) || (py <= 0.05f)) 
//				key->outTangentType = MFnAnimCurve::kTangentFlat;
//		}
//		switch (key->outTangentType) 
//		{
//		case MFnAnimCurve::kTangentFixed:
//			outTanX 		= key->outWeightX * cos (key->outAngle) * 3.0f ;
//			outTanY 		= key->outWeightY * sin (key->outAngle) * 3.0f ;
//			break;
//
//		case MFnAnimCurve::kTangentLinear:
//			if (nextKey == 0) 
//			{
//				outTanX 	= 1.0f;
//				outTanY 	= 0.0f;
//			}
//			else {
//				outTanX		= nextKey->time - key->time;
//				outTanY		= nextKey->value - key->value;
//			}
//			break;
//
//		case MFnAnimCurve::kTangentFlat:
//			if (nextKey == 0) 
//			{
//				outTanX		= (prevKey == 0 ? 0.0f : key->time - prevKey->time);
//				outTanY		= 0.0f;
//			}else 
//			{
//				outTanX		= nextKey->time - key->time;
//				outTanY		= 0.0f;
//			}
//			break;
//
//		case MFnAnimCurve::kTangentStep:
//			outTanX			= 0.0f;
//			outTanY			= 0.0f;
//			break;
//
//		case MFnAnimCurve::kTangentStepNext:
//			outTanX			= FLT_MAX;
//			outTanY			= FLT_MAX;
//			break;
//
//		case MFnAnimCurve::kTangentSlow:
//		case MFnAnimCurve::kTangentFast:
//			key->outTangentType = MFnAnimCurve::kTangentSmooth;
//			if (nextKey == 0) 
//			{
//				outTanX		= 1.0;
//				outTanY		= 0.0;
//			}
//			else {
//				outTanX		= nextKey->time - key->time;
//				outTanY		= nextKey->value - key->value;
//			}
//			break;
//
//		case MFnAnimCurve::kTangentSmooth:
//		case MFnAnimCurve::kTangentClamped:
//			key->outTangentType = MFnAnimCurve::kTangentSmooth;
//			hasSmooth			= true;
//			break;
//		}
//
//		/* compute smooth tangents (if necessary) */
//		if (hasSmooth) 
//		{
//			{
//				/* Maya 2.0 smooth tangents */
//				if ((prevKey == 0) && (nextKey != 0)) 
//				{
//					outTanXs	= nextKey->time - key->time;
//					outTanYs	= nextKey->value - key->value;
//					inTanXs		= outTanXs;
//					inTanYs		= outTanYs;
//				}else 
//				if ((prevKey != 0) && (nextKey == 0)) 
//				{
//					outTanXs	= key->time - prevKey->time;
//					outTanYs	= key->value - prevKey->value;
//					inTanXs		= outTanXs;
//					inTanYs		= outTanYs;
//				}else 
//				if ((prevKey != 0) && (nextKey != 0)) 
//				{
//					/* There is a CV before and after this one*/
//					/* Find average of the adjacent in and out tangents. */
//
//					dx = nextKey->time - prevKey->time;
//					if (dx < 0.0001) 
//						outTanYs = kMaxTan;
//					else 
//						outTanYs = (nextKey->value - prevKey->value) / dx;
//
//					outTanXs		= nextKey->time - key->time;
//					inTanXs			= key->time - prevKey->time;
//					inTanYs			= outTanYs * inTanXs;
//					outTanYs		*= outTanXs;
//				}else 
//				{
//					inTanXs			= 1.0;
//					inTanYs			= 0.0;
//					outTanXs		= 1.0;
//					outTanYs		= 0.0;
//				}
//			}
//
//			if (key->inTangentType == MFnAnimCurve::kTangentSmooth) 
//			{
//				inTanX = inTanXs;
//				inTanY = inTanYs;
//			}
//			if (key->outTangentType == MFnAnimCurve::kTangentSmooth) 
//			{
//				outTanX = outTanXs;
//				outTanY = outTanYs;
//			}
//		} // has smooth
//
//		/* make sure the computed tangents are valid */
//		if (src_and_dest->isWeighted) 
//		{
//			if (inTanX < 0.0) 
//				inTanX = 0.0;
//
//			if (outTanX < 0.0) 
//				outTanX = 0.0;
//		}else 
//		if (( inTanX == FLT_MAX && inTanY == FLT_MAX ) 
//			|| ( outTanX == FLT_MAX && outTanY == FLT_MAX ) )
//		{
//			// SPecial case for step next tangents, do nothing
//		}else 
//		{
//			if (inTanX < 0.0)
//				inTanX = 0.0;
//			
//			length			= sqrt ((inTanX * inTanX) + (inTanY * inTanY));
//			if (length != 0.0f) 
//			{	/* zero lengths can come from step tangents */
//				inTanX /= length;
//				inTanY /= length;
//			}
//			if ((inTanX == 0.0f) && (inTanY != 0.0f)) 
//			{
//				inTanX = 0.0001f;
//				inTanY = (inTanY < 0.0f ? -1.0f : 1.0f) * (inTanX * kMaxTan);
//			}
//			if (outTanX < 0.0f) 
//			{
//				outTanX = 0.0f;
//			}
//			
//			length				= sqrt ((outTanX * outTanX) + (outTanY * outTanY));
//			if (length != 0.0f) 
//			{	/* zero lengths can come from step tangents */
//				outTanX /= length;
//				outTanY /= length;
//			}
//			if ((outTanX == 0.0f) && (outTanY != 0.0f)) 
//			{
//				outTanX = 0.0001f;
//				outTanY = (outTanY < 0.0f ? -1.0f : 1.0f) * (outTanX * kMaxTan);
//			}
//		}
//
//		thisKey->inTanX 		= inTanX;
//		thisKey->inTanY 		= inTanY;
//		thisKey->outTanX		= outTanX;
//		thisKey->outTanY		= outTanY;
//
//		/*
//		// check whether or not this animation curve is static (i.e. all the
//		// key values are the same)
//		*/
//		if (src_and_dest->isStatic) 
//		{
//			if ((prevKey != 0) && (prevKey->value != key->value)) 
//			{
//				src_and_dest->isStatic = false;
//			}else 
//			if ((inTanY != 0.0f) || (outTanY != 0.0f)) 
//			{
//				src_and_dest->isStatic = false;
//			}
//		}
//	}
//	if (src_and_dest->isStatic) 
//	{
//		if ((prevKey != 0) && (key != 0) && (prevKey->value != key->value)) 
//			src_and_dest->isStatic = false;
//	}
//	if (prevKey != 0) 
//		FREE(prevKey);
//
//	if (key != 0) 
//		FREE(key);
//
//	return 0;
//}
//
//
//MStatus export_animated_plugs(MObject anim_joint, FILE*	f )
//{
//	XRAY_UNREFERENCED_PARAMETERS	( f );
//	MPlugArray						xf_animatedPlugs;
//	MAnimUtil::findAnimatedPlugs	(anim_joint, xf_animatedPlugs);
//
//	MFnDependencyNode				dep_node(anim_joint);
//	MString str = "joint ";	str += dep_node.name();	str += " has "; str += xf_animatedPlugs.length(); str += " animated plugs";
//	display_info			(str);
//
//	anm_track						track(&xray::maya::g_allocator);
//
//	CHK_STAT_R( export_animated_plugs			(track, xf_animatedPlugs) );
//	NOT_IMPLEMENTED();//
////.	track.save						(f);
//
//	xf_animatedPlugs.clear			();
//	return MStatus::kSuccess;
//}
//
//MStatus 	export_animated_plugs( MSelectionList &all_joints , FILE* f )
//{
//
//		int length = all_joints.length		();
//		animation_curve_data_header			header;
//		header.type = maya_spline;
//		fwrite								(&header, sizeof(header), 1, f);
//		fwrite								(&length, sizeof(length), 1, f);
//
//		for(int i=0; i<length; ++i)
//		{
//			MObject							anim_joint;
//			all_joints.getDependNode		(i, anim_joint);
//			MDagPath pth;
//			CHK_STAT_R( all_joints.getDagPath			(i, pth) );
//
//			display_info(pth.fullPathName());
//			CHK_STAT_R( export_animated_plugs( anim_joint, f ) ) ;
//
//		}
//	return MStatus::kSuccess;
//}
//
//
//
//#define kDegRad 0.0174532925199432958f
//
//MStatus export_animated_plugs( anm_track& track, const MPlugArray &animatedPlugs )
//{
//	unsigned int numPlugs		= animatedPlugs.length();
//	for (unsigned int i = 0; i < numPlugs; i++) 
//	{
//
//		MPlug plug				= animatedPlugs[i];
//		MObjectArray			animation;
//
//		if (!MAnimUtil::findAnimation (plug, animation))	
//		{
//			MString str = "  plug ";	str += plug.name();	str += " has no animation";
//			display_info			(str);
//			
//			continue;
//		}
//
//		MObject attrObj			= plug.attribute();
//		MFnAttribute fnAttr		(attrObj);
//
//		MObject animCurveNode = animation[0];
//		if (!animCurveNode.hasFn (MFn::kAnimCurve))
//		{
//			display_warning("error: !animCurveNode.hasFn");
//			return MStatus::kFailure;
//		}
//		MFnAnimCurve manim		(animCurveNode);
//
//		const char* att_name	= fnAttr.name().asChar();
//		enum_channel_id ch_id	= track.get_channel_id(att_name);
//		if(ch_id == channel_unknown)
//		{
//			MString str =					"Ignoring animated attribute [";	
//			str								+= att_name;
//			str								+= "]";
//			display_info			(str);
//			continue;
//		}
//		MString str =					"Exporting animated attribute [";	
//		str								+= att_name;
//		str								+= "]";
//		display_info			(str);
//
//		EtCurve*	curve		= track.get_channel(ch_id, true);
//
////--------
//		float angularConversion		= 1.0f;
//		float frameRate				= 1.0;
//		{
//			//calcs angular conversion
//			MAngle::Unit ut = MAngle::uiUnit();
//			switch(ut)
//			{
//			case MAngle::kRadians:
//				angularConversion		= 1.0f;
//				break;
//			case MAngle::kDegrees:
//				angularConversion		= kDegRad;
//				break;
//			default:
//				display_warning("unknown angle units");
//			}
//
//		 //calc frame rate
//			MTime						t;
//			t.setValue					(1.0f);
//			frameRate					= 1.0f / (float)t.as(MTime::kSeconds);
//		}
//		EtReadKey*	firstKey			= 0;
//		EtReadKey*	lastKey				= 0;
//		EtReadKey*	readKey				= 0;
//
////		bool endOfCurve					= false;
//		bool isWeighted					= manim.isWeighted();
//		int numKeys						= manim.numKeys();
//		float unitConversion			= 1.0;
//
//		if(manim.animCurveType()==MFnAnimCurve::kAnimCurveTA || manim.animCurveType()==MFnAnimCurve::kAnimCurveUA)
//			unitConversion	= angularConversion;
//
//		for(int kidx=0; kidx<numKeys; ++kidx)
//		{
//			
//			readKey					= ALLOC(EtReadKey,1);
//			readKey->time			= (float)manim.time(kidx).value() / frameRate;
//			readKey->value			= (float)manim.value(kidx) * unitConversion;
//			readKey->inTangentType	= manim.inTangentType(kidx);
//			readKey->outTangentType = manim.outTangentType(kidx);
//
//			if (readKey->inTangentType == MFnAnimCurve::kTangentFixed) 
//			{
//				MAngle angle;
//				double weight;
//				manim.getTangent	(kidx, angle, weight, true);
//
//				readKey->inAngle	= (float)angle.value() * angularConversion;
//				readKey->inWeightX	= (float)weight / frameRate; 
//				readKey->inWeightY	= (float)weight; 
//			}
//
//			if (readKey->outTangentType == MFnAnimCurve::kTangentFixed) 
//			{
//				MAngle angle;
//				double weight;
//
//				manim.getTangent	(kidx, angle, weight, false);
//				readKey->outAngle	= (float)angle.value() * angularConversion;
//				readKey->outWeightX = (float)weight / frameRate; 
//				readKey->outWeightY = (float)weight; 
//			}
//			
//			readKey->next			= 0;
//
//			if (firstKey == 0) 
//			{
//				firstKey			= readKey;
//				lastKey				= firstKey;
//			}else 
//			{
//				lastKey->next		= readKey;
//				lastKey				= readKey;
//			}
//		} // collect keys
////		int res					= 
//			assembleAnimCurve(firstKey, numKeys, isWeighted, curve);
//
//		curve->preInfinity		= (EtInfinityType)manim.preInfinityType();
//		curve->postInfinity		= (EtInfinityType)manim.postInfinityType();
//	}
//	return MStatus::kSuccess;
//}




