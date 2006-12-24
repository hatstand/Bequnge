// pDomain.h
//
// Copyright 1997-2006 by David K. McAllister
// http://www.cs.unc.edu/~davemc/Particle
//
// This file defines the pDomain class and all of the classes that derive from it.

#ifndef _pdomain_h
#define _pdomain_h

#include "pVec.h"

namespace PAPI
{

    /// A representation of a region of space.
    ///
    /// A Domain is a representation of a region of space. For example, the Source action uses a domain to describe the volume in which a particle will be created. A random point within the domain is chosen as the initial position of the particle. The Avoid, Sink and Bounce actions, for example, use domains to describe a volume in space for particles to steer around, die when they enter, or bounce off, respectively.
    ///
    /// Domains can be used to describe velocities. Picture the velocity vector as having its tail at the origin and its tip being in the domain. Domains can be used to describe colors in any three-valued color space. They can be used to describe three-valued sizes, such as Length, Width, Height.
    ///
    /// Several types of domains can be specified, such as points, lines, planes, discs, spheres, gaussian blobs, etc. Each subclass of the pDomain class represents a different kind of domain. These are described below.
    ///
    /// All domains support two basic operations. The first is Generate, which returns a random point in the domain.
    ///
    /// The second basic operation is Within, which tells whether a given point is within the domain.
    ///
    /// The application programmer never calls the Generate or Within functions, and will use the pDomain class and its derivatives solely as a way to communicate the domain to the API. The API's action commands will then perform operations on the domain, such as generating particles within it.
    class pDomain
    {
    public:
        virtual bool Within(const pVec &) const = 0; ///< Returns true if the given point is within the domain.
        virtual pVec Generate() const = 0; ///< Returns a random point in the domain.

        virtual pDomain *copy() const = 0; // Returns a pointer to a heap-allocated copy of the derived class

        virtual ~pDomain() {}
    };

    /// A single point.
    ///
    /// Generate always returns this point. Within always returns false.
    class PDPoint : public pDomain
    {
    public:
        pVec p;

    public:
        PDPoint(const pVec &p0)
        {
            p = p0;
        }

        ~PDPoint()
        {
        }

        bool Within(const pVec &pos) const /// Always returns false.
        {
            return false;
        }

        pVec Generate() const /// Always returns this point.
        {
            return p;
        }

        pDomain *copy() const
        {
            PDPoint *P = new PDPoint(*this);
            return P;
        }
    };

    /// A line segment.
    /// 
    /// e0 and e1 are the endpoints of the segment.
    /// 
    /// Generate returns a random point on this segment. Within always returns false.
    class PDLine : public pDomain
    {
    public:
        pVec p0, p1;

    public:
        PDLine(const pVec &e0, const pVec &e1)
        {
            p0 = e0;
            p1 = e1 - e0;
        }

        ~PDLine()
        {
        }

        bool Within(const pVec &pos) const /// Always returns false.
        {
            return false;
        }

        pVec Generate() const /// Returns a random point on this segment. 
        {
            return p0 + p1 * pRandf();
        }

        pDomain *copy() const
        {
            PDLine *P = new PDLine(*this);
            return P;
        }
    };

    /// A Triangle.
    /// 
    /// p0, p1, and p2 are the vertices of the triangle. The triangle can be used to define an arbitrary geometrical model for particles to bounce off, or generate particles on its surface (and explode them), etc.
    /// 
    /// Generate returns a random point in the triangle. Within always returns false. Currently it is not possible to sink particles that enter/exit a polygonal model. Suggestions?]
    class PDTriangle : public pDomain
    {
    public:
        pVec p, u, v, uNrm, vNrm, nrm;
        float uLen, vLen, D;

    public:
        PDTriangle(const pVec &p0, const pVec &p1, const pVec &p2)
        {
            p = p0;
            u = p1 - p0;
            v = p2 - p0;

            // The rest of this is needed for Avoid and Bounce.
            uLen = u.length();
            uNrm = u / uLen;
            vLen = v.length();
            vNrm = v / vLen;

            nrm = Cross(uNrm, vNrm); // This is the non-unit normal.
            nrm.normalize(); // Must normalize it.

            D = -dot(p, nrm);
        }

        ~PDTriangle()
        {
        }

        bool Within(const pVec &pos) const /// Always returns false.
        {
            return false;
        }

        pVec Generate() const /// Returns a random point in the triangle.
        {
            float r1 = pRandf();
            float r2 = pRandf();
            pVec pos;
            if(r1 + r2 < 1.0f)
                pos = p + u * r1 + v * r2;
            else
                pos = p + u * (1.0f-r1) + v * (1.0f-r2);

            return pos;
        }

        pDomain *copy() const
        {
            PDTriangle *P = new PDTriangle(*this);
            return P;
        }
    };

    /// Rhombus-shaped planar region.
    /// 
    /// p0 is a point on the plane. u0 and v0 are (non-parallel) basis vectors in the plane. They don't need to be normal or orthogonal.
    /// 
    /// Generate returns a random point in the diamond-shaped patch whose corners are o, o+u, o+u+v, and o+v. Within returns false.
    class PDRectangle : public pDomain
    {
    public:
        pVec p, u, v, uNrm, vNrm, nrm;
        float uLen, vLen, D;

    public:
        PDRectangle(const pVec &p0, const pVec &u0, const pVec &v0)
        {
            p = p0;
            u = u0;
            v = v0;

            // The rest of this is needed for Avoid and Bounce.
            uLen = u.length();
            uNrm = u / uLen;
            vLen = v.length();
            vNrm = v / vLen;

            nrm = Cross(uNrm, vNrm); // This is the non-unit normal.
            nrm.normalize(); // Must normalize it.

            D = -dot(p, nrm);
        }

        ~PDRectangle()
        {
            // std::cerr << "del " << typeid(*this).name() << this << std::endl;
        }

        bool Within(const pVec &pos) const /// Always returns false.
        {
            return false;
        }

        pVec Generate() const /// Returns a random point in the diamond-shaped patch whose corners are o, o+u, o+u+v, and o+v.
        {
            pVec pos = p + u * pRandf() + v * pRandf();
            return pos;
        }

        pDomain *copy() const
        {
            PDRectangle *P = new PDRectangle(*this);
            return P;
        }
    };

    /// Arbitrarily-oriented plane.
    /// 
    /// The point p0 is a point on the plane. Normal is the normal vector of the plane. If you have a plane in a,b,c,d form remember that n = [a,b,c] and you can compute a suitable point p0 as p0 = -n*d. The normal will get normalized, so it need not already be unit length.
    /// 
    /// Generate returns the point p0. Within returns true if the point is in the positive half-space of the plane (in the plane or on the side that Normal points to).
    class PDPlane : public pDomain
    {
    public:
        pVec p, nrm;
        float D;

    public:
        PDPlane(const pVec &p0, const pVec &Normal)
        {
            p = p0;
            nrm = Normal;
            nrm.normalize(); // Must normalize it.
            D = -dot(p, nrm);
        }

        ~PDPlane()
        {
        }

        // Distance from plane = n * p + d
        // Inside is the positive half-space.
        bool Within(const pVec &pos) const /// Returns true if the point is in the positive half-space of the plane (in the plane or on the side that Normal points to).
        {
            return dot(nrm, pos) >= -D;
        }

        // How do I sensibly make a point on an infinite plane?
        pVec Generate() const /// Returns the point p0.
        {
            return p;
        }

        pDomain *copy() const
        {
            PDPlane *P = new PDPlane(*this);
            return P;
        }
    };

    /// Axis-aligned box
    /// 
    /// e0 and e1 are opposite corners of an axis-aligned box. It doesn't matter which of each coordinate is min and which is max.
    /// 
    /// Generate returns a random point in this box. Within returns true if the point is in the box.
    /// 
    /// It is only possible to bounce particles off the outside of the box, not the inside. Likewise, particles can only Avoid the box from the outside. To use the Avoid action inside a box, define the box as six planes.
    class PDBox : public pDomain
    {
    public:
        // p0 is the min corner. p1 is the max corner.
        pVec p0, p1, dif;

    public:
        PDBox(const pVec &e0, const pVec &e1)
        {
            p0 = e0;
            p1 = e1;
            if(e1.x() < e0.x()) { p0.x() = e1.x(); p1.x() = e1.x(); }
            if(e1.y() < e0.y()) { p0.y() = e1.y(); p1.y() = e1.y(); }
            if(e1.z() < e0.z()) { p0.z() = e1.z(); p1.z() = e1.z(); }

            dif = p1 - p0;
        }

        ~PDBox()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point is in the box.
        {
            return !((pos.x() < p0.x()) || (pos.x() > p1.x()) ||
                (pos.y() < p0.y()) || (pos.y() > p1.y()) ||
                (pos.z() < p0.z()) || (pos.z() > p1.z()));
        }

        pVec Generate() const /// Returns a random point in this box.
        {
            // Scale and translate [0,1] random to fit box
            return p0 + CompMult(pRandVec(), dif);
        }

        pDomain *copy() const
        {
            PDBox *P = new PDBox(*this);
            return P;
        }
    };

    /// Cylinder
    /// 
    /// e0 and e1 are the endpoints of the axis of the cylinder. OuterRadius is the outer radius, and InnerRadius is the inner radius for a cylindrical shell. InnerRadius = 0 for a solid cylinder with no empty space in the middle.
    /// 
    /// Generate returns a random point in the cylindrical shell. Within returns true if the point is within the cylindrical shell.
    class PDCylinder : public pDomain
    {
    public:
        pVec apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
        float len, radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr;
        bool ThinShell;

    public:
        PDCylinder(const pVec &e0, const pVec &e1, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            apex = e0;
            axis = e1 - e0;

            if(OuterRadius < InnerRadius) {
                radOut = InnerRadius; radIn = OuterRadius;
            } else {
                radOut = OuterRadius; radIn = InnerRadius;
            }
            radOutSqr = fsqr(radOut);
            radInSqr = fsqr(radIn);

            ThinShell = (radIn == radOut);
            radDif = radOut - radIn;

            // Given an arbitrary nonzero vector n, make two orthonormal
            // vectors u and v forming a frame [u,v,n.normalize()].
            pVec n = axis;
            float axisLenSqr = axis.length2();
            axisLenInvSqr = axisLenSqr ? (1.0f / axisLenSqr) : 0.0f;
            n *= sqrtf(axisLenInvSqr);

            // Find a vector orthogonal to n.
            pVec basis(1.0f, 0.0f, 0.0f);
            if (fabsf(dot(basis, n)) > 0.999f)
                basis = pVec(0.0f, 1.0f, 0.0f);

            // Project away N component, normalize and cross to get
            // second orthonormal vector.
            u = basis - n * dot(basis, n);
            u.normalize();
            v = Cross(n, u);
        }

        ~PDCylinder()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point is within the cylindrical shell.
        {
            // This is painful and slow. Might be better to do quick accept/reject tests.
            // Axis is vector from base to tip of the cylinder.
            // x is vector from base to pos.
            //         x . axis
            // dist = ---------- = projected distance of x along the axis
            //        axis. axis   ranging from 0 (base) to 1 (tip)
            //
            // rad = x - dist * axis = projected vector of x along the base

            pVec x = pos - apex;

            // Check axial distance
            float dist = dot(axis, x) * axisLenInvSqr;
            if(dist < 0.0f || dist > 1.0f)
                return false;

            // Check radial distance
            pVec xrad = x - axis * dist; // Radial component of x
            float rSqr = xrad.length2();

            return (rSqr <= radInSqr && rSqr >= radOutSqr);
        }

        pVec Generate() const /// Returns a random point in the cylindrical shell.
        {
            float dist = pRandf(); // Distance between base and tip
            float theta = pRandf() * 2.0f * float(M_PI); // Angle around axis
            // Distance from axis
            float r = radIn + pRandf() * radDif;

            // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
            float x = r * cosf(theta);
            float y = r * sinf(theta);

            pVec pos = apex + axis * dist + u * x + v * y;
            return pos;
        }

        pDomain *copy() const
        {
            PDCylinder *P = new PDCylinder(*this);
            return P;
        }
    };

    ///  Cone
    /// 
    /// e0 is the apex of the cone and e1 is the other endpoint of the axis of the cone. OuterRadius is the radius of the base of the cone. InnerRadius is the radius of the base of a cone to subtract from the first cone to create a conical shell. This is similar to the cylindrical shell, which can be thought of as a large cylinder with a smaller cylinder subtracted from the middle. Both cones share the same apex and axis, which implies that the thickness of the conical shell tapers to 0 at the apex. InnerRadius = 0 for a solid cone with no empty space in the middle.
    /// 
    /// Generate returns a random point in the conical shell. Within returns true if the point is within the conical shell.
    class PDCone : public pDomain
    {
    public:
        pVec apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
        float len, radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr;
        bool ThinShell;

    public:
        PDCone(const pVec &e0, const pVec &e1, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            apex = e0;
            axis = e1 - e0;

            if(OuterRadius < InnerRadius) {
                radOut = InnerRadius; radIn = OuterRadius;
            } else {
                radOut = OuterRadius; radIn = InnerRadius;
            }
            radOutSqr = fsqr(radOut);
            radInSqr = fsqr(radIn);

            ThinShell = (radIn == radOut);
            radDif = radOut - radIn;

            // Given an arbitrary nonzero vector n, make two orthonormal
            // vectors u and v forming a frame [u,v,n.normalize()].
            pVec n = axis;
            float axisLenSqr = axis.length2();
            axisLenInvSqr = axisLenSqr ? 1.0f / axisLenSqr : 0.0f;
            n *= sqrtf(axisLenInvSqr);

            // Find a vector orthogonal to n.
            pVec basis(1.0f, 0.0f, 0.0f);
            if (fabsf(dot(basis, n)) > 0.999f)
                basis = pVec(0.0f, 1.0f, 0.0f);

            // Project away N component, normalize and cross to get
            // second orthonormal vector.
            u = basis - n * dot(basis, n);
            u.normalize();
            v = Cross(n, u);
        }

        ~PDCone()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point is within the conical shell.
        {
            // This is painful and slow. Might be better to do quick
            // accept/reject tests.
            // Let axis = vector from base to tip of the cylinder
            // x = vector from base to test point
            //         x . axis
            // dist = ---------- = projected distance of x along the axis
            //        axis. axis   ranging from 0 (base) to 1 (tip)
            //
            // rad = x - dist * axis = projected vector of x along the base

            pVec x = pos - apex;

            // Check axial distance
            // axisLenInvSqr stores 1 / dot(axis, axis)
            float dist = dot(axis, x) * axisLenInvSqr;
            if(dist < 0.0f || dist > 1.0f)
                return false;

            // Check radial distance; scale radius along axis for cones
            pVec xrad = x - axis * dist; // Radial component of x
            float rSqr = xrad.length2();

            return (rSqr <= fsqr(dist * radIn) && rSqr >= fsqr(dist * radOut));
        }

        pVec Generate() const /// Returns a random point in the conical shell. 
        {
            float dist = pRandf(); // Distance between base and tip
            float theta = pRandf() * 2.0f * float(M_PI); // Angle around axis
            // Distance from axis
            float r = radIn + pRandf() * radDif;

            // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
            float x = r * cosf(theta);
            float y = r * sinf(theta);

            // Scale radius along axis for cones
            x *= dist;
            y *= dist;

            pVec pos = apex + axis * dist + u * x + v * y;
            return pos;
        }

        pDomain *copy() const
        {
            PDCone *P = new PDCone(*this);
            return P;
        }
    };

    /// Sphere
    /// 
    /// The point Center is the center of the sphere. OuterRadius is the outer radius of the spherical shell and InnerRadius is the inner radius.
    /// 
    /// Generate returns a random point in the thick shell at a distance between OuterRadius and InnerRadius from point Center. If InnerRadius is 0, then it is the whole sphere. Within returns true if the point lies within the thick shell at a distance between InnerRadius to OuterRadius from point Center.
    class PDSphere : public pDomain
    {
    public:
        pVec ctr;
        float radOut, radIn, radOutSqr, radInSqr, radDif;
        bool ThinShell;

    public:
        PDSphere(const pVec &Center, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            ctr = Center;
            if(OuterRadius < InnerRadius) {
                radOut = InnerRadius; radIn = OuterRadius;
            } else {
                radOut = OuterRadius; radIn = InnerRadius;
            }
            if(radIn < 0.0f) radIn = 0.0f;

            radOutSqr = fsqr(radOut);
            radInSqr = fsqr(radIn);

            ThinShell = (radIn == radOut);
            radDif = radOut - radIn;
        }

        ~PDSphere()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point lies within the thick shell.
        {
            pVec rvec(pos - ctr);
            float rSqr = rvec.length2();
            return rSqr <= radOutSqr && rSqr >= radInSqr;
        }

        pVec Generate() const /// Returns a random point in the thick spherical shell.
        {
            pVec pos;

            do {
                pos = pRandVec() - vHalf; // Point on [-0.5,0.5] box
            } while (pos.length2() > fsqr(0.5)); // Make sure it's also on r=0.5 sphere.
            pos.normalize(); // Now it's on r=1 spherical shell

            // Scale unit sphere pos by [0..r] and translate
            if(ThinShell)
                pos = ctr + pos * radOut;
            else
                pos = ctr + pos * (radIn + pRandf() * radDif);

            return pos;
        }

        pDomain *copy() const
        {
            PDSphere *P = new PDSphere(*this);
            return P;
        }
    };

    /// Gaussian blob
    /// 
    /// The point Center is the center of a normal probability density of standard deviation StandardDev. The density is radially symmetrical. The blob domain allows for some very natural-looking effects because there is no sharp, artificial-looking boundary at the edge of the domain.
    /// 
    /// Generate returns a point with normal probability density. Within has a probability of returning true equal to the probability density at the specified point.
    class PDBlob : public pDomain
    {
    public:
        pVec ctr;
        float stdev, Scale1, Scale2;

    public:
        PDBlob(const pVec &Center, const float StandardDev)
        {
            ctr = Center;
            stdev = StandardDev;
            float oneOverSigma = 1.0f/(stdev+0.000000000001f);
            Scale1 = -0.5f*fsqr(oneOverSigma);
            Scale2 = P_ONEOVERSQRT2PI * oneOverSigma;
        }

        ~PDBlob()
        {
        }

        bool Within(const pVec &pos) const /// Has a probability of returning true equal to the probability density at the specified point.
        {
            pVec x = pos - ctr;
            // return exp(-0.5 * xSq * Sqr(oneOverSigma)) * P_ONEOVERSQRT2PI * oneOverSigma;
            float Gx = expf(x.length2() * Scale1) * Scale2;
            return (pRandf() < Gx);
        }

        pVec Generate() const /// Returns a point with normal probability density.
        {
            return ctr + pNRandVec(stdev);
        }

        pDomain *copy() const
        {
            PDBlob *P = new PDBlob(*this);
            return P;
        }
    };

    /// Arbitrarily-oriented disc
    /// 
    /// The point Center is the center of a disc in the plane with normal Normal. The disc has an OuterRadius. If InnerRadius is greater than 0, it the domain is a flat washer, rather than a disk. The normal will get normalized, so it need not already be unit length.
    /// 
    /// Generate returns a point inside the disc shell. Within returns false.
    class PDDisc : public pDomain
    {
    public:
        pVec p, nrm, u, v;
        float radIn, radOut, radInSqr, radOutSqr, dif, D;

    public:
        PDDisc(const pVec &Center, const pVec Normal, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            p = Center;
            nrm = Normal;
            nrm.normalize();

            if(OuterRadius > InnerRadius) {
                radOut = OuterRadius; radIn = InnerRadius;
            } else {
                radOut = InnerRadius; radIn = OuterRadius;
            }
            dif = radOut - radIn;
            radInSqr = fsqr(radIn);
            radOutSqr = fsqr(radOut);

            // Find a vector orthogonal to n.
            pVec basis(1.0f, 0.0f, 0.0f);
            if (fabsf(dot(basis, nrm)) > 0.999f)
                basis = pVec(0.0f, 1.0f, 0.0f);

            // Project away N component, normalize and cross to get
            // second orthonormal vector.
            u = basis - nrm * dot(basis, nrm);
            u.normalize();
            v = Cross(nrm, u);
            D = -dot(p, nrm);
        }

        ~PDDisc()
        {
        }

        bool Within(const pVec &pos) const /// Always returns false.
        {
            return false;
        }

        pVec Generate() const /// Returns a point inside the disc shell.
        {
            // Might be faster to generate a point in a square and reject if outside the circle
            float theta = pRandf() * 2.0f * float(M_PI); // Angle around normal
            // Distance from center
            float r = radIn + pRandf() * dif;

            float x = r * cosf(theta); // Weighting of each frame vector
            float y = r * sinf(theta);

            pVec pos = p + u * x + v * y;
            return pos;
        }

        pDomain *copy() const
        {
            PDDisc *P = new PDDisc(*this);
            return P;
        }
    };

};

#endif
