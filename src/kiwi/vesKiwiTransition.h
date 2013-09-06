#ifndef __vesKiwiTransition_h
#define __vesKiwiTransition_h

#include "vesObject.h"
#include "vesKiwiEasing.h"

#include <set>

class vesKiwiBaseApp;

/// \class vesKiwiTransition
/// \ingroup KiwiPlatform
/// \brief A base class for transition animations.
///
/// <h2>Using transitions</h2>
///
/// Transitions are implemented using a declarative API
/// which is not typical of other VTK or VES classes.
/// Methods that set properties of classes return a shared
/// pointer to the class instead of void (as other classes
/// usually do). This was done in order to make constructing
/// transitions simple and concise without requiring you to
/// store the transition in a local variable.
///
/// Transition objects should be constructed by calling
/// their class-static <code>create()</code> methods.
/// These methods generally require a reference
/// or pointer to the object whose state they will be
/// updating. They return a shared pointer to the newly-
/// constructed transition. You may then
/// call set() and take() methods on the instance to
/// describe the initial and final conditions, the
/// duration, and any easing.
///
/// Methods to specify a transition generally have
/// names that start with "set" or "take".
/// Set methods take an argument to be used as a value
/// while take methods take no argument but rather indicate
/// that the object being transitioned is currently in the
/// desired state. For example, imagine a transition for
/// modifying an object's color. It might have a set methods
/// with the signatures
/// "setInitialColor(const vesColor3f&)" and
/// "setFinalColor(const vesColor3f&)" and
/// take methods with the signatures
/// "takeInitialColor()" and "takeFinalColor()".
/// Calling takeFinalColor would use the current color of
/// the object passed to the transition's <code>create()</code>
/// method as the final color in the transition.
/// Calling setFinalColor would not examine the object
/// targeted in the transition.
///
/// Avoid calling "takeInitial..." methods if possible, since
/// other transitions queued before this one might change
/// the object's state before the transition you are configuring
/// begins.
///
/// Add the transition to the application's set of animations
/// by calling vesKiwiBaseApp::addTransition().
/// Only call addTransition() with transitions
/// you wish to start immediately; you may chain other
/// transitions to start synchronously using the
/// vesKiwiTransition::alsoStart() method or
/// when this transition completes using the
/// vesKiwiTransition::followedBy() method.
///
/// Do not attempt to adjust the transition start time or
/// duration after it has started; transitions do not yet
/// support varying durations as it could interfere with
/// the smoothness of easing functions.
///
/// <h2>Writing new transitions</h2>
///
/// When subclassing vesKiwiTransition, make sure you include
/// a vesSharedFromThisMacro call in your class declaration
/// since vesKiwiTransition derives from enable_shared_this.
///
/// As stated above, your transition should provide a static
/// construction method called create() that returns a shared
/// pointer to a new instance. It should accept a pointer or
/// reference to the object or value whose state will be
/// transitioned. Its final argument should be a reference to
/// a shared pointer in which the return value is also stored.
/// This method <b>must</b> use the vesSharedPtrHelper macro
/// defined in vesSharedPtr.h to construct a shared pointer to
/// return given a pointer to an instance of your class.
///
/// The application will call begin() on your class once
/// immediately before the first call to update().
/// If begin() returns 0, then the transition (and all the
/// transitions it references) will be skipped.
/// Then, update() will be called each time a frame is
/// rendered until update() returns 0.
///
/// begin() and update() are largely wrappers around
/// willBegin() and prepareState(), respectively. These
/// methods may return either
/// vesKiwiTransition::TRANSITION_END or
/// vesKiwiTransition::TRANSITION_ERROR
/// to indicate that no more frames of the transition exist.
/// The first (TRANSITION_END) indicates a normal but early
/// completion, indicating that follow-on transitions should
/// be executed. The second (TRANSITION_ERROR) indicates that
/// a problem occurred and follow-on transitions should be
/// not be queued. Return TRANSITION_OK if there is no need
/// to prematurely end the transition.
///
/// In the willBegin() method, transitions should attempt to
/// take their initial conditions from the object to be
/// transitioned if they have not already been stipulated.
/// This means that for each state variable, you should also
/// keep two boolean indicating whether its initial and
/// final values have already been set.
/// If at least one final condition for one value of the
/// transition has not been set when willBegin() is
/// invoked, willBegin() should return TRANSITION_ERROR
/// indicating that the transition is ill-defined.
/// Assuming one final condition is set but other values
/// do not have final conditions, they should be taken
/// from the current state (effectively making them constant
/// for the duration of the transition).
///
/// Each time update() is called, vesKiwiTransition transforms the
/// time relative to the start of the animation and the duration
/// into a normalized time in the range [0,1]. Then it applies
/// the easing function and calls prepareState() with the
/// eased, normalized state number.
/// Subclasses which wish to avoid easing in order to respond
/// to time directly may override this method and call prepareState()
/// themselves, however this is not supported or advised.
///
/// Subclasses must implement the prepareState() method which
/// will be called with a parameter value of 0.0 for the first
/// frame and 1.0 for the last frame. Note that these are not
/// necessarily the only times these values may be passed; an
/// easing function with overshoot may result in state values
/// below 0 or above 1.
/// To indicate that the transition should end prematurely,
/// prepareState() should return TRANSITION_END or
/// TRANSITION_ERROR, depending on whether follow-on transitions
/// should be queued or not.
///
/// Subclasses may override the willBegin() and didEnd()
/// methods if they wish to perform one-time setup or
/// shutdown tasks before/after the transition.
/// willBegin() is called before the first invocation of
/// prepareState(), while didEnd() is called after its
/// final invocation.
/// Subclasses should not normally override begin();
/// it is used by this class to maintain timestamps.
class vesKiwiTransition :
  public vesObject, vesEnableSharedPtr(vesKiwiTransition)
{
public:
  vesTypeMacro(vesKiwiTransition);
  vesKiwiTransition();
  virtual ~vesKiwiTransition();

  /// \brief An enumeration of transition queues maintained.
  enum TransitionDisposition {
    QUEUE_AT_STARTUP = 0, //!< Simultaneous startup.
    QUEUE_AT_COMPLETION,  //!< Startup afer this transition.
    NUMBER_OF_QUEUES // Must be last!
  };

  /// Return values for prepareState.
  enum TransitionStatus {
    TRANSITION_OK = 0, //!< The state was updated without incident.
    TRANSITION_END,    //!< Stop. Follow-on transitions should be queued now.
    TRANSITION_ERROR   //!< Stop this transition and queue no more.
  };

  /// Set/get the length (in seconds) the transition should last.
  //@{
  virtual vesKiwiTransition::Ptr setDuration(double d);
  double duration()
    { return this->mDuration; }
  //@}

  /// Set/get the easing used to modulate "normalized time" before it is passed to prepareState().
  //@{
  virtual vesKiwiTransition::Ptr setEasing(vesKiwiEasing::Ptr e);
  vesKiwiEasing::Ptr easing()
    { return this->mEasing; }
  //@}

  virtual int begin(double t, vesKiwiBaseApp* app);
  virtual int update(double t, vesKiwiBaseApp* app);

  /// Called by begin() when the first render is about to occur.
  virtual TransitionStatus willBegin() { return TRANSITION_OK; }
  /// Called by update() when a render is about to occur.
  virtual TransitionStatus prepareState(double state) = 0;
  /// Called by update() or begin() when the transition ends.
  virtual void didEnd(bool wasCanceledBeforeCompletion)
    { vesNotUsed(wasCanceledBeforeCompletion); }

  /// Add the given transition to the app upon this transition's completion.
  virtual vesKiwiTransition::Ptr followedBy(vesKiwiTransition::Ptr next) {
    this->chain(next, QUEUE_AT_COMPLETION);
    return shared_from_this();
  }

  /// Add the given transition to the app as soon as this transition starts.
  virtual vesKiwiTransition::Ptr alsoStart(vesKiwiTransition::Ptr next) {
    this->chain(next, QUEUE_AT_STARTUP);
    return shared_from_this();
  }

  vesKiwiTransition::Ptr chain(
    vesKiwiTransition::Ptr next, TransitionDisposition queue);

  /// A transition set is used to hold a queue of related transitions.
  typedef std::set<vesKiwiTransition::Ptr> TransitionSet;
  /// Return the set of transitions related to this one by a particular disposition.
  TransitionSet& chained(TransitionDisposition queue)
    { return this->mChained[queue]; }

protected:
  int respondToSubclass( TransitionStatus subclassStatus, vesKiwiBaseApp* app);
  void queueChain( TransitionDisposition queue, vesKiwiBaseApp* app);

  vesKiwiEasing::Ptr mEasing;
  double mDuration;
  double mBeginTime;
  double mLastTime;
  std::set<vesKiwiTransition::Ptr> mChained[NUMBER_OF_QUEUES];
};

#endif // __vesKiwiTransition_h
