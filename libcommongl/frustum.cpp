#include "frustum.h"
#include "matrix.h"
#include "opengl.h"

#include <QtDebug>

// Taken from http://www.racer.nl/reference/vfc.htm

void Frustum::getPlaneEquations()
{
	Plane* p;
	int i;
	
	// Get matrices from OpenGL
	mat4 matModelView(GL_MODELVIEW_MATRIX);
	mat4 matProjection(GL_PROJECTION_MATRIX);
	mat4 matFrustum(matProjection * matModelView);

	// Get plane parameters
	float* m = matFrustum.data();
	
	p=&frustumPlane[RIGHT];
	p->n.x=m[3]-m[0];
	p->n.y=m[7]-m[4];
	p->n.z=m[11]-m[8];
	p->d=m[15]-m[12];
	
	p=&frustumPlane[LEFT];
	p->n.x=m[3]+m[0];
	p->n.y=m[7]+m[4];
	p->n.z=m[11]+m[8];
	p->d=m[15]+m[12];
	
	p=&frustumPlane[BOTTOM];
	p->n.x=m[3]+m[1];
	p->n.y=m[7]+m[5];
	p->n.z=m[11]+m[9];
	p->d=m[15]+m[13];
	
	p=&frustumPlane[TOP];
	p->n.x=m[3]-m[1];
	p->n.y=m[7]-m[5];
	p->n.z=m[11]-m[9];
	p->d=m[15]-m[13];
	
	p=&frustumPlane[PFAR];
	p->n.x=m[3]-m[2];
	p->n.y=m[7]-m[6];
	p->n.z=m[11]-m[10];
	p->d=m[15]-m[14];
	
	p=&frustumPlane[PNEAR];
	p->n.x=m[3]+m[2];
	p->n.y=m[7]+m[6];
	p->n.z=m[11]+m[10];
	p->d=m[15]+m[14];
	
	// Normalize all plane normals
	for (i=0 ; i<6 ; i++)
		frustumPlane[i].normalize();
}

bool Frustum::testSphere(const vec3& center, float radius)
{
	Plane* p;
	for(int i=0 ; i<6 ; i++)
	{
		p = &frustumPlane[i];
		if (p->n.x*center.x + p->n.y*center.y + p->n.z*center.z + p->d <= -radius)
			return false;
	}
	return true;
}
