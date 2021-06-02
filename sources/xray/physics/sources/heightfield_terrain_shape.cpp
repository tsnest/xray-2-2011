////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "heightfield_terrain_shape.h"

namespace xray {
namespace physics {


heightfield_terrain_shape::heightfield_terrain_shape( int heightStickWidth, 
														int heightStickLength, 
														btScalar* heightfieldData,
														btScalar minHeight, 
														btScalar maxHeight,
														bool flipQuadEdges )
{
	initialize(	heightStickWidth, heightStickLength, 
				heightfieldData,
				minHeight, maxHeight, 
				flipQuadEdges );
}




void heightfield_terrain_shape::initialize( int heightStickWidth, 
											int heightStickLength, 
											btScalar* heightfieldData,
											btScalar minHeight, 
											btScalar maxHeight, 
											bool flipQuadEdges )
{
	// validation
	btAssert(heightStickWidth > 1 && "bad width");
	btAssert(heightStickLength > 1 && "bad length");
	btAssert(heightfieldData && "null heightfield data");
	// btAssert(heightScale) -- do we care?  Trust caller here
	btAssert(minHeight <= maxHeight && "bad min/max height");

	// initialize member variables
	m_shapeType = TERRAIN_SHAPE_PROXYTYPE;
	m_heightStickWidth = heightStickWidth;
	m_heightStickLength = heightStickLength;
	m_minHeight = minHeight;
	m_maxHeight = maxHeight;
	m_width = (btScalar) (heightStickWidth - 1);
	m_length = (btScalar) (heightStickLength - 1);
	m_heightfieldDataFloat = heightfieldData;
	m_flipQuadEdges = flipQuadEdges;
	m_useDiamondSubdivision = false;
	m_localScaling.setValue(btScalar(1.), btScalar(1.), btScalar(1.));

	m_localAabbMin.setValue(0, m_minHeight, 0);
	m_localAabbMax.setValue(m_width, m_maxHeight, m_length);

	// remember origin (defined as exact middle of aabb)
	m_localOrigin = btScalar(0.5) * (m_localAabbMin + m_localAabbMax);
}



heightfield_terrain_shape::~heightfield_terrain_shape()
{
}


void heightfield_terrain_shape::getAabb(const btTransform& t,btVector3& aabbMin,btVector3& aabbMax) const
{
	btVector3 halfExtents = (m_localAabbMax-m_localAabbMin)* m_localScaling * btScalar(0.5);

	btVector3 localOrigin(0, 0, 0);
	localOrigin[1] = (m_minHeight + m_maxHeight) * btScalar(0.5);
	localOrigin *= m_localScaling;

	btMatrix3x3 abs_b = t.getBasis().absolute();  
	btVector3 center = t.getOrigin();
	btVector3 extent = btVector3(abs_b[0].dot(halfExtents),
		   abs_b[1].dot(halfExtents),
		  abs_b[2].dot(halfExtents));
	extent += btVector3(getMargin(),getMargin(),getMargin());

	aabbMin = center - extent;
	aabbMax = center + extent;
}


/// This returns the "raw" (user's initial) height, not the actual height.
/// The actual height needs to be adjusted to be relative to the center
///   of the heightfield's AABB.
btScalar heightfield_terrain_shape::getRawHeightFieldValue(int x,int y) const
{
	btScalar val = m_heightfieldDataFloat[(y*m_heightStickWidth)+x];
	return val;
}




/// this returns the vertex in bullet-local coordinates
void heightfield_terrain_shape::getVertex(int x,int y,btVector3& vertex) const
{
	btAssert(x>=0);
	btAssert(y>=0);
	btAssert(x<m_heightStickWidth);
	btAssert(y<m_heightStickLength);

	btScalar	height = getRawHeightFieldValue(x,y);

	vertex.setValue( 	(-m_width/btScalar(2.0)) + x,
						height - m_localOrigin.getY(),
						(-m_length/btScalar(2.0)) + y	);

	vertex*=m_localScaling;
}



static inline int getQuantized ( btScalar x )
{
	if (x < 0.0) {
		return (int) (x - 0.5);
	}
	return (int) (x + 0.5);
}



/// given input vector, return quantized version
/**
  This routine is basically determining the gridpoint indices for a given
  input vector, answering the question: "which gridpoint is closest to the
  provided point?".

  "with clamp" means that we restrict the point to be in the heightfield's
  axis-aligned bounding box.
 */
void heightfield_terrain_shape::quantizeWithClamp(int* out, const btVector3& point,int /*isMax*/) const
{
	btVector3 clampedPoint(point);
	clampedPoint.setMax(m_localAabbMin);
	clampedPoint.setMin(m_localAabbMax);

	out[0] = getQuantized(clampedPoint.getX());
	out[1] = getQuantized(clampedPoint.getY());
	out[2] = getQuantized(clampedPoint.getZ());
		
}



/// process all triangles within the provided axis-aligned bounding box
/**
  basic algorithm:
    - convert input aabb to local coordinates (scale down and shift for local origin)
    - convert input aabb to a range of heightfield grid points (quantize)
    - iterate over all triangles in that subset of the grid
 */
void heightfield_terrain_shape::processAllTriangles( btTriangleCallback* callback,
													const btVector3& aabbMin,
													const btVector3& aabbMax ) const
{
	// scale down the input aabb's so they are in local (non-scaled) coordinates
	btVector3	localAabbMin = aabbMin*btVector3(1.f/m_localScaling[0],1.f/m_localScaling[1],1.f/m_localScaling[2]);
	btVector3	localAabbMax = aabbMax*btVector3(1.f/m_localScaling[0],1.f/m_localScaling[1],1.f/m_localScaling[2]);

	// account for local origin
	localAabbMin += m_localOrigin;
	localAabbMax += m_localOrigin;

	//quantize the aabbMin and aabbMax, and adjust the start/end ranges
	int	quantizedAabbMin[3];
	int	quantizedAabbMax[3];
	quantizeWithClamp(quantizedAabbMin, localAabbMin,0);
	quantizeWithClamp(quantizedAabbMax, localAabbMax,1);
	
	// expand the min/max quantized values
	// this is to catch the case where the input aabb falls between grid points!
	for (int i = 0; i < 3; ++i) {
		quantizedAabbMin[i]--;
		quantizedAabbMax[i]++;
	}	

	int startX=0;
	int endX=m_heightStickWidth-1;
	int startJ=0;
	int endJ=m_heightStickLength-1;

	{
		if (quantizedAabbMin[0]>startX)
			startX = quantizedAabbMin[0];
		if (quantizedAabbMax[0]<endX)
			endX = quantizedAabbMax[0];
		if (quantizedAabbMin[2]>startJ)
			startJ = quantizedAabbMin[2];
		if (quantizedAabbMax[2]<endJ)
			endJ = quantizedAabbMax[2];
	};
  

	for(int j=startJ; j<endJ; j++)
	{
		for(int x=startX; x<endX; x++)
		{
			btVector3 vertices[3];
			if (m_flipQuadEdges || (m_useDiamondSubdivision && !((j+x) & 1)))
			{
				//first triangle
				getVertex(x,j,vertices[0]);
				getVertex(x+1,j,vertices[1]);
				getVertex(x+1,j+1,vertices[2]);
				callback->processTriangle(vertices,x,j);
				//second triangle
				getVertex(x,j,vertices[0]);
				getVertex(x+1,j+1,vertices[1]);
				getVertex(x,j+1,vertices[2]);
				callback->processTriangle(vertices,x,j);				
			} else
			{
				//first triangle
				getVertex(x,j,vertices[0]);
				getVertex(x,j+1,vertices[1]);
				getVertex(x+1,j,vertices[2]);
				callback->processTriangle(vertices,x,j);
				//second triangle
				getVertex(x+1,j,vertices[0]);
				getVertex(x,j+1,vertices[1]);
				getVertex(x+1,j+1,vertices[2]);
				callback->processTriangle(vertices,x,j);
			}
		}
	}

	

}

void heightfield_terrain_shape::calculateLocalInertia(btScalar ,btVector3& inertia) const
{
	//moving concave objects not supported
	
	inertia.setValue(btScalar(0.),btScalar(0.),btScalar(0.));
}

void heightfield_terrain_shape::setLocalScaling(const btVector3& scaling)
{
	m_localScaling = scaling;
}

const btVector3& heightfield_terrain_shape::getLocalScaling() const
{
	return m_localScaling;
}

} // namespace physics
} // namespace xray