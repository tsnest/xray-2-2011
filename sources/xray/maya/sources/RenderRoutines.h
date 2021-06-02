//-----------------------------------------------------------------------------------------
/// \file	RenderRoutines.h
/// \author	Rob Bateman
///	\date	23-10-04
/// \brief	These are simple openGL drawing routines for a few basic primitives that i seem
/// 		to use time and again over the course of wanging locators into Maya. Whilst
///			drawing geometry with glBegin / glEnd is relatively simple, it is also very
/// 		repetetive! To save a bit of time, i want the more common geometry primitives
///			to be wrapped up in slightly higher level functions.
///
/// 		Since all of my locator nodes are likely to use this stuff, i might as well wrap
///			it in my own class, RenderRoutines. This class derives from the base class
///			MPxLocator and simply adds a few bits and bobs in over the surface. 
///
//-----------------------------------------------------------------------------------------

#ifndef __RENDER_ROUTINES__H__
#define __RENDER_ROUTINES__H__

class RenderRoutines
{
	public:
		/// \brief	Applies the user defined colour for the specified state
		static void ApplyStateColour(const float intensity,M3dView::DisplayStatus status);

		//	These functions draw a circle on the specified plane
		//	\param	r		-	the radius of the circle
		//	\param	divs	-	the number of divisions around the circle
		//
		static void DrawCircleXY(const float r,const int divs);
		static void DrawCircleXZ(const float r,const int divs);
		static void DrawCircleYZ(const float r,const int divs);

		static void DrawFilledCircleXY(const float r,const int divs);
		static void DrawFilledCircleXZ(const float r,const int divs);
		static void DrawFilledCircleYZ(const float r,const int divs);


		//	These functions draw a quad in the specified plane.
		//	\brief	w	-	the width
		//	\brief	h	-	the height
		//
		static void DrawQuadXY(const float w,const float h);
		static void DrawQuadXZ(const float w,const float h);
		static void DrawQuadYZ(const float w,const float h);

		static void DrawFilledQuadXY(const float w,const float h);
		static void DrawFilledQuadXZ(const float w,const float h);
		static void DrawFilledQuadYZ(const float w,const float h);


		//	These functions draw a 2 crossed lines in the specified plane.
		//	\brief	w	-	the width
		//	\brief	h	-	the height
		//
		static void DrawCrossXY(const float w,const float h);
		static void DrawCrossXZ(const float w,const float h);
		static void DrawCrossYZ(const float w,const float h);

		//	These functions draw an arc on the specified plane
		//	\param	sr		-	the start radius of the spiral
		//	\param	er		-	the end radius of the spiral
		//	\param	sa		-	the start angle in degrees
		// 	\param	ea		-	the end angle in degrees
		//	\param	h		-	the height of the spiral to draw
		//	\param	divs	-	the number of divisions around the circle
		//
		static void DrawSpiralXY(const float sr,const float er,const float sa,const float ea,const float h,const int divs);
		static void DrawSpiralXZ(const float sr,const float er,const float sa,const float ea,const float h,const int divs);
		static void DrawSpiralYZ(const float sr,const float er,const float sa,const float ea,const float h,const int divs);


		static void DrawSphere(const float r);
		static void DrawCone(const float r,const float h);
		static void DrawCube(const float w,const float h,const float d);
		static void DrawFilledCube(const float w,const float h,const float d);

		//	These functions draw a 2 crossed lines in the specified plane.
		//	\brief	r	-	the radius
		//	\brief	h	-	the height
		//
		static void DrawCylinderXY(const float r,const float h);
		static void DrawCylinderXZ(const float r,const float h);
		static void DrawCylinderYZ(const float r,const float h);
	};


#endif

