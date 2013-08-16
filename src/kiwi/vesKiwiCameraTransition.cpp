#include "vesKiwiCameraTransition.h"
#include "vtkQuaternion.h"

#include <iostream>

vesKiwiCameraTransition::vesKiwiCameraTransition(vesCamera::Ptr camera)
{
  if (!camera)
    {
    cerr << "Error. Camera transition needs a non-NULL camera!\n";
    }
  this->mCamera = camera;

  this->mInitialOrientationSet = false;
  this->mInitialFocusSet = false;
  this->mInitialDistanceSet = false;
  this->mInitialParallelScaleSet = false;

  this->mFinalOrientationSet = false;
  this->mFinalFocusSet = false;
  this->mFinalDistanceSet = false;
  this->mFinalParallelScaleSet = false;

  this->mFinalParallelScale = -1.;
  this->mInitialParallelScale = -1.;
}

vesKiwiCameraTransition::~vesKiwiCameraTransition()
{
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::create(vesCamera::Ptr cam)
{
  return vesSharedPtrHelper(
    new vesKiwiCameraTransition(cam));
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::create(vesCamera::Ptr cam, Ptr& ref)
{
  ref = vesKiwiCameraTransition::create(cam);
  return ref;
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setInitialOrientation(
  const vtkQuaternionf& orientation)
{
  this->mInitialOrientation = orientation;
  this->mInitialOrientationSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeInitialOrientation()
{
  return this->setInitialOrientation(this->currentOrientation());
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setFinalOrientation(
  const vtkQuaternionf& orientation)
{
  this->mFinalOrientation = orientation;
  this->mFinalOrientationSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeFinalOrientation()
{
  return this->setFinalOrientation(this->currentOrientation());
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setInitialFocus(
  const vesVector3f& focus)
{
  this->mInitialFocus = focus;
  this->mInitialFocusSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeInitialFocus()
{
  return this->setInitialFocus(this->mCamera->focalPoint());
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setFinalFocus(
  const vesVector3f& focus)
{
  this->mFinalFocus = focus;
  this->mFinalFocusSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeFinalFocus()
{
  return this->setFinalFocus(this->mCamera->focalPoint());
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setInitialDistance(
  double distance)
{
  this->mInitialDistance = distance;
  this->mInitialDistanceSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeInitialDistance()
{
  return this->setInitialDistance(
    (this->mCamera->focalPoint() - this->mCamera->position()).norm());
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setFinalDistance(
  double distance)
{
  this->mFinalDistance = distance;
  this->mFinalDistanceSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeFinalDistance()
{
  return this->setFinalDistance(
    (this->mCamera->focalPoint() - this->mCamera->position()).norm());
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setInitialParallelScale(
  double distance)
{
  this->mInitialParallelScale = distance;
  this->mInitialParallelScaleSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeInitialParallelScale()
{
  return this->setInitialParallelScale(
    this->mCamera->parallelProjection() ?
    this->mCamera->parallelScale() :
    -1.);
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setFinalParallelScale(
  double distance)
{
  this->mFinalParallelScale = distance;
  this->mFinalParallelScaleSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeFinalParallelScale()
{
  return this->setFinalParallelScale(
    this->mCamera->parallelProjection() ?
    this->mCamera->parallelScale() :
    -1.);
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setInitialFrame(
  const vesVector3f& cameraPosn,
  const vesVector3f& focalPoint,
  const vesVector3f& up)
{
  this->setInitialFocus(focalPoint);
  this->setInitialDistance((cameraPosn - focalPoint).norm());
  vesMatrix4x4f modelViewTransform = vesLookAt(cameraPosn, focalPoint, up);
  float tmp[3][3];
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      tmp[i][j] = modelViewTransform(j,i);
  this->mInitialOrientation.FromMatrix3x3(tmp);
  this->mInitialOrientationSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeInitialFrame()
{
  this->takeInitialFocus();
  this->takeInitialDistance();
  this->takeInitialOrientation();
  this->takeInitialParallelScale();
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::setFinalFrame(
  const vesVector3f& cameraPosn,
  const vesVector3f& focalPoint,
  const vesVector3f& up)
{
  this->setFinalFocus(focalPoint);
  this->setFinalDistance((cameraPosn - focalPoint).norm());
  vesMatrix4x4f modelViewTransform = vesLookAt(cameraPosn, focalPoint, up);
  float tmp[3][3];
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      tmp[i][j] = modelViewTransform(j,i);
  this->mFinalOrientation.FromMatrix3x3(tmp);
  this->mFinalOrientationSet = true;
  return shared_from_this();
}

vesKiwiCameraTransition::Ptr vesKiwiCameraTransition::takeFinalFrame()
{
  this->takeFinalFocus();
  this->takeFinalDistance();
  this->takeFinalOrientation();
  this->takeFinalParallelScale();
  return shared_from_this();
}

vesKiwiTransition::TransitionStatus vesKiwiCameraTransition::willBegin()
{
  // If we don't have *any* endpoint information set, there is no transition.
  if (
    !this->mFinalOrientationSet &&
    !this->mFinalFocusSet &&
    !this->mFinalDistanceSet &&
    !this->mFinalParallelScaleSet
    )
    {
    return vesKiwiTransition::TRANSITION_ERROR;
    }

  // Grab any unset initial state from the current state.
  if (!this->mInitialOrientationSet)
    {
    this->takeInitialOrientation();
    }
  if (!this->mInitialFocusSet)
    {
    this->takeInitialFocus();
    }
  if (!this->mInitialDistanceSet)
    {
    this->takeInitialDistance();
    }
  if (!this->mInitialParallelScaleSet)
    {
    this->takeInitialParallelScale();
    }

  // Grab any final state from the current state.
  // This assumes that any state not set should be
  // held constant.
  if (!this->mFinalOrientationSet)
    {
    this->takeFinalOrientation();
    }
  if (!this->mFinalFocusSet)
    {
    this->takeFinalFocus();
    }
  if (!this->mFinalDistanceSet)
    {
    this->takeFinalDistance();
    }
  if (!this->mFinalParallelScaleSet)
    {
    this->takeFinalParallelScale();
    }

  // Modulate the duration of the animation in proportion to
  // the angle between the initial and final view vectors.
  // The duration is unchanged when theta is 180 degrees (pi radians).
  this->mUnmodulatedDuration = this->mDuration;
  // FIXME: duration should be modulated by the arc-length of the quaternion slerp
#if 0
  vesMatrix3x3f finiFrame;
  this->mFinalOrientation.ToMatrix3x3(tmp);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      finiFrame(i,j) = tmp[i][j];
  vesVector3f vi = -camFrame.block<3,1>(0,2);
  vesVector3f vf = finiFrame.block<3,1>(0,2);
  double vdot = vi.dot(vf);
  double sinTheta = vi.cross(vf).norm();
  if (sinTheta < 1e-6)
    {
    if (vdot < -0.9)
      { // vectors are 180 degrees apart
      return;
      }
    // otherwise, vectors are nearly identical... no need to animate:
    this->mDuration = -1.;
    this->prepareState(1.0);
    return;
    }
  this->mDuration *= fabs(asin(sinTheta) / vtkMath::Pi());
#endif // 0
  return vesKiwiTransition::TRANSITION_OK;
}

vesKiwiTransition::TransitionStatus vesKiwiCameraTransition::prepareState(double state)
{
  // Compute the new camera orientation, distance, and focal point before updating the camera position.
  double curDist = this->mFinalDistance + (1. - state) * (this->mInitialDistance - this->mFinalDistance);
  float t[3][3];
  this->mInitialOrientation.Slerp(state, this->mFinalOrientation).ToMatrix3x3(t);
  vesMatrix3x3f m3;
  m3 <<
    t[0][0], t[0][1], t[0][2],
    t[1][0], t[1][1], t[1][2],
    t[2][0], t[2][1], t[2][2];
  vesVector3f dirn = m3.block<3,1>(0,2).normalized();
  vesVector3f focus = this->mFinalFocus +
      (1. - state) * (this->mInitialFocus - this->mFinalFocus);
  vesVector3f posn = focus + curDist * dirn;
  //std::cout << "Moving camera from " << this->mCamera->position() << " to " << posn << "\n";
  //std::cout << "s " << state << "\n";
  this->mCamera->setFocalPoint(focus);
  this->mCamera->setPosition(posn);
  this->mCamera->setViewUp(m3.block<3,1>(0,1).normalized());
  this->mCamera->orthogonalizeViewUp();
  if (this->mCamera->parallelProjection())
    {
    double curScale = this->mFinalParallelScale +
      (1. - state) * (this->mInitialParallelScale - this->mFinalParallelScale);
    this->mCamera->setParallelScale(curScale);
    }
  //this->mCamera->resetBounds();
  this->mCamera->setBoundsDirty(true);
  return vesKiwiTransition::TRANSITION_OK;
}

void vesKiwiCameraTransition::didEnd(bool finishedEarly)
{
  vesNotUsed(finishedEarly);
  // Reset the duration so that if we're called again
  // things don't go increasingly quickly.
  this->mDuration = this->mUnmodulatedDuration;
}

vtkQuaternionf vesKiwiCameraTransition::currentOrientation() const
{
  vesMatrix3x3f camFrame =
    makeNormalMatrix3x3f(this->mCamera->computeViewTransform());
  float tmp[3][3];
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      tmp[i][j] = camFrame(j,i);
  vtkQuaternionf orientation;
  orientation.FromMatrix3x3(tmp);
  return orientation;
}

double vesKiwiCameraTransition::currentDistance() const
{
  return (this->mCamera->position() - this->mCamera->focalPoint()).norm();
}
