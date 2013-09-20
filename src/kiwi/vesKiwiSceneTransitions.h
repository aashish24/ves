#ifndef __vesKiwiSceneTransitions_h
#define __vesKiwiSceneTransitions_h

#include "vesKiwiIVarTransition.h"
#include "vesKiwiPolyDataRepresentation.h"
#include "vesActor.h"
#include "vesMapper.h"

/// \file vesKiwiSceneTransitions.h
/// This file contains typedefs for template specializations
/// of vesKiwiIVarTransition to be used in transitioning
/// scene graph nodes or representations that hold scene graph nodes.

/// \class vesKiwiPolyDataColorTransition
/// \ingroup KiwiPlatform
/// \brief Alters a polydata-representation's color by specializing vesKiwiIVarTransition.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector4f,
    vesKiwiPolyDataRepresentation,
    &vesKiwiPolyDataRepresentation::color,
    &vesKiwiPolyDataRepresentation::setColor
  >
> vesKiwiPolyDataColorTransition;

/// \class vesKiwiPolyDataOpacityTransition
/// \ingroup KiwiPlatform
/// \brief Alters a polydata-representation's opacity by specializing vesKiwiIVarTransition.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    double,
    vesKiwiPolyDataRepresentation,
    &vesKiwiPolyDataRepresentation::opacity,
    &vesKiwiPolyDataRepresentation::setOpacity
  >
> vesKiwiPolyDataOpacityTransition;

/// \class vesKiwiActorRotationTransition
/// \ingroup KiwiPlatform
/// \brief Specialize a transition that rotates an actor.
/// \brief Rotate an actor by specializing vesKiwiIVarTransition.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector4f,
    vesActor,
    &vesActor::rotation,
    &vesActor::setRotation
  >
> vesActorRotationTransition;

/// \class vesKiwiActorCenterTransition
/// \ingroup KiwiPlatform
/// \brief Translate an actor's center of rotation by specializing vesKiwiIVarTransition.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector3f,
    vesActor,
    &vesActor::center,
    &vesActor::setCenter
  >
> vesActorCenterTransition;

/// \class vesKiwiActorTranslationTransition
/// \ingroup KiwiPlatform
/// \brief Translate an actor by specializing vesKiwiIVarTransition.
typedef vesKiwiIVarTransition<
  vesKiwiIVarHelper<
    vesVector3f,
    vesActor,
    &vesActor::translation,
    &vesActor::setTranslation
  >
> vesActorTranslationTransition;

#endif // __vesKiwiSceneTransitions_h
