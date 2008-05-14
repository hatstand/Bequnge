#include "frustum.h"
#include "opengl.h"

#include <QtDebug>

Mat4 glGetMatrix(int matrix)
{
	float m[16];
	glGetFloatv(matrix, m);
	
	return Mat4(m[0], m[4], m[8],  m[12],
	            m[1], m[5], m[9],  m[13],
	            m[2], m[6], m[10], m[14],
	            m[3], m[7], m[11], m[15]);
}



// Taken from http://www.racer.nl/reference/vfc.htm

void Frustum::getPlaneEquations()
{
	Plane* p;
	int i;
	
	// Get matrices from OpenGL
	Mat4 matModelView(glGetMatrix(GL_MODELVIEW_MATRIX));
	Mat4 matProjection(glGetMatrix(GL_PROJECTION_MATRIX));
	Mat4 matFrustum(matProjection * matModelView);
	
	// Get plane parameters
	matFrustum = trans(matFrustum);
	Real* m = matFrustum.Ref();
	
	p=&frustumPlane[RIGHT];
	p->n[0]=m[3]-m[0];
	p->n[1]=m[7]-m[4];
	p->n[2]=m[11]-m[8];
	p->d=m[15]-m[12];
	
	p=&frustumPlane[LEFT];
	p->n[0]=m[3]+m[0];
	p->n[1]=m[7]+m[4];
	p->n[2]=m[11]+m[8];
	p->d=m[15]+m[12];
	
	p=&frustumPlane[BOTTOM];
	p->n[0]=m[3]+m[1];
	p->n[1]=m[7]+m[5];
	p->n[2]=m[11]+m[9];
	p->d=m[15]+m[13];
	
	p=&frustumPlane[TOP];
	p->n[0]=m[3]-m[1];
	p->n[1]=m[7]-m[5];
	p->n[2]=m[11]-m[9];
	p->d=m[15]-m[13];
	
	p=&frustumPlane[PFAR];
	p->n[0]=m[3]-m[2];
	p->n[1]=m[7]-m[6];
	p->n[2]=m[11]-m[10];
	p->d=m[15]-m[14];
	
	p=&frustumPlane[PNEAR];
	p->n[0]=m[3]+m[2];
	p->n[1]=m[7]+m[6];
	p->n[2]=m[11]+m[10];
	p->d=m[15]+m[14];
	
	// Normalize all plane normals
	for (i=0 ; i<6 ; i++)
		frustumPlane[i].normalize();
}

bool Frustum::testSphere(const Vec3& center, float radius)
{
	Plane* p;
	for(int i=0 ; i<6 ; i++)
	{
		p = &frustumPlane[i];
		if (p->n[0]*center[0] + p->n[1]*center[1] + p->n[2]*center[2] + p->d <= -radius)
			return false;
	}
	return true;
}
