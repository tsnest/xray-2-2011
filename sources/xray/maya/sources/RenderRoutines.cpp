//-----------------------------------------------------------------------------------------
/// \file	RenderRoutines.cpp
/// \author	Rob Bateman
///	\date	23-10-04
/// \brief	These are simple openGL drawing routines for a few basic circle type primitives
///
//-----------------------------------------------------------------------------------------
#include "pch.h"
#include "RenderRoutines.h"

void RenderRoutines::DrawSphere(const float r) {
	DrawCircleXY(r,30);
	DrawCircleXZ(r,30);
	DrawCircleYZ(r,30);
}


//------------------------------------------------------------------------
//	These functions draw a 2 crossed lines in the specified plane.
//	\brief	w	-	the width
//	\brief	h	-	the height
//
void RenderRoutines::DrawCrossXY(const float w,const float h) 
{
	glBegin(GL_LINES);
		glVertex3f( -0.5f*w, 0, 0 );
		glVertex3f(  0.5f*w, 0, 0 );
		glVertex3f( 0,  0.5f*h, 0 );
		glVertex3f( 0, -0.5f*h, 0 );
	glEnd();
}

void RenderRoutines::DrawCrossXZ(const float w,const float h) 
{
	glBegin(GL_LINES);
		glVertex3f( -0.5f*w, 0, 0 );
		glVertex3f(  0.5f*w, 0, 0 );
		glVertex3f( 0,0,  0.5f*h );
		glVertex3f( 0,0, -0.5f*h );
	glEnd();
}

void RenderRoutines::DrawCrossYZ(const float w,const float h) 
{
	glBegin(GL_LINES);
		glVertex3f( 0, -0.5f*w, 0 );
		glVertex3f( 0,  0.5f*w, 0 );
		glVertex3f( 0, 0,  0.5f*h );
		glVertex3f( 0, 0, -0.5f*h );
	glEnd();
}

//------------------------------------------------------------------------
//	These functions draw a quad in the specified plane.
//	\brief	w	-	the width
//	\brief	h	-	the height
//
void RenderRoutines::DrawQuadXY(const float w,const float h) 
{
	glBegin(GL_LINE_LOOP);
		glVertex3f( -0.5f*w, -0.5f*h, 0 );
		glVertex3f(  0.5f*w, -0.5f*h, 0 );
		glVertex3f(  0.5f*w,  0.5f*h, 0 );
		glVertex3f( -0.5f*w,  0.5f*h, 0 );
	glEnd();
}

void RenderRoutines::DrawQuadXZ(const float w,const float h) 
{
	glBegin(GL_LINE_LOOP);
		glVertex3f( -0.5f*w, 0, -0.5f*h );
		glVertex3f(  0.5f*w, 0, -0.5f*h );
		glVertex3f(  0.5f*w, 0,  0.5f*h );
		glVertex3f( -0.5f*w, 0,  0.5f*h );
	glEnd();
}

void RenderRoutines::DrawQuadYZ(const float w,const float h) 
{
	glBegin(GL_LINE_LOOP);
		glVertex3f( 0, -0.5f*w, -0.5f*h );
		glVertex3f( 0,  0.5f*w, -0.5f*h );
		glVertex3f( 0,  0.5f*w,  0.5f*h );
		glVertex3f( 0, -0.5f*w,  0.5f*h );
	glEnd();
}

void RenderRoutines::DrawFilledQuadXY(const float w,const float h) 
{
	glBegin(GL_QUADS);
		glVertex3f( -0.5f*w, -0.5f*h, 0 );
		glVertex3f(  0.5f*w, -0.5f*h, 0 );
		glVertex3f(  0.5f*w,  0.5f*h, 0 );
		glVertex3f( -0.5f*w,  0.5f*h, 0 );
	glEnd();
}

void RenderRoutines::DrawFilledQuadXZ(const float w,const float h) 
{
	glBegin(GL_QUADS);
		glVertex3f( -0.5f*w, 0, -0.5f*h );
		glVertex3f(  0.5f*w, 0, -0.5f*h );
		glVertex3f(  0.5f*w, 0,  0.5f*h );
		glVertex3f( -0.5f*w, 0,  0.5f*h );
	glEnd();
}
void RenderRoutines::DrawFilledQuadYZ(const float w,const float h) {
	glBegin(GL_QUADS);
		glVertex3f( 0, -0.5f*w, -0.5f*h );
		glVertex3f( 0,  0.5f*w, -0.5f*h );
		glVertex3f( 0,  0.5f*w,  0.5f*h );
		glVertex3f( 0, -0.5f*w,  0.5f*h );
	glEnd();
}

void RenderRoutines::DrawCube(const float w,const float h,const float d)
{
	glPushMatrix();
		glTranslatef(0,0,d/2);
		DrawQuadXY(w,h);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,0,-d/2);
		DrawQuadXY(w,h);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0,h/2,0);
		DrawQuadXZ(w,d);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,-h/2,0);
		DrawQuadXZ(w,d);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(w/2,0,0);
		DrawQuadYZ(h,d);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-w/2,0,0);
		DrawQuadYZ(h,d);
	glPopMatrix();
}

void RenderRoutines::DrawFilledCube(const float w,const float h,const float d) 
{
	glPushMatrix();
		glTranslatef(0,0,d/2);
		DrawFilledQuadXY(w,h);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,0,-d/2);
		DrawFilledQuadXY(w,h);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0,h/2,0);
		DrawFilledQuadXZ(w,d);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,-h/2,0);
		DrawFilledQuadXZ(w,d);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(w/2,0,0);
		DrawFilledQuadYZ(h,d);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-w/2,0,0);
		DrawFilledQuadYZ(h,d);
	glPopMatrix();
}

//------------------------------------------------------------------------
//	These functions draw a 2 crossed lines in the specified plane.
//	\brief	r	-	the radius
//	\brief	h	-	the height
//
void RenderRoutines::DrawCylinderXY(const float r,const float h)
{
	glPushMatrix();
		glTranslatef(0,0,h/2);
		DrawCircleXY(r,30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,0,-h/2);
		DrawCircleXY(r,30);
	glPopMatrix();

	glBegin(GL_LINES);
		glVertex3f(r,0,-h/2);
		glVertex3f(r,0,h/2);

		glVertex3f(-r,0,-h/2);
		glVertex3f(-r,0,h/2);

		glVertex3f(0,r,-h/2);
		glVertex3f(0,r,h/2);

		glVertex3f(0,-r,-h/2);
		glVertex3f(0,-r,h/2);
	glEnd();
}

void RenderRoutines::DrawCone(const float r,const float h)
{
	glPushMatrix();
		glTranslatef(0,-h/2,0);
		DrawCircleXZ(r,30);
	glPopMatrix();

	glBegin(GL_LINES);
		glVertex3f(r,-h/2,0);
		glVertex3f(0,h/2,0);

		glVertex3f(-r,-h/2,0);
		glVertex3f(0,h/2,0);

		glVertex3f(0,-h/2,r);
		glVertex3f(0,h/2,0);

		glVertex3f(0,-h/2,-r);
		glVertex3f(0,h/2,0);
	glEnd();
}

void RenderRoutines::DrawCylinderXZ(const float r,const float h)
{
	glPushMatrix();
		glTranslatef(0,h/2,0);
		DrawCircleXZ(r,30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,-h/2,0);
		DrawCircleXZ(r,30);
	glPopMatrix();

	glBegin(GL_LINES);
		glVertex3f(r,-h/2,0);
		glVertex3f(r,h/2,0);

		glVertex3f(-r,-h/2,0);
		glVertex3f(-r,h/2,0);

		glVertex3f(0,-h/2,r);
		glVertex3f(0,h/2,r);

		glVertex3f(0,-h/2,-r);
		glVertex3f(0,h/2,-r);
	glEnd();
}

void RenderRoutines::DrawCylinderYZ(const float r,const float h)
{
	glPushMatrix();
		glTranslatef(h/2,0,0);
		DrawCircleYZ(r,30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-h/2,0,0);
		DrawCircleYZ(r,30);
	glPopMatrix();

	glBegin(GL_LINES);
		glVertex3f(-h/2,r,0);
		glVertex3f(h/2,r,0);

		glVertex3f(-h/2,-r,0);
		glVertex3f(h/2,-r,0);

		glVertex3f(-h/2,0,r);
		glVertex3f(h/2,0,r);

		glVertex3f(-h/2,0,-r);
		glVertex3f(h/2,0,-r);
	glEnd();
}

//------------------------------------------------------------------------
//	These functions draw a circle on the specified plane
//	\param	r		-	the radius of the circle
//	\param	divs	-	the number of divisions around the circle
//
void RenderRoutines::DrawCircleXY(const float r,const int divs) 
{
	glBegin(GL_LINE_LOOP);
	for(int i=0;i!=divs;++i)
	{
		float angle = (i*360.0f/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(ca*r,sa*r,0.0f);
	}
	glEnd();
}

void RenderRoutines::DrawCircleXZ(const float r,const int divs)
{
	glBegin(GL_LINE_LOOP);
	for(int i=0;i!=divs;++i)
	{
		float angle = (i*360.0f/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(ca*r,0.0f,sa*r);
	}
	glEnd();
}

void RenderRoutines::DrawCircleYZ(const float r,const int divs) {
	glBegin(GL_LINE_LOOP);
	for(int i=0;i!=divs;++i)
	{
		float angle = (i*360.0f/divs) * (3.1415926535898f/180.0f);
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(0.0f,ca*r,sa*r);
	}
	glEnd();
}

void RenderRoutines::DrawFilledCircleXY(const float r,const int divs) 
{
	glBegin(GL_POLYGON);
	for(int i=0;i!=divs;++i)
	{
		float angle = (i*360.0f/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(ca*r,sa*r,0.0f);
	}
	glEnd();
}

void RenderRoutines::DrawFilledCircleXZ(const float r,const int divs) 
{
	glBegin(GL_POLYGON);
	for(int i=0;i!=divs;++i)
	{
		float angle = (i*360.0f/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(ca*r,0.0f,sa*r);
	}
	glEnd();
}

void RenderRoutines::DrawFilledCircleYZ(const float r,const int divs)
{
	glBegin(GL_POLYGON);
	for(int i=0;i!=divs;++i)
	{
		float angle = (i*360.0f/divs) * (3.1415926535898f/180.0f);
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(0.0f,ca*r,sa*r);
	}
	glEnd();
}

//------------------------------------------------------------------------
//	These functions draw an arc on the specified plane
//	\param	r		-	the radius of the circle
//	\param	sa		-	the start angle in degrees
// 	\param	ea		-	the end angle in degrees
//	\param	h		-	the height of the spiral to draw
//	\param	divs	-	the number of divisions around the circle
//
void RenderRoutines::DrawSpiralXY(const float sr,const float er,const float sa,const float ea,const float h,const int divs)
{

	glPushMatrix();
		glTranslatef(0,0,-h/2);
		DrawCircleXY(sr,30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,0,h/2);
		DrawCircleXY(er,30);
	glPopMatrix();

	glBegin(GL_LINE_STRIP);
	for(int i=0;i!=divs;++i)
	{
		float t = (float) i / (divs-1);
		float r = (1-t)*sr + t*er;
		float angle = (i*(ea-sa)/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(ca*r,sa*r,t*h -h/2);
	}
	glEnd();
}

void RenderRoutines::DrawSpiralXZ(const float sr,const float er,const float sa,const float ea,const float h,const int divs)
{

	glPushMatrix();
		glTranslatef(0,-h/2,0);
		DrawCircleXZ(sr,30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0,h/2,0);
		DrawCircleXZ(er,30);
	glPopMatrix();

	glBegin(GL_LINE_STRIP);
	for(int i=0;i!=divs;++i)
	{
		float t = (float) i / (divs-1);
		float r = (1-t)*sr + t*er;
		float angle = (i*(ea-sa)/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(ca*r,t*h -h/2,sa*r);
	}
	glEnd();
}

void RenderRoutines::DrawSpiralYZ(const float sr,const float er,const float sa,const float ea,const float h,const int divs)
{

	glPushMatrix();
		glTranslatef(-h/2,0,0);
		DrawCircleXZ(sr,30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(h/2,0,0);
		DrawCircleXZ(er,30);
	glPopMatrix();

	glBegin(GL_LINE_STRIP);
	for(int i=0;i!=divs;++i)
	{
		float t = (float) i / (divs-1);
		float r = (1-t)*sr + t*er;
		float angle = (i*(ea-sa)/divs) * 3.1415926535898f/180.0f;
		float ca = cos(angle);
		float sa = sin(angle);
		glVertex3f(t*h -h/2,ca*r,sa*r);
	}
	glEnd();
}
