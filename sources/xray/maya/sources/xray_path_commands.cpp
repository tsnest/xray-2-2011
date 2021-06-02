////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray_path_commands.h"
#include "maya_engine.h"

const MString	xray_path_cmd::Name		= "xray_path";

pcstr models_path_Flag			= "-m";
pcstr models_path_LongFlag		= "-models";
pcstr animations_path_Flag		= "-a";
pcstr animations_path_LongFlag	= "-animations";

MSyntax	xray_path_cmd::newSyntax()
{
	MSyntax	syntax;

	syntax.addFlag(models_path_Flag,			models_path_LongFlag);
	syntax.addFlag(animations_path_Flag,		animations_path_LongFlag);

	return syntax;
}

MStatus	xray_path_cmd::doIt( const MArgList& args)
{
	MString path	= xray::maya::g_maya_engine.get_resources_path();

	MStatus					stat;
	MArgDatabase argData	(syntax(), args, &stat);
	CHK_STAT_R				(stat);

	if(argData.isFlagSet(models_path_Flag))
	{
		path					+= "/exported/models";
	}else
	if(argData.isFlagSet(animations_path_Flag))
	{
		path					+= "/exported/animations";
	}else
	{
		// no args specified - return resources root path.
	}

	this->setResult			( path );
	return					MStatus::kSuccess;
}


// get assigned material
//MStatus	xray_path_cmd::doIt( const MArgList& args)
//{
//        MStatus stat = MS::kSuccess;
//
//
//        //  Get the selection list and using the iterator to trace them
//        MSelectionList list;
//        MGlobal::getActiveSelectionList( list );
//
//		MStringArray	sel_strings;
//		list.getSelectionStrings	( sel_strings );
//		
//		if(sel_strings.length()>1)
//			return MStatus::kFailure;
//		
//		MItSelectionList listIt( list );
//		if(list.length()>1)
//			return MStatus::kFailure;
//
//		int i=0;
//        for (listIt.reset(); !listIt.isDone(); listIt.next(),++i )
//		{
//			MString& face_name = sel_strings[i];
//			display_info("Selected item=" + face_name);
//			
//			int idx1 = face_name.index('[');
//			int idx2 = face_name.index(']');
//
//			MString num_str = sel_strings[i].substring(idx1+1, idx2-1);
//			u32 face_index = num_str.asUnsigned();
//
//			
//            // Get the DAG path of each element and extend the path to its shape node
//            MDagPath			path;
//            listIt.getDagPath	( path );
//            path.extendToShape	( );
//    
//            // if a DAG Path is MFn::kMesh, it tries to obtain its material
//            if ( path.apiType() != MFn::kMesh )
//				continue;
//
//            // Create a function set for poly geometries, find faces that are assigned as a set
//            MFnMesh		meshFn( path );
//            MObjectArray sets, comps;
//
//            MString pathName					= path.fullPathName();
//            unsigned int instanceNumber			= path.instanceNumber();
//            meshFn.getConnectedSetsAndMembers	( instanceNumber, sets, comps, 1 );
//
//			int sets_len		= sets.length();
//			if (sets_len>1)		--sets_len;
//			
//			bool bfound = false;
//            for ( int i = 0; i<sets_len && !bfound; ++i )
//			{
//                MFnSet setFn ( sets[i] );
//                MItMeshPolygon tempFaceIt ( path, comps[i] );
//
//                MString aMemberFaceName;
//                for ( ;!tempFaceIt.isDone() ; tempFaceIt.next() )
//				{
//					if(tempFaceIt.index()==face_index)
//					{
//						display_info("ASSIGNED set name= "+setFn.name());
//						bfound = true;
//						break;
//					}
//                }
//            } // all sets
//		if(!bfound)
//		display_info("Has no ASSIGNED set name -*(");
//	}// all selection
//        return stat; // TODO: Error handling/Additional ASSERT requiredBCurrently it always returns MS::kSuccess
//}

//struct item
//{
//	MString				m_set_name;
//	MObject				m_initial_set;
//	MObjectArray		m_components;
//};
//
//vector<item*>			g_cont;
//
//void insert_item( MObject set_object,  MObject component_object )
//{
//	MFnSet setFn ( set_object );
//
//	vector<item*>::iterator it = g_cont.begin();
//	vector<item*>::iterator it_e = g_cont.end();
//
//	for(; it!=it_e; ++it)
//	{
//		item*	itm = *it;
//		if(itm->m_set_name == setFn.name() )
//		{
//			itm->m_components.append(component_object);
//			return;
//		}
//	}
//	
//	item* result			= CRT_NEW(item);
//	g_cont.push_back		( result );
//	result->m_set_name		= setFn.name();
//	result->m_initial_set	= set_object;
//	result->m_components.append(component_object);
//}
//
//// MERGE SGs
//MStatus	xray_path_cmd::doIt( const MArgList& args)
//{
//	g_cont.clear();
//    MStatus stat = MS::kSuccess;
//
//
//    //  Get the selection list and using the iterator to trace them
//    MSelectionList list;
//    MGlobal::getActiveSelectionList( list );
//
//	
//	MItSelectionList listIt( list );
//
//	int i=0;
//    for (listIt.reset(); !listIt.isDone(); listIt.next(),++i )
//	{
//        // Get the DAG path of each element and extend the path to its shape node
//        MDagPath			path;
//        listIt.getDagPath	( path );
//        path.extendToShape	( );
//
//        // if a DAG Path is MFn::kMesh, it tries to obtain its material
//        if ( path.apiType() != MFn::kMesh )
//			continue;
//
//        // Create a function set for poly geometries, find faces that are assigned as a set
//        MFnMesh		meshFn( path );
//        MObjectArray sets, comps;
//
//        MString pathName					= path.fullPathName();
//        unsigned int instanceNumber			= path.instanceNumber();
//        meshFn.getConnectedSetsAndMembers	( instanceNumber, sets, comps, 1 );
//		int set_cnt = sets.length();
//		int comp_cnt = comps.length();
//
//		int sets_len		= sets.length();
//		if (sets_len>1)		--sets_len;
//		
//        for ( int i = 0; i<sets_len; ++i )
//		{
//			display_info( "found SG " + MFnSet(sets[i]).name());
//			insert_item(sets[i], comps[i]);
//		}
//
//		//merge
//		vector<item*>::iterator it = g_cont.begin();
//		vector<item*>::iterator it_e = g_cont.end();
//		for(; it!=it_e; ++it)
//		{
//			item* itm				= *it;
//			display_info	( "merge SG "+ itm->m_set_name + "count =" + itm->m_sets.length());
//			MFnSet	initial(itm->m_initial_set);
//			MSelectionList result_union;
//			MStatus r = initial.getUnion(itm->m_sets, result_union);
//			MFnSet	merged_set;
//			merged_set.create(result_union, MFnSet::kNone, &r);
//			display_info("merged to " + merged_set.name());
//		}
//	}// all selection
//
//	return stat; // TODO: Error handling/Additional ASSERT requiredBCurrently it always returns MS::kSuccess
//}
