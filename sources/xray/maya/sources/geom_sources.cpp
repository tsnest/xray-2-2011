////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_node.h"
#include <assert.h>
MPoint box_points[]={
	MPoint(0.5,		-0.5,	0.5,	0),
	MPoint(-0.5,	-0.5,	0.5,	0),
	MPoint(-0.5,	-0.5,	-0.5,	0),
	MPoint(0.5,		-0.5,	-0.5,	0),

	MPoint(0.5,		0.5,	0.5,	0),
	MPoint(-0.5,	0.5,	0.5,	0),
	MPoint(-0.5,	0.5,	-0.5,	0),
	MPoint(0.5,		0.5,	-0.5,	0)
};
unsigned short box_indices[]			= {0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7};

MPoint cylinder_points[]={
	MPoint( 0.404509, -0.500000, 	-0.293893		,0),
	MPoint( 0.154508, -0.500000, 	-0.475528		,0),
	MPoint(-0.154509, -0.500000, 	-0.475528		,0),
	MPoint(-0.404509, -0.500000, 	-0.293893		,0),
	MPoint(-0.500000, -0.500000, 	0.000000		,0),
	MPoint(-0.404509, -0.500000, 	0.293893		,0),
	MPoint(-0.154508, -0.500000, 	0.475528		,0),
	MPoint( 0.154509, -0.500000, 	0.475528		,0),
	MPoint( 0.404509, -0.500000, 	0.293893		,0),
	MPoint( 0.500000, -0.500000, 	0.000000		,0),
	MPoint( 0.404509, 0.500000 ,	-0.293893		,0),
	MPoint( 0.154508, 0.500000 ,	-0.475528		,0),
	MPoint(-0.154509, 0.500000 ,	-0.475528		,0),
	MPoint(-0.404509, 0.500000 ,	-0.293893		,0),
	MPoint(-0.500000, 0.500000 ,	0.000000		,0),
	MPoint(-0.404509, 0.500000 ,	0.293893		,0),
	MPoint(-0.154508, 0.500000 ,	0.475528		,0),
	MPoint( 0.154509, 0.500000 ,	0.475528		,0),
	MPoint( 0.404509, 0.500000 ,	0.293893		,0),
	MPoint( 0.500000, 0.500000 ,	0.000000		,0),
};
const unsigned short cyl_idx_count = 20*2 + 10*2;
unsigned short cylinder_indices[cyl_idx_count];
void build_cylinder(MStatus& status)
{
	memset(cylinder_indices, 0, cyl_idx_count*sizeof(cylinder_indices[0]));
	int segments	= 10;
	int k			= 0;
	int cc			= 0;

	for(int i=0; i<segments*2*2; i+=2)
	{
		int idx1				= cc;
		int idx2				= cc+1;
		if(idx1==segments-1)
			idx2				= 0;
		else
		if(idx1==segments*2-1)
			idx2				= segments;
				
		cylinder_indices[k++]	= (u16)idx1;
		cylinder_indices[k++]	= (u16)idx2;

		++cc;
	}

	cc			= 0;
	for(int i=0; i<segments*2; i+=2)
	{
		int idx1				= cc;
		int idx2				= cc+segments;

		cylinder_indices[k++]	= (u16)idx1;
		cylinder_indices[k++]	= (u16)idx2;

		++cc;
	}

	if(k!=cyl_idx_count)
	{
		display_warning	("error in procedure [build_cylinder]");
		status					= MStatus::kFailure;
	}

}

MPoint sphere_points[]={
MPoint( 0.125000,	-0.475528, 		-0.090818,	0.0),
MPoint( 0.047746,	-0.475528, 		-0.146946,	0.0),
MPoint(-0.047746,	-0.475528, 		-0.146946,	0.0),
MPoint(-0.125000,	-0.475528, 		-0.090818,	0.0),
MPoint(-0.154509,	-0.475528, 		0.000000,	0.0),
MPoint(-0.125000,	-0.475528, 		0.090818,	0.0),
MPoint(-0.047746,	-0.475528, 		0.146946,	0.0),
MPoint( 0.047746,	-0.475528, 		0.146946,	0.0),
MPoint( 0.125000,	-0.475528, 		0.090818,	0.0),
MPoint( 0.154509,	-0.475528, 		0.000000,	0.0),
MPoint( 0.237764,	-0.404509, 		-0.172746,	0.0),
MPoint( 0.090818,	-0.404509, 		-0.279509,	0.0),
MPoint(-0.090818,	-0.404509, 		-0.279509,	0.0),
MPoint(-0.237764,	-0.404509, 		-0.172746,	0.0),
MPoint(-0.293893,	-0.404509, 		0.000000,	0.0),
MPoint(-0.237764,	-0.404509, 		0.172746,	0.0),
MPoint(-0.090818,	-0.404509, 		0.279509,	0.0),
MPoint( 0.090818,	-0.404509, 		0.279509,	0.0),
MPoint( 0.237764,	-0.404509, 		0.172746,	0.0),
MPoint( 0.293893,	-0.404509, 		0.000000,	0.0),
MPoint( 0.327254,	-0.293893, 		-0.237764,	0.0),
MPoint( 0.125000,	-0.293893, 		-0.384711,	0.0),
MPoint(-0.125000,	-0.293893, 		-0.384710,	0.0),
MPoint(-0.327254,	-0.293893, 		-0.237764,	0.0),
MPoint(-0.404509,	-0.293893, 		0.000000,	0.0),
MPoint(-0.327254,	-0.293893, 		0.237764,	0.0),
MPoint(-0.125000,	-0.293893, 		0.384710,	0.0),
MPoint(	0.125000,	-0.293893, 		0.384710,	0.0),
MPoint(	0.327254,	-0.293893, 		0.237764,	0.0),
MPoint(	0.404509,	-0.293893, 		0.000000,	0.0),
MPoint(	0.384710,	-0.154508, 		-0.279509,	0.0),
MPoint(	0.146946,	-0.154508, 		-0.452254,	0.0),
MPoint(-0.146946,	-0.154508, 		-0.452254,	0.0),
MPoint(-0.384711,	-0.154508, 		-0.279509,	0.0),
MPoint(-0.475528,	-0.154508, 		0.000000,	0.0),
MPoint(-0.384710,	-0.154508, 		0.279509,	0.0),
MPoint(-0.146946,	-0.154508, 		0.452254,	0.0),
MPoint(	0.146946,	-0.154508, 		0.452254,	0.0),
MPoint(	0.384710,	-0.154508, 		0.279509,	0.0),
MPoint(	0.475528,	-0.154508, 		0.000000,	0.0),
MPoint(	0.404509, 	0.000000 , 		-0.293893, 	0.0),
MPoint(	0.154508, 	0.000000 ,		-0.475528, 	0.0),
MPoint(-0.154509, 	0.000000 ,		-0.475528,	0.0),
MPoint(-0.404509, 	0.000000 ,		-0.293893,	0.0),
MPoint(-0.500000, 	0.000000 ,		0.000000,	0.0),
MPoint(-0.404509, 	0.000000 ,		0.293893,	0.0),
MPoint(-0.154508, 	0.000000 ,		0.475528,	0.0),
MPoint(	0.154509, 	0.000000 ,		0.475528,	0.0),
MPoint(	0.404509, 	0.000000 ,		0.293893,	0.0),
MPoint(	0.500000, 	0.000000 ,		0.000000,	0.0),
MPoint(	0.384710, 	0.154508 ,		-0.279509,	0.0),
MPoint(	0.146946, 	0.154508 ,		-0.452254,	0.0),
MPoint(-0.146946, 	0.154508 ,		-0.452254,	0.0),
MPoint(-0.384711, 	0.154508 ,		-0.279509,	0.0),
MPoint(-0.475528, 	0.154508 ,		0.000000,	0.0),
MPoint(-0.384710, 	0.154508 ,		0.279509,	0.0),
MPoint(-0.146946, 	0.154508 ,		0.452254,	0.0),
MPoint(	0.146946, 	0.154508 ,		0.452254,	0.0),
MPoint(	0.384710, 	0.154508 ,		0.279509,	0.0),
MPoint(	0.475528, 	0.154508 ,		0.000000,	0.0),
MPoint(	0.327254, 	0.293893 ,		-0.237764,	0.0),
MPoint(	0.125000, 	0.293893 ,		-0.384711,	0.0),
MPoint(-0.125000, 	0.293893 ,		-0.384710,	0.0),
MPoint(-0.327254, 	0.293893 ,		-0.237764,	0.0),
MPoint(-0.404509, 	0.293893 ,		0.000000, 	0.0),
MPoint(-0.327254, 	0.293893 ,		0.237764, 	0.0),
MPoint(-0.125000, 	0.293893 ,		0.384710,	0.0),
MPoint(	0.125000, 	0.293893 ,		0.384710,	0.0),
MPoint(	0.327254, 	0.293893 ,		0.237764,	0.0),
MPoint(	0.404509, 	0.293893 ,		0.000000,	0.0),
MPoint(	0.237764, 	0.404509 ,		-0.172746,	0.0),
MPoint(	0.090818, 	0.404509 ,		-0.279509,	0.0),
MPoint(-0.090818, 	0.404509 ,		-0.279509,	0.0),
MPoint(-0.237764, 	0.404509 ,		-0.172746, 	0.0),
MPoint(-0.293893, 	0.404509 ,		0.000000,	0.0),
MPoint(-0.237764, 	0.404509 ,		0.172746,	0.0),
MPoint(-0.090818, 	0.404509 ,		0.279509,	0.0),
MPoint(	0.090818, 	0.404509 ,		0.279509,	0.0),
MPoint(	0.237764, 	0.404509 ,		0.172746,	0.0),
MPoint(	0.293893, 	0.404509 ,		0.000000,	0.0),
MPoint(	0.125000, 	0.475528 ,		-0.090818,	0.0),
MPoint(	0.047746, 	0.475528 ,		-0.146946, 	0.0),
MPoint(-0.047746, 	0.475528 ,		-0.146946,	0.0),
MPoint(-0.125000, 	0.475528 ,		-0.090818,	0.0),
MPoint(-0.154509, 	0.475528 ,		0.000000,	0.0),
MPoint(-0.125000, 	0.475528 ,		0.090818,	0.0),
MPoint(-0.047746, 	0.475528 ,		0.146946,	0.0),
MPoint(	0.047746, 	0.475528 ,		0.146946,	0.0),
MPoint(	0.125000, 	0.475528 ,		0.090818,	0.0),
MPoint(	0.154509, 	0.475528 ,		0.000000,	0.0),

MPoint(	0.000000,	-0.50000 ,		0.000000,	0.0),
MPoint(	0.000000,	0.500000 ,		0.000000,	0.0)
};

const unsigned short sph_idx_count = 90*2*2;
unsigned short sphere_indices[sph_idx_count];
void build_sphere(MStatus& status)
{
	memset(sphere_indices, 0, sph_idx_count*sizeof(sphere_indices[0]));

	int row_num		= 9;
	int col_num		= 10;

	int k			= 0;
	for (int r=0; r<row_num; ++r)
	{
		int row_offset				= r*col_num;
		
		int cc = 0;
		for(int c=0; c<col_num*2; c+=2)
		{
			sphere_indices[k++]	=	(u16)(cc						+ row_offset);
			sphere_indices[k++]	=	(u16)(((cc+1==col_num)?0:cc+1)	+ row_offset);
			cc++;
		}
	}

	int top_pt_idx		= 91;
//	int bottom_pt_idx	= 90;

	for (int c=0; c<col_num; ++c)
	{
		int rr = 0;
		for(int r=0; r<row_num*2; r+=2)
		{
			int idx1 = c				+ (rr*col_num);
			int idx2 = c + col_num		+ (rr*col_num);
			if(rr+1==row_num)
				idx2 = top_pt_idx;

			sphere_indices[k++]	=	(u16)idx1;
			sphere_indices[k++]	=	(u16)idx2;
			++rr;
		}
	}
	if(k!=sph_idx_count)
	{
		display_warning	("error in procedure [build_sphere]");
		status					= MStatus::kFailure;
	}
	
}

void collision_node::get_points(MPoint** points, unsigned short& points_count, unsigned short** indices, unsigned short& index_count, int& type) const
{
	MStatus stat;
	MObject thisNode	= thisMObject();
	MFnDagNode dagFn	(thisNode);

	MPlug typePlug		= dagFn.findPlug(dispType, &stat);
	typePlug.getValue	(type);


	switch( type )
	   {
	   case collision_box:
		   {
			points_count	= sizeof(box_points)/sizeof(box_points[0]);
			*points			= box_points;
			index_count		= sizeof(box_indices)/sizeof(box_indices[0]);
			*indices		= box_indices;
		   }break;
	   case collision_cylinder:
			points_count	= sizeof(cylinder_points)/sizeof(cylinder_points[0]);
			*points			= cylinder_points;
			index_count		= sizeof(cylinder_indices)/sizeof(cylinder_indices[0]);
			*indices		= cylinder_indices;
		   break;
	   case collision_sphere:
			points_count	= sizeof(sphere_points)/sizeof(sphere_points[0]);
			*points			= sphere_points;
			index_count		= sizeof(sphere_indices)/sizeof(sphere_indices[0]);
			*indices		= sphere_indices;
		   break;
	   }
}
