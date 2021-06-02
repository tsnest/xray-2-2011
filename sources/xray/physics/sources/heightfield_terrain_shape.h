////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HEIGHTFIELD_TERRAIN_SHAPE_H_INCLUDED
#define HEIGHTFIELD_TERRAIN_SHAPE_H_INCLUDED

#include "bullet_include.h"

namespace xray {
namespace physics {

class heightfield_terrain_shape : public btConcaveShape
{
protected:
	btVector3	m_localAabbMin;
	btVector3	m_localAabbMax;
	btVector3	m_localOrigin;
	btVector3	m_localScaling;

	///terrain data
	int			m_heightStickWidth;
	int			m_heightStickLength;
	btScalar	m_minHeight;
	btScalar	m_maxHeight;
	btScalar	m_width;
	btScalar	m_length;

	btScalar*	m_heightfieldDataFloat;

	bool		m_flipQuadEdges;
	bool		m_useDiamondSubdivision;


	virtual btScalar	getRawHeightFieldValue	(int x,int y) const;
	void		quantizeWithClamp				(int* out, const btVector3& point,int isMax) const;
	void		getVertex						(int x,int y,btVector3& vertex) const;

	/// protected initialization
	/**
	  Handles the work of constructors so that public constructors can be
	  backwards-compatible without a lot of copy/paste.
	 */
	void initialize( int heightStickWidth, int heightStickLength,
	                btScalar* heightfieldData, 
					btScalar minHeight, btScalar maxHeight, 
					bool flipQuadEdges );

public:
	/// preferred constructor
	/**
	  This constructor supports a range of heightfield
	  data types, and allows for a non-zero minimum height value.
	  heightScale is needed for any integer-based heightfield data types.
	 */
	heightfield_terrain_shape(int heightStickWidth,int heightStickLength,
	                          btScalar* heightfieldData,
	                          btScalar minHeight, btScalar maxHeight,
	                          bool flipQuadEdges);


	virtual ~heightfield_terrain_shape();


	void setUseDiamondSubdivision(bool useDiamondSubdivision=true) { m_useDiamondSubdivision = useDiamondSubdivision;}


	virtual void getAabb(const btTransform& t,btVector3& aabbMin,btVector3& aabbMax) const;

	virtual void	processAllTriangles(btTriangleCallback* callback,const btVector3& aabbMin,const btVector3& aabbMax) const;

	virtual void	calculateLocalInertia(btScalar mass,btVector3& inertia) const;

	virtual void	setLocalScaling(const btVector3& scaling);
	
	virtual const btVector3& getLocalScaling() const;
	
	//debugging
	virtual const char*	getName()const {return "HEIGHTFIELD";}

};

} // namespace physics
} // namespace xray

#endif // #ifndef HEIGHTFIELD_TERRAIN_SHAPE_H_INCLUDED