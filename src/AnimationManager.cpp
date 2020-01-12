#include "AnimationManager.h"
#include "ParallelAnimationGroup.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AnimationManager::AnimationManager()
    : animGroup_(nctl::makeUnique<ParallelAnimationGroup>())
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AnimationManager::update(float deltaTime)
{
	animGroup_->update(deltaTime);
}

void AnimationManager::reset()
{
	animGroup_->reset();
}

void AnimationManager::clear()
{
	animGroup_->anims().clear();
}
