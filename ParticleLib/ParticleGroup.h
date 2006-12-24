// ParticleGroup.h
//
// Copyright 1998-2005 by David K. McAllister.
//
// A group of particles - Info and an array of Particles
//
// Defines these classes: ParticleGroup

#ifndef _ParticleGroup_h
#define _ParticleGroup_h

// Particle.h includes pVec.h.
#include "Particle.h"

#include <vector>

namespace PAPI {

typedef void (*P_PARTICLE_CALLBACK)(struct Particle_t &particle, void *data); // Also defined in pAPI.h.

typedef std::vector<Particle_t> ParticleList;

class ParticleGroup
{
    ParticleList list;

    size_t max_particles;	// Max particles allowed in group
    P_PARTICLE_CALLBACK cb_birth;
    P_PARTICLE_CALLBACK cb_death;
    void *cb_birth_data;
    void *cb_death_data;

public:
    ParticleGroup()
    {
        max_particles = 0;
        cb_birth = NULL;
        cb_death = NULL;
        cb_birth_data = NULL;
        cb_death_data = NULL;
    }

    ParticleGroup(size_t maxp) : max_particles(maxp)
    {
        list.reserve(max_particles);
        cb_birth = NULL;
        cb_death = NULL;
        cb_birth_data = NULL;
        cb_death_data = NULL;
    }

    ParticleGroup(const ParticleGroup &rhs) : list(rhs.list)
    {
        max_particles = rhs.max_particles;
        cb_birth = rhs.cb_birth;
        cb_death = rhs.cb_death;
        cb_birth_data = rhs.cb_birth_data;
        cb_death_data = rhs.cb_death_data;
    }

    ~ParticleGroup()
    {
        if (cb_death) {
            ParticleList::iterator it;
            for (it = list.begin(); it != list.end(); ++it)
                (*cb_death)((*it), cb_death_data);
        }
    }

    ParticleGroup &operator=(const ParticleGroup &rhs)
    {
        if (this != &rhs) {
            if (cb_death) {
                ParticleList::iterator it;
                for (it = list.begin(); it != list.end(); ++it)
                    (*cb_death)((*it), cb_death_data);
            }
            list = rhs.list;
            cb_birth = rhs.cb_birth;
            cb_death = rhs.cb_death;
            cb_birth_data = rhs.cb_birth_data;
            cb_death_data = rhs.cb_death_data;
            max_particles = rhs.max_particles;
        }
        return *this;
    }

    inline size_t GetMaxParticles() { return max_particles; }
    inline ParticleList &GetList() { return list; }

    inline void SetBirthCallback(P_PARTICLE_CALLBACK cbb, void *cbb_data)
    {
        cb_birth = cbb;
        cb_birth_data = cbb_data;
    }

    inline void SetDeathCallback(P_PARTICLE_CALLBACK cbd, void *cbd_data)
    {
        cb_death = cbd;
        cb_death_data = cbd_data;
    }

    inline void SetMaxParticles(size_t maxp)
    {
        max_particles = maxp;
        if(list.size() > max_particles) {
            if (cb_death) {
                for (ParticleList::iterator it = list.begin() + max_particles; it != list.end(); ++it)
                    (*cb_death)((*it), cb_death_data);
            }
            list.resize(max_particles);
        }
        list.reserve(max_particles);
    }

    inline size_t size() const { return list.size(); }
    inline ParticleList::iterator begin() { return list.begin(); }
    inline ParticleList::iterator end() { return list.end(); }

    // Returns a valid iterator. If we deleted the end, it points to one past last.
    inline ParticleList::iterator Remove(ParticleList::iterator it)
    {
        if (cb_death)
            (*cb_death)((*it), cb_death_data);

        // Copy the one from the end to here.
        if(it != list.end() - 1) {
            *it = *(list.end() - 1);
            list.pop_back(); // Delete the one at the end
        } else {
            list.pop_back(); // Delete the one at the end
            it = list.end();
        }

        return it;
    }

    inline bool Add(const pVec &pos, const pVec &posB,
        const pVec &up, 
        const pVec &vel, const pVec &rvel,
        const pVec &size, const pVec &color,
        const float alpha = 1.0f,
        const float age = 0.0f,
        const float mass = 1.0f,
        const long data = 0)
    {
        if (list.size() >= max_particles)
            return false;
        else {
            list.push_back(Particle_t(pos, posB, up, up, vel, vel, rvel, rvel, size, color, alpha, age, mass, data, 0.0f));
            Particle_t &p = list.back();
            if (cb_birth)
                (*cb_birth)(p, cb_birth_data);
            return true;
        }
    }

    inline bool Add(const Particle_t &P)
    {
        if (list.size() >= max_particles)
            return false;
        else {
            list.push_back(P);
            Particle_t &p = list.back();
            if (cb_birth)
                (*cb_birth)(p, cb_birth_data);
            return true;
        }
    }
};

};

#endif
