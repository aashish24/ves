#ifndef __vesKiwiSceneTransitions_h
#define __vesKiwiSceneTransitions_h

#include "vesKiwiIVarTransition.h"
#include "vesKiwiPolyDataRepresentation.h"
#include "vesActor.h"
#include "vesMapper.h"

// This file contains typedefs of template specializations
// for use in transitioning scene graph nodes or representations
// that hold scene graph nodes.

/// Specialize a transition that alters a polydata-representation's color.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector4f,
    vesKiwiPolyDataRepresentation,
    &vesKiwiPolyDataRepresentation::color,
    &vesKiwiPolyDataRepresentation::setColor
  >
> vesKiwiPolyDataColorTransition;

/// Specialize a transition that alters a polydata-representation's opacity.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    double,
    vesKiwiPolyDataRepresentation,
    &vesKiwiPolyDataRepresentation::opacity,
    &vesKiwiPolyDataRepresentation::setOpacity
  >
> vesKiwiPolyDataOpacityTransition;

/// Specialize a transition that rotates an actor.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector4f,
    vesActor,
    &vesActor::rotation,
    &vesActor::setRotation
  >
> vesActorRotationTransition;

/// Specialize a transition that translates an actor's center of rotation.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector3f,
    vesActor,
    &vesActor::center,
    &vesActor::setCenter
  >
> vesActorCenterTransition;

/// Specialize a transition that translates an actor.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector3f,
    vesActor,
    &vesActor::translation,
    &vesActor::setTranslation
  >
> vesActorTranslationTransition;

#endif // __vesKiwiSceneTransitions_h
