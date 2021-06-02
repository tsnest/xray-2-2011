/// PInternalSourceState.h
///
/// Copyright 1997-2007 by David K. McAllister
/// http://www.ParticleSystems.org
///
/// Defines these classes: PInternalSourceState_t

#ifndef PInternalSourceState_h
#define PInternalSourceState_h

#include <ParticleAPI/pDomain.h>

namespace PAPI {

    const pVec P010(0.0f, 1.0f, 0.0f);
    const pVec P000(0.0f, 0.0f, 0.0f);
    const pVec P111(1.0f, 1.0f, 1.0f);

    class PInternalSourceState_t
    {
    public:
        pDomain *Up;
        pDomain *Vel;
        pDomain *RotVel;
        pDomain *VertexB;
        pDomain *Size;
        pDomain *Color;
        pDomain *Alpha;
        puint64 Data;
        float Age;
        float AgeSigma;
        float Mass;
        bool vertexB_tracks;

        PInternalSourceState_t() : Up(NEW(PDPoint)(P010)), Vel(NEW(PDPoint)(P000)), RotVel(NEW(PDPoint)(P000)),
            VertexB(NEW(PDPoint)(P000)), Size(NEW(PDPoint)(P111)), Color(NEW(PDPoint)(P111)), Alpha(NEW(PDPoint)(P111))
        {
            Data = 0;
            Age = 0.0f;
            AgeSigma = 0.0f;
            Mass = 1.0f;
            vertexB_tracks = true;
        }

        void WipeIt()
        {
            DELETE( Up );
            DELETE( Vel );
            DELETE( RotVel );
            DELETE( VertexB );
            DELETE( Size );
            DELETE( Color );
            DELETE( Alpha );
        }

        ~PInternalSourceState_t() { WipeIt(); }

        // Make copies of the other guy's entries and point me to the copies
        void set(const PInternalSourceState_t &In)
        {
            WipeIt();
            Up = In.Up->copy();
            Vel = In.Vel->copy();
            RotVel = In.RotVel->copy();
            VertexB = In.VertexB->copy();
            Size = In.Size->copy();
            Color = In.Color->copy();
            Alpha = In.Alpha->copy();
            Data = In.Data;
            Age = In.Age;
            AgeSigma = In.AgeSigma;
            Mass = In.Mass;
            vertexB_tracks = In.vertexB_tracks;
        }
    };

};

#endif
