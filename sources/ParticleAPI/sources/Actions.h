/// Actions.h
///
/// Copyright 1997-2007 by David K. McAllister
/// http://www.ParticleSystems.org
///
/// These structures store the details of actions for action lists

#ifndef _Actions_h
#define _Actions_h

// pDomain.h includes pVec.h.
#include <ParticleAPI/pDomain.h>

#include "PInternalSourceState.h"
#include "ParticleGroup.h"

namespace PAPI {

// This method actually does the particle's action.
#define EXEC_METHOD void Execute(ParticleGroup &pg, ParticleList::iterator ibegin, ParticleList::iterator iend)

class PInternalState_t;

struct PActionBase
{
    virtual ~PActionBase()
    {
    }

    float dt; // This is copied to here from PInternalState_t.

    bool GetKillsParticles() { return bKillsParticles; }
    bool GetDoNotSegment() { return bDoNotSegment; }

    void SetKillsParticles(const bool v) { bKillsParticles = v; }
    void SetDoNotSegment(const bool v) { bDoNotSegment = v; }

    void SetPInternalState(PInternalState_t *P) { PS = P; }

    virtual EXEC_METHOD = 0;

private:
    // These are used for doing optimizations where we perform all actions to a working set of particles,
    // then to the next working set, etc. to improve cache coherency.
    // This doesn't work if the application of an action to a particle is a function of other particles in the group
    bool bKillsParticles; // True if this action can kill particles
    bool bDoNotSegment;   // True if this action can't be segmented

protected:
    PInternalState_t *PS;
};

///////////////////////////////////////////////////////////////////////////
// Data types derived from PActionBase.

struct PAAvoid : public PActionBase
{
    pDomain *position;	// Avoid region
    float look_ahead;	// how many time units ahead to look
    float magnitude;	// what percent of the way to go each time
    float epsilon;		// add to r^2 for softening

    EXEC_METHOD;

    ~PAAvoid() {DELETE( position );}

    void Exec(const PDTriangle &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDRectangle &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDPlane &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDSphere &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDDisc &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
};

struct PABounce : public PActionBase
{
    pDomain *position;	// Bounce region
    float oneMinusFriction;	// Friction tangent to surface
    float resilience;	// Resilence perpendicular to surface
    float cutoffSqr;	// cutoff velocity; friction applies iff v > cutoff

    EXEC_METHOD;

    ~PABounce() {DELETE( position );}

    void Exec(const PDTriangle &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDRectangle &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDPlane &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDSphere &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
    void Exec(const PDDisc &dom, ParticleGroup &group, ParticleList::iterator ibegin, ParticleList::iterator iend);
};

struct PACallback : public PActionBase
{
    P_PARTICLE_CALLBACK callback;
    puint64 Data; // The action list number to call

    EXEC_METHOD;
};

struct PACallActionList : public PActionBase
{
    int action_list_num; // The action list number to call

    EXEC_METHOD;
};

struct PACopyVertexB : public PActionBase
{
    bool copy_pos;		// True to copy pos to posB.
    bool copy_vel;		// True to copy vel to velB.

    EXEC_METHOD;
};

struct PADamping : public PActionBase
{
    pVec damping;	    // Damping constant applied to velocity
    float vlowSqr;		// Low and high cutoff velocities
    float vhighSqr;

    EXEC_METHOD;
};

struct PARotDamping : public PActionBase
{
    pVec damping;	    // Damping constant applied to velocity
    float vlowSqr;		// Low and high cutoff velocities
    float vhighSqr;

    EXEC_METHOD;
};

struct PAExplosion : public PActionBase
{
    pVec center;		// The center of the explosion
    float radius;		// Of shock wave peak
    float magnitude;	// At unit radius
    float stdev;		// Sharpness or width of shock wave
    float epsilon;		// Softening parameter

    EXEC_METHOD;
};

struct PAFollow : public PActionBase
{
    float magnitude;	// The grav of each particle
    float epsilon;		// Softening parameter
    float max_radius;	// Only influence particles within max_radius

    EXEC_METHOD;
};

struct PAGravitate : public PActionBase
{
    float magnitude;	// The grav of each particle
    float epsilon;		// Softening parameter
    float max_radius;	// Only influence particles within max_radius

    EXEC_METHOD;
};

struct PAGravity : public PActionBase
{
    pVec direction;	    // Amount to increment velocity

    EXEC_METHOD;
};

struct PAJet : public PActionBase
{
    pDomain *dom;		// Accelerate particles that are within this domain
    pDomain *acc;		// Acceleration vector domain

    EXEC_METHOD;

    ~PAJet() {DELETE( dom ); DELETE( acc );}
};

struct PAKillOld : public PActionBase
{
    float age_limit;		// Exact age at which to kill particles.
    bool kill_less_than;	// True to kill particles less than limit.

    EXEC_METHOD;
};

struct PAMatchVelocity : public PActionBase
{
    float magnitude;	// The grav of each particle
    float epsilon;		// Softening parameter
    float max_radius;	// Only influence particles within max_radius

    EXEC_METHOD;
};

struct PAMatchRotVelocity : public PActionBase
{
    float magnitude;	// The grav of each particle
    float epsilon;		// Softening parameter
    float max_radius;	// Only influence particles within max_radius

    EXEC_METHOD;
};

struct PAMove : public PActionBase
{
    bool move_velocity;
    bool move_rotational_velocity;

    EXEC_METHOD;
};

struct PAOrbitLine : public PActionBase
{
    pVec p, axis;	    // Endpoints of line to which particles are attracted
    float magnitude;	// Scales acceleration
    float epsilon;		// Softening parameter
    float max_radius;	// Only influence particles within max_radius

    EXEC_METHOD;
};

struct PAOrbitPoint : public PActionBase
{
    pVec center;		// Point to which particles are attracted
    float magnitude;	// Scales acceleration
    float epsilon;		// Softening parameter
    float max_radius;	// Only influence particles within max_radius

    EXEC_METHOD;
};

struct PARandomAccel : public PActionBase
{
    pDomain *gen_acc;	// The domain of random accelerations.

    EXEC_METHOD;

    ~PARandomAccel() {DELETE( gen_acc );}
};

struct PARandomDisplace : public PActionBase
{
    pDomain *gen_disp;	// The domain of random displacements.

    EXEC_METHOD;

    ~PARandomDisplace() {DELETE( gen_disp );}
};

struct PARandomVelocity : public PActionBase
{
    pDomain *gen_vel;	// The domain of random velocities.

    EXEC_METHOD;

    ~PARandomVelocity() {DELETE( gen_vel );}
};

struct PARandomRotVelocity : public PActionBase
{
    pDomain *gen_vel;	// The domain of random velocities.

    EXEC_METHOD;

    ~PARandomRotVelocity() {DELETE( gen_vel );}
};

struct PARestore : public PActionBase
{
    float time_left;	// Time remaining until they should be in position.
    bool restore_velocity;
    bool restore_rvelocity;

    EXEC_METHOD;
};

struct PASink : public PActionBase
{
    bool kill_inside;	// True to dispose of particles *inside* domain
    pDomain *position;	// Disposal region

    EXEC_METHOD;

    ~PASink() {
        DELETE( position );
    }
};

struct PASinkVelocity : public PActionBase
{
    bool kill_inside;	// True to dispose of particles with vel *inside* domain
    pDomain *velocity;	// Disposal region

    EXEC_METHOD;

    ~PASinkVelocity() {DELETE( velocity );}
};

struct PASort : public PActionBase
{
    pVec Eye;		// A point on the line to project onto
    pVec Look;		// The direction for which to sort particles
    bool front_to_back; // True to sort front_to_back
    bool clamp_negative; // True to clamp negative dot products to zero

    EXEC_METHOD;
};

struct PASource : public PActionBase
{
    pDomain *position;	           // Choose a position in this domain
    float particle_rate;	       // Particles to generate per unit time
    PInternalSourceState_t SrcSt;  // The state needed to create a new particle

    EXEC_METHOD;

    ~PASource()
    {
        DELETE( position );	// Choose a position in this domain.
    }
};

struct PASpeedLimit : public PActionBase
{
    float min_speed;		// Clamp speed to this minimum.
    float max_speed;		// Clamp speed to this maximum.

    EXEC_METHOD;
};

struct PATargetColor : public PActionBase
{
    pVec color;		    // Color to shift towards
    float alpha;		// Alpha value to shift towards
    float scale;		// Amount to shift by (1 == all the way)

    EXEC_METHOD;
};

struct PATargetSize : public PActionBase
{
    pVec size;		// Size to shift towards
    pVec scale;		// Amount to shift by per frame (1 == all the way)

    EXEC_METHOD;
};

struct PATargetVelocity : public PActionBase
{
    pVec velocity;	    // Velocity to shift towards
    float scale;		// Amount to shift by (1 == all the way)

    EXEC_METHOD;
};

struct PATargetRotVelocity : public PActionBase
{
    pVec velocity;	    // Velocity to shift towards
    float scale;		// Amount to shift by (1 == all the way)

    EXEC_METHOD;
};

struct PAVortex : public PActionBase
{
    pVec tip;		         // Tip of vortex
    pVec axis;		         // Axis around which vortex is applied
    float tightnessExponent; // Raise radius to this power to create vortex-like silhouette
    float max_radius;	     // Only influence particles within max_radius of axis
    float inSpeed;	         // inward acceleration of particles outside the vortex
    float upSpeed;           // vertical acceleration of particles inside the vortex
    float aroundSpeed;       // acceleration around vortex of particles inside the vortex

    EXEC_METHOD;
};

};

#endif
