#include "vesKiwiTransition.h"

#include "vesKiwiBaseApp.h"

#include <iostream>

vesKiwiTransition::vesKiwiTransition()
{
  this->mDuration = 0.;
  this->mBeginTime = -1.;
  this->mLastTime = -1.;
  this->mEasing = vesKiwiEasing::Ptr(new vesKiwiQuadraticInOutEasing);
}

vesKiwiTransition::~vesKiwiTransition()
{
  // If we are being destroyed before the transition
  // was complete, give the subclass a chance to clean up.
  if (this->mBeginTime >= 0.)
    {
    this->didEnd(true);
    }
}

vesKiwiTransition::Ptr vesKiwiTransition::setEasing(vesKiwiEasing::Ptr easingFn)
{
  if (!easingFn || easingFn == this->mEasing)
    {
    return shared_from_this();
    }

  if (this->mBeginTime >= 0.)
    {
    std::cerr
      << "Error. You should never change the easing\n"
      << "function after a transition has begun.\n";
    }

  this->mEasing = easingFn;
  return shared_from_this();
}

vesKiwiTransition::Ptr vesKiwiTransition::setDuration(double d)
{
  if (this->mBeginTime >= 0.)
    {
    std::cerr
      << "Error. You should never change the duration\n"
      << "after a transition has begun.\n";
    }

  this->mDuration = d;
  return shared_from_this();
}

/// \brief Called by the application as the transition starts.
int vesKiwiTransition::begin(double t, vesKiwiBaseApp* app)
{
  // First, queue other transitions that should start with us.
  this->queueChain(QUEUE_AT_STARTUP, app);

  // Now record the timestamp and give the subclass a chance to prepare.
  this->mBeginTime = t;
  this->mLastTime = -1.;
  TransitionStatus status = this->willBegin();
  // NB: Do not move the mDuration check above willBegin()
  //     so that subclasses can modulate the duration without
  //     causing us trouble.
  if (this->mDuration <= 0.)
    {
    status = TRANSITION_ERROR;
    }
  return this->respondToSubclass(status, app);
}

/// \brief Called by the application before each rendering pass through the scene graph.
int vesKiwiTransition::update(double t, vesKiwiBaseApp* app)
{
  double curTime = (t - this->mBeginTime) / this->mDuration;
  bool atEnd = false;
  if (curTime >= 1.)
    {
    curTime = 1.0;
    atEnd = true;
    }
  double state = this->mEasing->evaluate(curTime);
  // The subclass can tell us to finish early but it can't
  // prolong the transition:
  TransitionStatus status = this->prepareState(state);
  if (atEnd && status == TRANSITION_OK)
    {
    status = TRANSITION_END;
    }
  return this->respondToSubclass(status, app);
}

/// \brief Add a transition to a queue for later addition to the app.
vesKiwiTransition::Ptr vesKiwiTransition::chain(
  vesKiwiTransition::Ptr next, TransitionDisposition queue)
{
  if (queue < 0 || queue > NUMBER_OF_QUEUES)
    {
    std::cerr << "Unknown transition queue " << queue << ". Ignoring.\n";
    }
  else
    {
    this->mChained[queue].insert(next);
    }
  return next;
}

/// \brief Update the application's transition queue based on virtual method return values.
int vesKiwiTransition::respondToSubclass(TransitionStatus status, vesKiwiBaseApp* app)
{
  switch (status)
    {
  case TRANSITION_OK:
  default:
    break;
  case TRANSITION_END:
    // Queue transitions that should start when this one ends.
    this->queueChain(QUEUE_AT_COMPLETION, app);
    // Now go away.
    this->didEnd(false);
    this->mBeginTime = -1.;
    return 0;
    break;
  case TRANSITION_ERROR:
    // Do not queue other transitions. Just go away.
    this->didEnd(true);
    this->mBeginTime = -1.;
    return 0;
    }
  return 1;
}

/// \brief Add transitions to the application's queue from a chain held by the transition.
void vesKiwiTransition::queueChain(
  TransitionDisposition queue, vesKiwiBaseApp* app)
{
  TransitionSet& followers(this->chained(queue));
  for (TransitionSet::iterator it = followers.begin(); it != followers.end(); ++it)
    {
    app->addTransition(*it);
    }
  followers.clear();
}
