/*********************************************************************NVMH4****
Path:  
File:  

Copyright NVIDIA Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:
  Todo
  -	Performance improvements, right now for each vertex I am building the list
    of it's neighbors, when we could do a single pass and build the full adjacency
	map in the beginning.  Note: I tried this and didn't see a real perf improvement.

  - I'd like to provide a non c++ standard library interface, probably 
    a simple c interface for all those simple c folks. or the 
	old nvMeshMender interface for all those using it already
  
*/

#include "pch.h"
#include "nvMeshMender.h"

namespace
{
	const unsigned int NO_GROUP = 0xFFFFFFFF;

};

void MeshMender::Triangle::Reset()
{
	handled		= false;
	group		= NO_GROUP;
}

class MeshMender::CanSmoothChecker
{
public:
	virtual bool CanSmooth(MeshMender::Triangle* t1, MeshMender::Triangle* t2 )=0;
};


class CanSmoothNormalsChecker: public MeshMender::CanSmoothChecker, private boost::noncopyable
{
	const float m_minCreaseAngle;
public:
	CanSmoothNormalsChecker(float const minCreaseAngle ):m_minCreaseAngle(minCreaseAngle){};

	virtual bool CanSmooth( MeshMender::Triangle* t1, MeshMender::Triangle* t2 )
	{

		R_ASSERT		(t1 && t2);
		//for checking the angle, we want these to be normalized,
		//they may not be for whatever reason
		xray::math::float3 tmp1 = t1->normal;
		xray::math::float3 tmp2 = t2->normal;
		tmp1.normalize();
		tmp2.normalize();

		if( tmp1.dot_product(tmp2) >= m_minCreaseAngle )
		{
			return true;
		}else
			return false;
	}
};

class CanSmoothTangentsChecker: public MeshMender::CanSmoothChecker, private boost::noncopyable
{
	const float m_minCreaseAngle;
public:
	CanSmoothTangentsChecker(float const minCreaseAngle ):m_minCreaseAngle(minCreaseAngle){};

	virtual bool CanSmooth(MeshMender::Triangle* t1, MeshMender::Triangle* t2 )
	{

		R_ASSERT(t1 && t2);
		//for checking the angle, we want these to be normalized,
		//they may not be for whatever reason
		xray::math::float3 tmp1 = t1->tangent;
		xray::math::float3 tmp2 = t2->tangent;
		tmp1.normalize();
		tmp2.normalize();

		if(tmp1.dot_product(tmp2) >= m_minCreaseAngle )
		{
			return true;
		}else
			return false;
	}
};

class CanSmoothBinormalsChecker: public MeshMender::CanSmoothChecker, private boost::noncopyable
{
	const float m_minCreaseAngle;

public:
	CanSmoothBinormalsChecker(float const minCreaseAngle ):m_minCreaseAngle(minCreaseAngle){};

	virtual bool CanSmooth(MeshMender::Triangle* t1, MeshMender::Triangle* t2 )
	{

		R_ASSERT(t1 && t2);
		//for checking the angle, we want these to be normalized,
		//they may not be for whatever reason
		xray::math::float3 tmp1 = t1->binormal;
		xray::math::float3 tmp2 = t2->binormal;
		tmp1.normalize();
		tmp2.normalize();

		if(tmp1.dot_product(tmp2) >= m_minCreaseAngle )
			return true;
		else
			return false;
	}
};

MeshMender::MeshMender()
{
	MinNormalsCreaseCosAngle	= 0.3f; 
	MinTangentsCreaseCosAngle	= 0.0f;
	MinBinormalsCreaseCosAngle	= 0.0f;	
	WeightNormalsByArea			= 0.0f;
	m_RespectExistingSplits		= DONT_RESPECT_SPLITS;

}
MeshMender::~MeshMender()
{

}

void MeshMender::UpdateIndices(const u32 oldIndex , const u32 newIndex , TriangleList& curGroup)
{
   //make any triangle that used the oldIndex use the newIndex instead

	for( u32 t = 0; t < curGroup.size(); ++t )
	{
		TriID tID = curGroup[ t ];
		for(u32 indx = 0 ; indx < 3 ; ++indx)
		{
			if(m_Triangles[tID].indices[indx] == oldIndex)
				m_Triangles[tID].indices[indx] = newIndex;
		}
	}
}

void MeshMender::ProcessTangents(TriangleList&		possibleNeighbors,
								vector< Vertex >&   theVerts,
								vector< u16 >&		/*mappingNewToOldVert*/,
								Vertex*				workingVertex)
{
		NeighborGroupList neighborGroups;//a fresh group for each pass

		//reset each triangle to prepare for smoothing group building
		for( u32 i =0; i < possibleNeighbors.size(); ++i)
			m_Triangles[ possibleNeighbors[i] ].Reset();

		//now start building groups
		CanSmoothTangentsChecker canSmoothTangentsChecker(MinTangentsCreaseCosAngle);
		for( u32 i =0; i < possibleNeighbors.size(); ++i)
		{
			Triangle* currTri	= &(m_Triangles[ possibleNeighbors[i] ]);
			R_ASSERT			(currTri);

			if(!currTri->handled)
			{
				BuildGroups(currTri,possibleNeighbors, 
							neighborGroups, theVerts,
							&canSmoothTangentsChecker);
			}
		}

//		vector<D3DXVECTOR3> groupTangentVectors;
		xray::math::float3 gtang(0,0,0);
		xray::math::float3 last(0,1,0);
		for( u32 i=0; i<neighborGroups.size(); ++i)
		{
//			D3DXVECTOR3 gtang(0,0,0);
			for(u32 t = 0; t < neighborGroups[i].size(); ++t)//for each triangle in the group, 
			{
				TriID tID	= neighborGroups[i][t];
				last		= m_Triangles[tID].tangent; 
				gtang		+= last;
			}
//			Vec3Normalize( &gtang, &gtang );
//			groupTangentVectors.push_back(gtang);
		}
		gtang.normalize_safe( last );
		workingVertex->tangent = gtang;

//		//next step, ensure that triangles in different groups are not
//		//sharing vertices. and give the shared vertex their new group vector
//		set<size_t> otherGroupsIndices;
//		for( i = 0; i < neighborGroups.size(); ++i )
//		{
//			TriangleList& curGroup = neighborGroups[ i ];
//			set<size_t> thisGroupIndices;
//
//			for( size_t t = 0; t < curGroup.size(); ++t ) //for each tri
//			{
//				TriID tID = curGroup[ t ];
//				for(size_t indx = 0; indx < 3 ; indx ++)//for each vert in that tri
//				{
//					//if it is at the positions in question
////					if( theVerts[ m_Triangles[tID].indices[indx] ].pos  == workingPosition)
//					if( &(theVerts[ m_Triangles[tID].indices[indx] ])  == workingVertex)
//					{
//						//see if another group is already using this vert
//						if(otherGroupsIndices.find( m_Triangles[tID].indices[indx] ) != otherGroupsIndices.end() )
//						{
//							//then we need to make a new vertex
//							Vertex ov;
//							ov = theVerts[ m_Triangles[tID].indices[indx] ];
//							ov.tangent = groupTangentVectors[i];
//							float3 tn;
//							cv_vector(tn, ov.tangent);
//							if(!valid(tn))
//							{
//								LOG_ERROR("ff");
//							}
//
//							size_t oldIndex = m_Triangles[tID].indices[indx];
//							size_t newIndex = theVerts.size();
//							theVerts.push_back(ov);
//							AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
//							UpdateIndices(oldIndex,newIndex,curGroup);
//						}
//						else
//						{
//							//otherwise, just update it with the new vector
//							theVerts[ m_Triangles[tID].indices[indx] ].tangent = groupTangentVectors[i];
//						}
//
//						//store that we have used this index, so other groups can check
//						thisGroupIndices.insert(m_Triangles[tID].indices[indx]);
//					}
//				}
//				
//			}
//			
//			for(set<size_t>::iterator it = thisGroupIndices.begin(); it!= thisGroupIndices.end() ; ++it)
//			{
//				otherGroupsIndices.insert(*it);
//			}
//
//		}

}


void MeshMender::ProcessBinormals(TriangleList&		possibleNeighbors,
								vector< Vertex >&   theVerts,
								vector< u16 >&		/*mappingNewToOldVert*/,
								Vertex*				workingVertex)
{
		NeighborGroupList neighborGroups;//a fresh group for each pass

		//reset each triangle to prepare for smoothing group building
		for( u32 i =0; i < possibleNeighbors.size(); ++i )
			m_Triangles[ possibleNeighbors[i] ].Reset();

		//now start building groups
		CanSmoothBinormalsChecker canSmoothBinormalsChecker(MinBinormalsCreaseCosAngle);
		for( u32 i =0; i < possibleNeighbors.size(); ++i )
		{
			Triangle* currTri = &(m_Triangles[ possibleNeighbors[i] ]);
			R_ASSERT(currTri);
			if(!currTri->handled)
			{
				BuildGroups( currTri, possibleNeighbors, 
							neighborGroups, theVerts,
							&canSmoothBinormalsChecker );
			}
		}

//		vector<D3DXVECTOR3> groupBinormalVectors;
		
		xray::math::float3 gbinormal(0,0,0);
		xray::math::float3 last(0,1,0);
		for( u32 i=0; i<neighborGroups.size(); ++i)
		{
			for(u32 t = 0; t < neighborGroups[i].size(); ++t)//for each triangle in the group, 
			{
				TriID tID = neighborGroups[i][t];
				last		= m_Triangles[tID].binormal;
				gbinormal+=  m_Triangles[tID].binormal;
			}
//			Vec3Normalize( &gbinormal, &gbinormal );
//			groupBinormalVectors.push_back(gbinormal);
		}

		gbinormal.normalize_safe	( last);
		workingVertex->binormal		= gbinormal;
		R_ASSERT					(valid(workingVertex->binormal));


//		//next step, ensure that triangles in different groups are not
//		//sharing vertices. and give the shared vertex their new group vector
//		set<size_t> otherGroupsIndices;
//		for( i = 0; i < neighborGroups.size(); ++i )
//		{
//			TriangleList& curGroup = neighborGroups[ i ];
//			set<size_t> thisGroupIndices;
//
//			for( size_t t = 0; t < curGroup.size(); ++t ) //for each tri
//			{
//				TriID tID = curGroup[ t ];
//				for(size_t indx = 0; indx < 3 ; ++indx)//for each vert in that tri
//				{
//					//if it is at the positions in question
////					if( theVerts[ m_Triangles[tID].indices[indx] ].pos  == workingPosition)
//					if( &(theVerts[ m_Triangles[tID].indices[indx] ])  == workingVertex)
//					{
//						//see if another group is already using this vert
//						if(otherGroupsIndices.find( m_Triangles[tID].indices[indx] ) != otherGroupsIndices.end() )
//						{
//							//then we need to make a new vertex
//							Vertex ov;
//							ov = theVerts[ m_Triangles[tID].indices[indx] ];
//							ov.binormal = groupBinormalVectors[i];
//
//							float3 bn;
//							cv_vector(bn, ov.binormal);
//							if(!valid(bn))
//							{
//								LOG_ERROR("ff");
//							}
//
//							size_t oldIndex = m_Triangles[tID].indices[indx];
//							size_t newIndex = theVerts.size();
//							theVerts.push_back(ov);
//							AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
//							UpdateIndices(oldIndex,newIndex,curGroup);
//						}
//						else
//						{
//							//otherwise, just update it with the new vector
//							theVerts[ m_Triangles[tID].indices[indx] ].binormal = groupBinormalVectors[i];
//						}
//
//						//store that we have used this index, so other groups can check
//						thisGroupIndices.insert(m_Triangles[tID].indices[indx]);
//					}
//				}
//				
//			}
//			
//			for(set<size_t>::iterator it = thisGroupIndices.begin(); it!= thisGroupIndices.end() ; ++it)
//			{
//				otherGroupsIndices.insert(*it);
//			}
//
//		}

}


bool MeshMender::Mend( 
                  vector< Vertex >&    theVerts,
 				  vector< u16 >& theIndices,
				  vector< u16 >& mappingNewToOldVert,
				  const float minNormalsCreaseCosAngle,
				  const float minTangentsCreaseCosAngle,
				  const float minBinormalsCreaseCosAngle,
				  const float weightNormalsByArea,
				  const NormalCalcOption computeNormals,
				  const ExistingSplitOption respectExistingSplits )
{
	MinNormalsCreaseCosAngle		= minNormalsCreaseCosAngle;
	MinTangentsCreaseCosAngle		= minTangentsCreaseCosAngle;
	MinBinormalsCreaseCosAngle		= minBinormalsCreaseCosAngle;
	WeightNormalsByArea				= weightNormalsByArea;
	m_RespectExistingSplits			= respectExistingSplits;


	SetUpData( theVerts, theIndices, mappingNewToOldVert, computeNormals);

	//for each unique position
	for(VertexChildrenMap::iterator vert = m_VertexChildrenMap.begin();
		vert!= m_VertexChildrenMap.end();
		++vert)
	{
		Vertex* workingVertex				= vert->first;
		TriangleList& possibleNeighbors		= vert->second;

		ProcessTangents		( possibleNeighbors, theVerts, mappingNewToOldVert, workingVertex );
		ProcessBinormals	( possibleNeighbors, theVerts, mappingNewToOldVert, workingVertex );
	}

	UpdateTheIndicesWithFinalIndices	( theIndices );
	OrthogonalizeTangentsAndBinormals	( theVerts );
		
	return true;
}

void MeshMender::BuildGroups(	Triangle* tri, //the tri of interest
								TriangleList const& possibleNeighbors, //all tris arround a vertex
								NeighborGroupList& neighborGroups, //the neighbor groups to be updated
								vector< Vertex > const& theVerts,
								CanSmoothChecker* smoothChecker)
{
	if( (!tri)  ||  (tri->handled) )
		return;
	
	Triangle* neighbor1 = NULL;
	Triangle* neighbor2 = NULL;

	FindNeighbors(tri, possibleNeighbors, &neighbor1, &neighbor2,theVerts);
	
	//see if I can join my first neighbors group
	if(neighbor1 && (neighbor1->group != NO_GROUP))
	{
		// this was solved by maya engine before 
		//if( smoothChecker->CanSmooth(tri,neighbor1, minCreaseAngle) )
		{
			neighborGroups[neighbor1->group].push_back(tri->myID);
			tri->group = neighbor1->group;
		}
	}

	//see if I can join my second neighbors group
	if(neighbor2 && (neighbor2->group!= NO_GROUP))
	{
		// this was solved by maya engine before 
		//if( smoothChecker->CanSmooth(tri,neighbor2, minCreaseAngle) )
		{
			neighborGroups[neighbor2->group].push_back(tri->myID);
			tri->group = neighbor2->group;
		}
	}
	//I either couldn't join, or they weren't in a group, so I think I'll
	//just go and start my own group...right here we go.
	if(tri->group == NO_GROUP)
	{
		tri->group = neighborGroups.size();
		neighborGroups.push_back( TriangleList() );
		neighborGroups.back().push_back(tri->myID);

	}
	R_ASSERT((tri->group != NO_GROUP) && "error!: tri should have a group set");
	tri->handled = true;

	//continue growing our group with each neighbor.
	BuildGroups	( neighbor1, possibleNeighbors, neighborGroups, theVerts, smoothChecker );
	BuildGroups	( neighbor2, possibleNeighbors, neighborGroups, theVerts, smoothChecker );
}


void MeshMender::FindNeighbors(
							   Triangle* tri, 
							   TriangleList const& possibleNeighbors, 
							   Triangle** neighbor1, 
							   Triangle** neighbor2,
							   vector< Vertex > const& theVerts)
{
	*neighbor1 = NULL;
	*neighbor2 = NULL;

	vector<Triangle*> theNeighbors;
	for(u32 n = 0; n < possibleNeighbors.size(); ++n)
	{
		TriID tID = possibleNeighbors[n];
		Triangle* possible =&(m_Triangles[tID]);
		if(possible != tri ) //check for myself
		{
			if( SharesEdge(tri, possible, theVerts)  )
				theNeighbors.push_back(possible);  
		}
	}
	
	if(theNeighbors.size()>0)
		*neighbor1 = theNeighbors[0];
	if(theNeighbors.size()>1)
		*neighbor2 = theNeighbors[1];
}

bool MeshMender::SharesEdge(	
							Triangle* triA, 
							Triangle* triB,
							vector< Vertex > const& theVerts)
{
	XRAY_UNREFERENCED_PARAMETERS(theVerts);
	R_ASSERT(triA && triB && "invalid data passed to SharesEdge");

	size_t a0 = triA->indices[0];
	size_t a1 = triA->indices[1];
	size_t a2 = triA->indices[2];

	size_t b0 = triB->indices[0];
	size_t b1 = triB->indices[1];
	size_t b2 = triB->indices[2];

	bool a0_a1 = (a0==b0 && a1==b1) || (a0==b1 && a1==b2) || (a0==b2 && a1==b0);
	bool a2_a0 = (a2==b2 && a0==b0) || (a2==b0 && a0==b1) || (a2==b1 && a0==b2);
	bool a1_a2 = (a1==b1 && a2==b2) || (a1==b2 && a2==b0) || (a1==b0 && a2==b1);
	return a0_a1 || a2_a0 || a1_a2;
}

void MeshMender::SetUpData(
						  vector< Vertex >&    theVerts,
						  const vector< u16 >& theIndices,
						  vector< u16 >& mappingNewToOldVert,
						  const NormalCalcOption computeNormals)
{
	R_ASSERT( ((theIndices.size()%3 )== 0) && "expected the indices to be a multiple of 3");

	//initialize the mapping
	for(u16 ivert = 0 ; ivert< theVerts.size() ; ++ivert )
		mappingNewToOldVert.push_back( ivert );

	m_originalNumVerts = theVerts.size();

	//set up our triangles
	for(u32 iidx = 0; iidx< theIndices.size() ; iidx+= 3 )
	{
		Triangle t;

		t.indices[0] = theIndices[iidx+0];
		t.indices[1] = theIndices[iidx+1];
		t.indices[2] = theIndices[iidx+2];

		//set up bin, norm, and tan
		SetUpFaceVectors		( t, theVerts, computeNormals );
		
		t.myID					= m_Triangles.size();//set id, to my index into m_Triangles
		m_Triangles.push_back	( t );
	}

	//build vertex position/traingle pairings.
	//we use the position and not the actual vertex, because there may
	//be multiple coppies of the same vertex for textureing
	//but we don't want that to 
	//effect our decisions about normal smoothing.  
	//note: maybe this should be an option, the position thing.
	for(u16 itri= 0; itri< m_Triangles.size(); ++itri )
	{

		for(u32 indx = 0 ; indx < 3 ; ++indx )
		{
			Vertex* v		= &(theVerts[m_Triangles[itri].indices[indx]]);

			VertexChildrenMap::iterator iter = m_VertexChildrenMap.find( v );
			if(iter != m_VertexChildrenMap.end())
			{
				iter->second.push_back(TriID(itri));
			}else
			{
				//we didn't find it so join whatever was there.
				vector<TriID> tmp;
				m_VertexChildrenMap[v] = tmp;
				m_VertexChildrenMap[v].push_back( TriID(itri) );
			}
		}
	}
}

//sets up the normal, binormal, and tangent for a triangle
//assumes the triangle indices are set to match whats in the verts
void MeshMender::SetUpFaceVectors(Triangle& t, 
								  const vector< Vertex >&verts, 
								  const NormalCalcOption computeNormals)
{

	if(computeNormals == CALCULATE_NORMALS)
	{ 
		xray::math::float3 edge0 = verts[t.indices[1]].pos - verts[t.indices[0]].pos;
		xray::math::float3 edge1 = verts[t.indices[2]].pos - verts[t.indices[0]].pos;

		t.normal = edge0.cross_product(edge1);

		if( WeightNormalsByArea < 1.0f )
		{
			
			xray::math::float3 normalizedNorm	= t.normal;
			normalizedNorm.normalize();
			xray::math::float3 finalNorm = (normalizedNorm * (1.0f - WeightNormalsByArea))
							+ (t.normal * WeightNormalsByArea);
			t.normal = finalNorm;
		}
	}
	//need to set up tangents, and binormals here
	GetGradients(  verts[ t.indices[0] ],
                   verts[ t.indices[1] ],
                   verts [t.indices[2] ],
                   t.tangent,
                   t.binormal);

	R_ASSERT	( valid(t.tangent) );
	R_ASSERT	( valid(t.binormal) );
}


bool MeshMender::OrthogonalizeTangentsAndBinormals( vector< Vertex >&   theVerts )
{
	//put our tangents and binormals through the final orthogonalization
	//with the final processed normals
	u32 len = theVerts.size();
	for(u32 i = 0 ; i < len ; ++ i )
	{
		//now with T and B and N we can get from tangent space to object space
		//but we want to go the other way, so we need the inverse
		//of the T, B,N matrix
		//we can use the Gram-Schmidt algorithm to find the newTangent and the newBinormal
		//newT = T - (N dot T)N
		//newB = B - (N dot B)N - (newT dot B)newT

		//NOTE: this should maybe happen with the final smoothed N, T, and B
		//will try it here and see what the results look like

		xray::math::float3 tmpNorm = theVerts[i].normal;
		xray::math::float3 tmpTan = theVerts[i].tangent;
		xray::math::float3 tmpBin = theVerts[i].binormal;

		R_ASSERT(valid(theVerts[i].normal));
		R_ASSERT(valid(theVerts[i].tangent));
		R_ASSERT(valid(theVerts[i].binormal));

		xray::math::float3 newT = tmpTan -  (tmpNorm.dot_product(tmpTan)  * tmpNorm );
		
		xray::math::float3 newB = tmpBin - (tmpNorm.dot_product(tmpBin) * tmpNorm)
							- (newT.dot_product(tmpBin)*newT);

		R_ASSERT(valid(newT));
		R_ASSERT(valid(newB));

		theVerts[i].tangent = newT;
		theVerts[i].tangent.normalize_safe(tmpNorm);

		if(newB.length()<0.001f)
			newB = newT;

		theVerts[i].binormal = newB;
		theVerts[i].binormal.normalize_safe(tmpNorm);

		//this is where we can do a final check for zero length vectors
		//and set them to something appropriate
		float lenTan = theVerts[i].tangent.length();
		float lenBin = theVerts[i].binormal.length();

		if( (lenTan <= 0.001f) || (lenBin <= 0.001f)  ) //should be approx 1.0f
		{	
			//the tangent space is ill defined at this vertex
			//so we can generate a valid one based on the normal vector,
			//which I'm assuming is valid!

			if(lenTan > 0.5f)
			{
				//the tangent is valid, so we can just use that
				//to calculate the binormal
				theVerts[i].binormal = theVerts[i].normal.cross_product(theVerts[i].tangent);
			}
			else if(lenBin > 0.5)
			{
				//the binormal is good and we can use it to calculate
				//the tangent
				theVerts[i].tangent = theVerts[i].binormal.cross_product(theVerts[i].normal);
			}
			else
			{
				//both vectors are invalid, so we should create something
				//that is at least valid if not correct
				xray::math::float3 xAxis( 1.0f , 0.0f , 0.0f);
				xray::math::float3 yAxis( 0.0f , 1.0f , 0.0f);
				//I'm checking two possible axis, because the normal could be one of them,
				//and we want to chose a different one to start making our valid basis.
				//I can find out which is further away from it by checking the dot product
				xray::math::float3 startAxis;

				if( xAxis.dot_product(theVerts[i].normal) <  yAxis.dot_product(theVerts[i].normal) )
				{
					//the xAxis is more different than the yAxis when compared to the normal
					startAxis = xAxis;
				}
				else
				{
					//the yAxis is more different than the xAxis when compared to the normal
					startAxis = yAxis;
				}

				theVerts[i].tangent = theVerts[i].normal.cross_product(startAxis);
				theVerts[i].binormal = theVerts[i].normal.cross_product(theVerts[i].tangent);
			}
		}
		else
		{
			//one final sanity check, make sure that they tangent and binormal are different enough
			if( theVerts[i].binormal.dot_product(theVerts[i].tangent)  > 0.999f )
			{
				//then they are too similar lets make them more different
				theVerts[i].binormal = theVerts[i].normal.cross_product(theVerts[i].tangent);
			}

		}

		R_ASSERT(valid(theVerts[i].normal));
		R_ASSERT(valid(theVerts[i].tangent));
		R_ASSERT(valid(theVerts[i].binormal));
	}

	return					true;
}

void MeshMender::GetGradients( const MeshMender::Vertex& v0,
                               const MeshMender::Vertex& v1,
                               const MeshMender::Vertex& v2,
                               xray::math::float3& tangent,
                               xray::math::float3& binormal) const
{
	//using Eric Lengyel's approach with a few modifications
	//from Mathematics for 3D Game Programmming and Computer Graphics
	// want to be able to trasform a vector in Object Space to Tangent Space
	// such that the x-axis cooresponds to the 's' direction and the
	// y-axis corresponds to the 't' direction, and the z-axis corresponds
	// to <0,0,1>, straight up out of the texture map

	//let P = v1 - v0
	xray::math::float3 P = v1.pos - v0.pos;
	//let Q = v2 - v0
	xray::math::float3 Q = v2.pos - v0.pos;
	float s1 = v1.s - v0.s;
	float t1 = v1.t - v0.t;
	float s2 = v2.s - v0.s;
	float t2 = v2.t - v0.t;


	//we need to solve the equation
	// P = s1*T + t1*B
	// Q = s2*T + t2*B
	// for T and B


	//this is a linear system with six unknowns and six equatinos, for TxTyTz BxByBz
	//[px,py,pz] = [s1,t1] * [Tx,Ty,Tz]
	// qx,qy,qz     s2,t2     Bx,By,Bz

	//multiplying both sides by the inverse of the s,t matrix gives
	//[Tx,Ty,Tz] = 1/(s1t2-s2t1) *  [t2,-t1] * [px,py,pz]
	// Bx,By,Bz                      -s2,s1	    qx,qy,qz  

	//solve this for the unormalized T and B to get from tangent to object space

	
	float tmp = 0.0f;
	if(xray::math::abs(s1*t2 - s2*t1) <= 0.0001f)
	{
		tmp = (s1*t2 - s2*t1) > 0.f ? 1.0f : -1.f;
	}
	else
	{
		tmp = 1.0f/(s1*t2 - s2*t1 );
	}
	
	tangent.x = (t2*P.x - t1*Q.x);
	tangent.y = (t2*P.y - t1*Q.y);
	tangent.z = (t2*P.z - t1*Q.z);
	
	tangent = tmp * tangent;

	binormal.x = (s1*Q.x - s2*P.x);
	binormal.y = (s1*Q.y - s2*P.y);
	binormal.z = (s1*Q.z - s2*P.z);

	binormal = tmp * binormal;

	//after these vectors are smoothed together,
	//they must be again orthogonalized with the final normals
	// see OrthogonalizeTangentsAndBinormals

}

void MeshMender::UpdateTheIndicesWithFinalIndices( vector< u16 >& theIndices )
{
	//theIndices is assumed to be filled with a copy of the in Indices.

	R_ASSERT(((theIndices.size()/3) == m_Triangles.size()) && "invalid number of tris, or indices.");
	//Note that we do not change the number or the order of indices at all,
	//so we just need to copy the triangles indices to the output.
	u32 oIndex = 0;

	for( u32 i = 0; i < m_Triangles.size();++i )
	{
		theIndices[oIndex+0] = m_Triangles[i].indices[0] & 0xffff;
		theIndices[oIndex+1] = m_Triangles[i].indices[1] & 0xffff;
		theIndices[oIndex+2] = m_Triangles[i].indices[2] & 0xffff;
		oIndex				+= 3;
	}
}




void MeshMender::AppendToMapping(	const u32 oldIndex,
									const u32 originalNumVerts,
									vector< u16 >& mappingNewToOldVert)
{
	if( oldIndex >= originalNumVerts )
	{
		//then this is a newer vertex we are mapping to another vertex we created in meshmender.
		//we need to find the original old vertex index to map to.
		// so we can just use the mapping
	
		//that is to say, just keep the same mapping for this new one.
		u16 originalVertIndex = mappingNewToOldVert[oldIndex] & 0xffff;
		R_ASSERT( originalVertIndex < originalNumVerts );
		
		mappingNewToOldVert.push_back( originalVertIndex ); 
	}
	else
	{
		//this is mapping to an original vertex
		mappingNewToOldVert.push_back( (u16)oldIndex );
	}
}