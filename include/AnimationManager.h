#ifndef CLASS_ANIMATIONMANAGER
#define CLASS_ANIMATIONMANAGER

#include <nctl/UniquePtr.h>
#include "AnimationGroup.h"

class Sprite;

/// The animation manager class
class AnimationManager
{
  public:
	AnimationManager();

	inline nctl::Array<nctl::UniquePtr<IAnimation>> &anims() { return (*animGroup_).anims(); }
	inline const nctl::Array<nctl::UniquePtr<IAnimation>> &anims() const { return (*animGroup_).anims(); }

	inline IAnimation::State state() const { return animGroup_->state(); }

	inline void stop() { animGroup_->stop(); }
	inline void pause() { animGroup_->pause(); }
	inline void play() { animGroup_->play(); }

	void update(float deltaTime);
	void reset();
	void clear();

	void removeSprite(Sprite *sprite);

  private:
	nctl::UniquePtr<AnimationGroup> animGroup_;
};

#endif
