#ifndef __vesKiwiCameraTransition_h
#define __vesKiwiCameraTransition_h

#include "vesKiwiTransition.h"
#include "vtkQuaternion.h"
#include "vesCamera.h"

/// \class vesKiwiCameraTransition
/// \ingroup KiwiPlatform
/// \brief A transition that refocuses and reorients the camera.
///
/// By default, both the initial and final camera state
/// (orientation, focus, distance from the focus to the camera, and
/// -- when parallel projection is turned on -- the parallel scale)
/// are set to the current camera state, making the transition
/// a no-operation. You need only set camera parameters you wish
/// to change.
///
/// The transition will not switch between perspective and
/// parallel projection.
class vesKiwiCameraTransition : public vesKiwiTransition
{
public:
  vesTypeMacro(vesKiwiCameraTransition);
  vesSharedFromThisMacro(vesKiwiTransition);
  static vesKiwiCameraTransition::Ptr create(vesCamera::Ptr cam);
  static vesKiwiCameraTransition::Ptr create(vesCamera::Ptr cam, Ptr& ref);
  virtual ~vesKiwiCameraTransition();

  vesCamera::Ptr camera() const
    { return this->mCamera; }

  vtkQuaternionf initialOrientation() const
    { return this->mInitialOrientation; }
  virtual Ptr setInitialOrientation(const vtkQuaternionf&);
  Ptr takeInitialOrientation();

  vtkQuaternionf finalOrientation() const
    { return this->mFinalOrientation; }
  virtual Ptr setFinalOrientation(const vtkQuaternionf& orientation);
  Ptr takeFinalOrientation();

  vesVector3f initialFocus() const
    { return this->mInitialFocus; }
  virtual Ptr setInitialFocus(const vesVector3f&);
  Ptr takeInitialFocus();

  vesVector3f finalFocus() const
    { return this->mFinalFocus; }
  virtual Ptr setFinalFocus(const vesVector3f& focus);
  Ptr takeFinalFocus();

  double initialDistance() const
    { return this->mInitialDistance; }
  virtual Ptr setInitialDistance(double);
  Ptr takeInitialDistance();

  double finalDistance() const
    { return this->mFinalDistance; }
  virtual Ptr setFinalDistance(double distance);
  Ptr takeFinalDistance();

  double initialParallelScale() const
    { return this->mInitialParallelScale; }
  virtual Ptr setInitialParallelScale(double);
  Ptr takeInitialParallelScale();

  double finalParallelScale() const
    { return this->mFinalParallelScale; }
  virtual Ptr setFinalParallelScale(double distance);
  Ptr takeFinalParallelScale();

  virtual Ptr setInitialFrame(
    const vesVector3f& cameraPosn,
    const vesVector3f& focalPoint,
    const vesVector3f& up);
  Ptr takeInitialFrame();

  virtual Ptr setFinalFrame(
    const vesVector3f& cameraPosn,
    const vesVector3f& focalPoint,
    const vesVector3f& up);
  Ptr takeFinalFrame();


  virtual TransitionStatus willBegin();
  virtual TransitionStatus prepareState(double state);
  virtual void didEnd(bool finishedEarly);

protected:
  vesKiwiCameraTransition(vesCamera::Ptr);

  // Return the camera's current orientation as a quaternion.
  vtkQuaternionf currentOrientation() const;
  double currentDistance() const;

  // The object being manipulated by the transition
  vesCamera::Ptr mCamera;

  // Initial and final properties of the object.
  vtkQuaternionf mInitialOrientation;
  vtkQuaternionf mFinalOrientation;
  vesVector3f mInitialFocus;
  vesVector3f mFinalFocus;
  double mInitialDistance;
  double mFinalDistance;
  double mInitialParallelScale;
  double mFinalParallelScale;

  // Which properties have been defined.
  bool mInitialOrientationSet;
  bool mFinalOrientationSet;
  bool mInitialFocusSet;
  bool mFinalFocusSet;
  bool mInitialDistanceSet;
  bool mFinalDistanceSet;
  bool mInitialParallelScaleSet;
  bool mFinalParallelScaleSet;

  // Temporary storage
  double mUnmodulatedDuration;
};

#endif // __vesKiwiCameraTransition_h
