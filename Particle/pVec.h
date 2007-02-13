// pVec.h - yet another 3D vector class.
//
// Copyright 1997-2006 by David K. McAllister
// Based on code Copyright 1997 by Jonathan P. Leech
//
// A simple 3D float vector class for internal use by the particle systems.
//
// The new implementation of this class uses four floats. This allows the class to be implemented using
// SSE intrinsics for faster execution on P4 and AMD processors.

#ifndef _pvec_h
#define _pvec_h

#include <iostream>

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433f
#endif

#ifdef WIN32
#include <stdlib.h>
// This is because their stupid compiler thinks it's smart.
//#define inline __forceinline
#endif

namespace PAPI
{
    const float P_SQRT2PI = 2.506628274631000502415765284811045253006f;
    const float P_ONEOVERSQRT2PI = (1.f / P_SQRT2PI);

    inline float fsqr(float f) { return f * f; }

#ifdef unix
    inline float pRandf() { return drand48(); }
    inline void pSRandf(int x) { srand48(x); }
#else
    const float P_ONEOVER_RAND_MAX = (1.0f/((float) RAND_MAX));
    inline float pRandf() { return ((float) rand())*P_ONEOVER_RAND_MAX; }
    inline void pSRandf(int x) { srand(x); }
#endif

    inline bool pSameSign(const float &a, const float &b) { return a * b >= 0.0f; }
    inline bool pSameSignd(const float &a, const float &b)
    {
        int *fi = (int *)&a;
        int *gi = (int *)&b;
        return !(((*fi) ^ (*gi)) & 0x80000000);
    }

    /// Return a random number with a normal distribution.
    inline float pNRandf(float sigma = 1.0f)
    {
        const float P_ONE_OVER_SIGMA_EXP = (1.0f / 0.7975f);

        if(sigma == 0) return 0;

        float y;
        do {
            y = -logf(pRandf());
        }
        while(pRandf() > expf(-fsqr(y - 1.0f)*0.5f));

        if(rand() & 0x1)
            return y * sigma * P_ONE_OVER_SIGMA_EXP;
        else
            return -y * sigma * P_ONE_OVER_SIGMA_EXP;
    }

    /// A single-precision floating point three-vector.
    ///
    /// This class is used for packaging three floats for the application to pass into the API.
    ///
    /// This is also the class used internally to do vector math.
    class pVec
    {
        float vx, vy, vz;

    public:
        inline pVec(float ax, float ay, float az) : vx(ax), vy(ay), vz(az) {}
        inline pVec(float a) : vx(a), vy(a), vz(a) {}
        inline pVec() {}

        const float& x() const { return vx; }
        const float& y() const { return vy; }
        const float& z() const { return vz; }

        float& x() { return vx; }
        float& y() { return vy; }
        float& z() { return vz; }

        inline float length() const
        {
            return sqrtf(vx*vx+vy*vy+vz*vz);
        }

        inline float length2() const
        {
            return (vx*vx+vy*vy+vz*vz);
        }

        inline float normalize()
        {
            float onel = 1.0f / sqrtf(vx*vx+vy*vy+vz*vz);
            vx *= onel;
            vy *= onel;
            vz *= onel;

            return onel;
        }

        // Dot product
        friend inline float dot(const pVec &a, const pVec &b)
        {
            return b.x()*a.x() + b.y()*a.y() + b.z()*a.z();
        }

        // Scalar multiply
        inline pVec operator*(const float s) const
        {
            return pVec(vx*s, vy*s, vz*s);
        }

        inline pVec operator/(const float s) const
        {
            float invs = 1.0f / s;
            return pVec(vx*invs, vy*invs, vz*invs);
        }

        inline pVec operator+(const pVec& a) const
        {
            return pVec(vx+a.x(), vy+a.y(), vz+a.z());
        }

        inline pVec operator-(const pVec& a) const
        {
            return pVec(vx-a.x(), vy-a.y(), vz-a.z());
        }

        inline bool operator==(const pVec &a) const
        {
            return vx==a.x() && vy==a.y() && vz==a.z();
        }

        inline pVec operator-()
        {
            vx = -vx;
            vy = -vy;
            vz = -vz;
            return *this;
        }

        inline pVec& operator+=(const pVec& a)
        {
            vx += a.x();
            vy += a.y();
            vz += a.z();
            return *this;
        }

        inline pVec& operator-=(const pVec& a)
        {
            vx -= a.x();
            vy -= a.y();
            vz -= a.z();
            return *this;
        }

        inline pVec& operator*=(const float a)
        {
            vx *= a;
            vy *= a;
            vz *= a;
            return *this;
        }

        inline pVec& operator/=(const float a)
        {
            float b = 1.0f / a;
            vx *= b;
            vy *= b;
            vz *= b;
            return *this;
        }

        inline pVec& operator=(const pVec& a)
        {
            vx = a.x();
            vy = a.y();
            vz = a.z();
            return *this;
        }

        // Component-wise absolute value
        friend inline pVec Abs(const pVec &a)
        {
            return pVec(fabs(a.x()), fabs(a.y()), fabs(a.z()));
        }

        // Component-wise multiply
        friend inline pVec CompMult(const pVec &a, const pVec& b)
        {
            return pVec(b.x()*a.x(), b.y()*a.y(), b.z()*a.z());
        }

        friend inline pVec Cross(const pVec& a, const pVec& b)
        {
            return pVec(
                a.y()*b.z()-a.z()*b.y(),
                a.z()*b.x()-a.x()*b.z(),
                a.x()*b.y()-a.y()*b.x());
        }

        friend inline std::ostream& operator<<(std::ostream& os, const pVec& v)
        {
            os << &v << '[' << v.x() << ", " << v.y() << ", " << v.z() << ']';

            return os;
        }
    };

    // To offset [0 .. 1] vectors to [-.5 .. .5]
    static pVec vHalf(0.5, 0.5, 0.5);

    inline pVec pRandVec()
    {
        return pVec(pRandf(), pRandf(), pRandf());
    }

    inline pVec pNRandVec(float stdev)
    {
        return pVec(pNRandf(stdev), pNRandf(stdev), pNRandf(stdev));
    }

};

#endif
