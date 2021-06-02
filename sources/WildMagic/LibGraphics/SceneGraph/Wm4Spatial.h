// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.2 (2007/09/24)

#ifndef WM4SPATIAL_H
#define WM4SPATIAL_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4BoundingVolume.h"
#include "Wm4Effect.h"
#include "Wm4GlobalState.h"
#include "Wm4Transformation.h"

namespace Wm4
{

class Culler;
class Light;

class WM4_GRAPHICS_ITEM Spatial : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~Spatial ();

    // Local and world transforms.  In some situations you might need to set
    // the world transform directly and bypass the Spatial::Update()
    // mechanism.  If World is set directly, the WorldIsCurrent flag should
    // be set to true.  For example, inverse kinematic controllers and skin
    // controllers need this capability.
    Transformation Local;
    Transformation World;
    bool WorldIsCurrent;

    // World bound access.  In some situations you might want to set the
    // world bound directly and bypass the Spatial::UpdateGS() mechanism.  If
    // WorldBound is set directly, the WorldBoundIsCurrent flag should be set
    // to true.
    BoundingVolumePtr WorldBound;
    bool WorldBoundIsCurrent;

    // Culling parameters.
    enum CullingMode
    {
        // Determine visibility state by comparing the world bounding volume
        // to culling planes.
        CULL_DYNAMIC,

        // Force the object to be culled.  If a Node is culled, its entire
        // subtree is culled.
        CULL_ALWAYS,

        // Never cull the object.  If a Node is never culled, its entire
        // subtree is never culled.  To accomplish this, the first time such
        // a Node is encountered, the bNoCull parameter is set to 'true' in
        // the recursive chain GetVisibleSet/OnGetVisibleSet.
        CULL_NEVER,

        MAX_CULLING_MODE
    };

    CullingMode Culling;

    // Update of geometric state and controllers.  The UpdateGS function
    // computes world transformations on the downward pass and world bounding
    // volumes on the upward pass.  The UpdateBS function just computes the
    // world bounding volumes on an upward pass.  This is useful if model
    // data changes, causing the model and world bounds to change, but no
    // transformations need recomputing.
    void UpdateGS (double dAppTime = -Mathd::MAX_REAL,
        bool bInitiator = true);
    void UpdateBS ();

    // global state
    int GetGlobalStateQuantity () const;
    GlobalState* GetGlobalState (int i) const;
    GlobalState* GetGlobalState (GlobalState::StateType eType) const;
    void AttachGlobalState (GlobalState* pkState);
    void DetachGlobalState (GlobalState::StateType eType);
    void DetachAllGlobalStates ();

    // light state
    int GetLightQuantity () const;
    Light* GetLight (int i) const;
    void AttachLight (Light* pkLight);
    void DetachLight (Light* pkLight);
    void DetachAllLights ();

    // effect state
    int GetEffectQuantity () const;
    Effect* GetEffect (int i) const;
    void AttachEffect (Effect* pkEffect);
    void DetachEffect (Effect* pkEffect);
    void DetachAllEffects ();
    void SetStartEffect (int i);
    int GetStartEffect () const;

    // update of render state
    virtual void UpdateRS (std::vector<GlobalState*>* akGStack = 0,
        std::vector<Light*>* pkLStack = 0);

    // parent access
    Spatial* GetParent ();

protected:
    Spatial ();

    // geometric updates
    virtual void UpdateWorldData (double dAppTime);
    virtual void UpdateWorldBound () = 0;
    void PropagateBoundToRoot ();

    // render state updates
    void PropagateStateFromRoot (std::vector<GlobalState*>* akGStack,
        std::vector<Light*>* pkLStack);
    void PushState (std::vector<GlobalState*>* akGStack,
        std::vector<Light*>* pkLStack);
    void PopState (std::vector<GlobalState*>* akGStack,
        std::vector<Light*>* pkLStack);
    virtual void UpdateState (std::vector<GlobalState*>* akGStack,
        std::vector<Light*>* pkLStack) = 0;

    // support for hierarchical scene graph
    Spatial* m_pkParent;

    // global render state
    std::vector<GlobalStatePtr> m_kGlobalStates;

    // light state
    std::vector<ObjectPtr> m_kLights;

    // Effect state.  If the effect is attached to a Geometry object, it
    // applies to that object alone.  If the effect is attached to a Node
    // object, it applies to all Geometry objects in the subtree rooted at
    // the Node.  The "mutable" tag allows this array to change temporarily
    // in Geometry::Save in order to prevent Spatial from saving the
    // LightingEffect m_spkLEffect to disk.
    mutable std::vector<EffectPtr> m_kEffects;

    // Normally, all effects are applied to an object.  To allow overriding
    // some of the effects, a starting index may be specified by the
    // application.  This is useful for complex effects, where the current
    // effects must be ignored and another effect is used instead.  Without
    // this mechanism, you would have to detach and save the current effects,
    // attach the desired effect, draw, detach the desired effect, and
    // reattach the old effects.  With this mechanism, you attach the desired
    // effect, set the starting index to that of the desired effect, draw,
    // reset the starting index to zero, and detach the desired effect.
    int m_iStartEffect;

private:
    static const char* ms_aacCullingMode[MAX_CULLING_MODE];

// internal use
public:
    // parent access (Node calls this during attach/detach of children)
    void SetParent (Spatial* pkParent);

    // culling
    void OnGetVisibleSet (Culler& rkCuller, bool bNoCull);
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull) = 0;
};

WM4_REGISTER_STREAM(Spatial);
typedef Pointer<Spatial> SpatialPtr;
#include "Wm4Spatial.inl"

}

#endif
