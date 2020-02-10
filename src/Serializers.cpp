#include "Canvas.h"
#include "Texture.h"
#include "Sprite.h"
#include "ParallelAnimationGroup.h"
#include "SequentialAnimationGroup.h"
#include "PropertyAnimation.h"
#include "GridAnimation.h"
#include "GridFunction.h"
#include "AnimationManager.h"
#include "RenderGuiStatus.h"

#include "Serializers.h"
#include "LuaSerializer.h"
#include "SerializerContext.h"

namespace Serializers {

void serialize(LuaSerializer &ls, const char *name, const Canvas &canvas)
{
	ls.buffer().formatAppend("%s =\n", name);
	ls.buffer().append("{\n");
	ls.indent();

	serialize(ls, "size", canvas.size());
	serialize(ls, "background_color", canvas.backgroundColor);

	ls.unindent();
	ls.buffer().append("}\n");
}

void serialize(LuaSerializer &ls, const Texture &texture)
{
	ls.buffer().append("{\n");
	ls.indent();

	serialize(ls, "name", texture.name());

	ls.unindent();
	ls.buffer().append("},\n");
}

void serialize(LuaSerializer &ls, const char *name, Sprite::BlendingPreset blendingPreset)
{
	switch (blendingPreset)
	{
		case Sprite::BlendingPreset::DISABLED:
			serialize(ls, name, "disabled");
			break;
		case Sprite::BlendingPreset::ALPHA:
			serialize(ls, name, "alpha");
			break;
		case Sprite::BlendingPreset::PREMULTIPLIED_ALPHA:
			serialize(ls, name, "premultiplied_alpha");
			break;
		case Sprite::BlendingPreset::ADDITIVE:
			serialize(ls, name, "additive");
			break;
		case Sprite::BlendingPreset::MULTIPLY:
			serialize(ls, name, "multiply");
			break;
	}
}

void serialize(LuaSerializer &ls, const Sprite &sprite)
{
	SerializerContext *context = static_cast<SerializerContext *>(ls.context());

	ls.buffer().append("{\n");
	ls.indent();

	serializePtr(ls, "texture", &sprite.texture(), *context->textureHash);
	serializePtr(ls, "parent", sprite.parent(), *context->spriteHash);
	serialize(ls, "name", sprite.name);
	serialize(ls, "visible", sprite.visible);
	nc::Vector2f position(sprite.x, sprite.y);
	serialize(ls, "position", position);
	serialize(ls, "rotation", sprite.rotation);
	serialize(ls, "scale_factor", sprite.scaleFactor);
	serialize(ls, "anchor_point", sprite.anchorPoint);
	serialize(ls, "color", sprite.color);
	serialize(ls, "grid_anchor_point", sprite.gridAnchorPoint);
	serialize(ls, "texrect", sprite.texRect());
	serialize(ls, "flip_x", sprite.isFlippedX());
	serialize(ls, "flip_y", sprite.isFlippedY());
	serialize(ls, "blending", sprite.blendingPreset());

	ls.unindent();
	ls.buffer().append("},\n");
}

void serialize(LuaSerializer &ls, const IAnimation *anim)
{
	SerializerContext *context = static_cast<SerializerContext *>(ls.context());

	ls.buffer().append("{\n");
	ls.indent();

	serialize(ls, "name", anim->name.data());
	serializePtr(ls, "parent", static_cast<const IAnimation *>(anim->parent()), *context->animationHash);

	switch (anim->type())
	{
		case IAnimation::Type::PARALLEL_GROUP:
		{
			serialize(ls, "type", "parallel_group");
			break;
		}
		case IAnimation::Type::SEQUENTIAL_GROUP:
		{
			serialize(ls, "type", "sequential_group");
			break;
		}
		case IAnimation::Type::PROPERTY:
		{
			const PropertyAnimation &propertyAnim = static_cast<const PropertyAnimation &>(*anim);
			serialize(ls, "type", "property_animation");
			serialize(ls, propertyAnim);
			break;
		}
		case IAnimation::Type::GRID:
		{
			const GridAnimation &gridAnimation = static_cast<const GridAnimation &>(*anim);
			serialize(ls, "type", "grid_animation");
			serialize(ls, gridAnimation);
			break;
		}
	}

	ls.unindent();
	ls.buffer().append("},\n");
}

void serialize(LuaSerializer &ls, const char *name, EasingCurve::Type type)
{
	switch (type)
	{
		case EasingCurve::Type::LINEAR:
			serialize(ls, name, "linear");
			break;
		case EasingCurve::Type::QUAD:
			serialize(ls, name, "quad");
			break;
		case EasingCurve::Type::CUBIC:
			serialize(ls, name, "cubic");
			break;
		case EasingCurve::Type::QUART:
			serialize(ls, name, "quart");
			break;
		case EasingCurve::Type::QUINT:
			serialize(ls, name, "quint");
			break;
		case EasingCurve::Type::SINE:
			serialize(ls, name, "sine");
			break;
		case EasingCurve::Type::EXPO:
			serialize(ls, name, "expo");
			break;
		case EasingCurve::Type::CIRC:
			serialize(ls, name, "circ");
			break;
	}
}

void serialize(LuaSerializer &ls, const char *name, EasingCurve::Direction direction)
{
	switch (direction)
	{
		case EasingCurve::Direction::FORWARD:
			serialize(ls, name, "forward");
			break;
		case EasingCurve::Direction::BACKWARD:
			serialize(ls, name, "backward");
			break;
	}
}

void serialize(LuaSerializer &ls, const char *name, EasingCurve::LoopMode loopMode)
{
	switch (loopMode)
	{
		case EasingCurve::LoopMode::DISABLED:
			serialize(ls, name, "disabled");
			break;
		case EasingCurve::LoopMode::REWIND:
			serialize(ls, name, "rewind");
			break;
		case EasingCurve::LoopMode::PING_PONG:
			serialize(ls, name, "ping_pong");
			break;
	}
}

void serialize(LuaSerializer &ls, const char *name, const EasingCurve &curve)
{
	ls.buffer().formatAppend("%s =\n", name);
	ls.buffer().append("{\n");
	ls.indent();

	serialize(ls, "type", curve.type());
	serialize(ls, "direction", curve.direction());
	serialize(ls, "loop_mode", curve.loopMode());
	serialize(ls, "start_time", curve.start());
	serialize(ls, "end_time", curve.end());
	serialize(ls, "scale", curve.scale());
	serialize(ls, "shift", curve.shift());

	ls.unindent();
	ls.buffer().append("},\n");
}

void serialize(LuaSerializer &ls, const PropertyAnimation &anim)
{
	SerializerContext *context = static_cast<SerializerContext *>(ls.context());

	serializePtr(ls, "sprite", anim.sprite(), *context->spriteHash);
	serialize(ls, "speed", anim.speed());
	serialize(ls, "curve", anim.curve());
	serialize(ls, "property_name", anim.propertyName().data());
}

void serialize(LuaSerializer &ls, const GridAnimation &anim)
{
	SerializerContext *context = static_cast<SerializerContext *>(ls.context());

	serializePtr(ls, "sprite", anim.sprite(), *context->spriteHash);
	serialize(ls, "speed", anim.speed());
	serialize(ls, "curve", anim.curve());

	if (anim.function() != nullptr)
	{
		serialize(ls, "function_name", anim.function()->name().data());

		ls.buffer().append("parameters =\n");
		ls.buffer().append("{\n");
		ls.indent();

		for (unsigned int i = 0; i < anim.function()->numParameters(); i++)
		{
			ls.buffer().append("{\n");
			ls.indent();

			const char *paramName = anim.function()->parameterName(i);
			serialize(ls, "name", paramName);
			if (anim.function()->parameterType(i) == GridFunction::ParameterType::FLOAT)
				serialize(ls, "value", anim.parameters()[i].value0);
			else if (anim.function()->parameterType(i) == GridFunction::ParameterType::VECTOR2F)
			{
				nc::Vector2f vecParam(anim.parameters()[i].value0, anim.parameters()[i].value1);
				serialize(ls, "value", vecParam);
			}

			ls.unindent();
			ls.buffer().append("},\n");
		}

		ls.unindent();
		ls.buffer().append("},\n");
	}
}

void serialize(LuaSerializer &ls, const char *name, const SaveAnim &saveAnim)
{
	ls.buffer().formatAppend("%s =\n", name);
	ls.buffer().append("{\n");
	ls.indent();

	serialize(ls, "num_frames", saveAnim.numFrames);
	serialize(ls, "fps", saveAnim.fps);
	serialize(ls, "canvas_resize", saveAnim.canvasResize);

	ls.unindent();
	ls.buffer().append("}\n");
}

}

namespace Deserializers {

template <>
Sprite::BlendingPreset deserialize(LuaSerializer &ls, const char *name)
{
	lua_State *L = ls.luaState();
	nctl::String blendingString = nc::LuaUtils::retrieveField<const char *>(L, -1, name);

	if (blendingString == "disabled")
		return Sprite::BlendingPreset::DISABLED;
	else if (blendingString == "alpha")
		return Sprite::BlendingPreset::ALPHA;
	else if (blendingString == "premultiplied_alpha")
		return Sprite::BlendingPreset::PREMULTIPLIED_ALPHA;
	else if (blendingString == "additive")
		return Sprite::BlendingPreset::ADDITIVE;
	else if (blendingString == "multiply")
		return Sprite::BlendingPreset::MULTIPLY;
	else
		return Sprite::BlendingPreset::DISABLED;
}

void deserialize(LuaSerializer &ls, const char *name, Canvas &canvas)
{
	lua_State *L = ls.luaState();
	nc::LuaUtils::retrieveGlobalTable(L, name);

	canvas.backgroundColor = deserialize<nc::Colorf>(ls, "background_color");
	canvas.resizeTexture(deserialize<nc::Vector2i>(ls, "size"));

	nc::LuaUtils::pop(L);
}

void deserialize(LuaSerializer &ls, nctl::UniquePtr<Texture> &texture)
{
	const char *textureName = deserialize<const char *>(ls, "name");
	texture = nctl::makeUnique<Texture>(textureName);
}

void deserialize(LuaSerializer &ls, nctl::UniquePtr<Sprite> &sprite)
{
	DeserializerContext *context = static_cast<DeserializerContext *>(ls.context());

	Texture *texture = deserializePtr(ls, "texture", *context->textures);
	FATAL_ASSERT(texture);
	sprite = nctl::makeUnique<Sprite>(texture);

	Sprite *parent = deserializePtr(ls, "parent", *context->sprites);
	sprite->setParent(parent);

	deserialize(ls, "name", sprite->name);
	deserialize(ls, "visible", sprite->visible);
	nc::Vector2f position = deserialize<nc::Vector2f>(ls, "position");
	sprite->x = position.x;
	sprite->y = position.y;
	deserialize(ls, "rotation", sprite->rotation);
	deserialize(ls, "scale_factor", sprite->scaleFactor);
	deserialize(ls, "anchor_point", sprite->anchorPoint);
	deserialize(ls, "color", sprite->color);
	deserialize(ls, "grid_anchor_point", sprite->gridAnchorPoint);
	sprite->setTexRect(deserialize<nc::Recti>(ls, "texrect"));
	sprite->setFlippedX(deserialize<bool>(ls, "flip_x"));
	sprite->setFlippedY(deserialize<bool>(ls, "flip_y"));
	sprite->setBlendingPreset(deserialize<Sprite::BlendingPreset>(ls, "blending"));
}

template <>
IAnimation::Type deserialize(LuaSerializer &ls, const char *name)
{
	lua_State *L = ls.luaState();
	nctl::String animTypeString = nc::LuaUtils::retrieveField<const char *>(L, -1, name);

	if (animTypeString == "parallel_group")
		return IAnimation::Type::PARALLEL_GROUP;
	else if (animTypeString == "sequential_group")
		return IAnimation::Type::SEQUENTIAL_GROUP;
	else if (animTypeString == "property_animation")
		return IAnimation::Type::PROPERTY;
	else if (animTypeString == "grid_animation")
		return IAnimation::Type::GRID;
	else
		return IAnimation::Type::PARALLEL_GROUP;
}

template <>
EasingCurve::Type deserialize(LuaSerializer &ls, const char *name)
{
	lua_State *L = ls.luaState();
	nctl::String easingTypeString = nc::LuaUtils::retrieveField<const char *>(L, -1, name);

	if (easingTypeString == "linear")
		return EasingCurve::Type::LINEAR;
	else if (easingTypeString == "quad")
		return EasingCurve::Type::QUAD;
	else if (easingTypeString == "cubic")
		return EasingCurve::Type::CUBIC;
	else if (easingTypeString == "quart")
		return EasingCurve::Type::QUART;
	else if (easingTypeString == "quint")
		return EasingCurve::Type::QUINT;
	else if (easingTypeString == "sine")
		return EasingCurve::Type::SINE;
	else if (easingTypeString == "expo")
		return EasingCurve::Type::EXPO;
	else if (easingTypeString == "circ")
		return EasingCurve::Type::CIRC;
	else
		return EasingCurve::Type::LINEAR;
}

template <>
EasingCurve::Direction deserialize(LuaSerializer &ls, const char *name)
{
	lua_State *L = ls.luaState();
	nctl::String directionString = nc::LuaUtils::retrieveField<const char *>(L, -1, name);

	if (directionString == "forward")
		return EasingCurve::Direction::FORWARD;
	else if (directionString == "backward")
		return EasingCurve::Direction::BACKWARD;
	else
		return EasingCurve::Direction::FORWARD;
}

template <>
EasingCurve::LoopMode deserialize(LuaSerializer &ls, const char *name)
{
	lua_State *L = ls.luaState();
	nctl::String loopModeString = nc::LuaUtils::retrieveField<const char *>(L, -1, name);

	if (loopModeString == "disabled")
		return EasingCurve::LoopMode::DISABLED;
	else if (loopModeString == "rewind")
		return EasingCurve::LoopMode::REWIND;
	else if (loopModeString == "ping_pong")
		return EasingCurve::LoopMode::PING_PONG;
	else
		return EasingCurve::LoopMode::DISABLED;
}

void deserialize(LuaSerializer &ls, const char *name, EasingCurve &curve)
{
	ASSERT(name);
	lua_State *L = ls.luaState();

	nc::LuaUtils::retrieveFieldTable(L, -1, name);

	curve.setType(deserialize<EasingCurve::Type>(ls, "type"));
	curve.setDirection(deserialize<EasingCurve::Direction>(ls, "direction"));
	curve.setLoopMode(deserialize<EasingCurve::LoopMode>(ls, "loop_mode"));
	curve.setStart(deserialize<float>(ls, "start_time"));
	curve.setEnd(deserialize<float>(ls, "end_time"));
	curve.setScale(deserialize<float>(ls, "scale"));
	curve.setShift(deserialize<float>(ls, "shift"));

	nc::LuaUtils::pop(L);
}

void deserialize(LuaSerializer &ls, nctl::UniquePtr<PropertyAnimation> &anim)
{
	DeserializerContext *context = static_cast<DeserializerContext *>(ls.context());

	Sprite *sprite = deserializePtr(ls, "sprite", *context->sprites);
	anim->setSprite(sprite);
	anim->setSpeed(deserialize<float>(ls, "speed"));
	deserialize(ls, "curve", anim->curve());
	const char *propertyName = deserialize<const char *>(ls, "property_name");
	anim->setPropertyName(propertyName);
	Properties::assign(*anim, propertyName);
}

void deserialize(LuaSerializer &ls, nctl::UniquePtr<GridAnimation> &anim)
{
	DeserializerContext *context = static_cast<DeserializerContext *>(ls.context());

	Sprite *sprite = deserializePtr(ls, "sprite", *context->sprites);
	anim->setSprite(sprite);
	anim->setSpeed(deserialize<float>(ls, "speed"));
	deserialize(ls, "curve", anim->curve());

	const char *functionName = deserialize<const char *>(ls, "function_name");
	const GridFunction **functionPtr = context->functionHash->find(functionName);
	if (functionPtr)
	{
		const GridFunction *function = *functionPtr;
		for (Array ar(ls, "parameters"); ar.hasNext(); ar.next())
		{
			const unsigned int i = ar.index();
			const char *paramName = deserialize<const char *>(ls, "name");
			if (function->parameterInfo(i).name == paramName)
			{
				if (function->parameterType(i) == GridFunction::ParameterType::FLOAT)
					anim->parameters()[i].value0 = deserialize<float>(ls, "value");
				else if (function->parameterType(i) == GridFunction::ParameterType::VECTOR2F)
				{
					const nc::Vector2f valueVec = deserialize<nc::Vector2f>(ls, "value");
					anim->parameters()[i].value0 = valueVec.x;
					anim->parameters()[i].value1 = valueVec.y;
				}
			}
		}
	}
}

void deserialize(LuaSerializer &ls, nctl::UniquePtr<IAnimation> &anim)
{
	DeserializerContext *context = static_cast<DeserializerContext *>(ls.context());

	IAnimation::Type type = deserialize<IAnimation::Type>(ls, "type");

	switch (type)
	{
		case IAnimation::Type::PARALLEL_GROUP:
		{
			nctl::UniquePtr<ParallelAnimationGroup> parallelAnim = nctl::makeUnique<ParallelAnimationGroup>();
			anim = nctl::move(parallelAnim);
			break;
		}
		case IAnimation::Type::SEQUENTIAL_GROUP:
		{
			nctl::UniquePtr<SequentialAnimationGroup> sequentialAnim = nctl::makeUnique<SequentialAnimationGroup>();
			anim = nctl::move(sequentialAnim);
			break;
		}
		case IAnimation::Type::PROPERTY:
		{
			nctl::UniquePtr<PropertyAnimation> propertyAnim = nctl::makeUnique<PropertyAnimation>();
			deserialize(ls, propertyAnim);
			anim = nctl::move(propertyAnim);
			break;
		}
		case IAnimation::Type::GRID:
		{
			nctl::UniquePtr<GridAnimation> gridAnim = nctl::makeUnique<GridAnimation>();
			deserialize(ls, gridAnim);
			anim = nctl::move(gridAnim);
			break;
		}
	}

	deserialize(ls, "name", anim->name);
	AnimationGroup *parent = static_cast<AnimationGroup *>(deserializePtr(ls, "parent", *context->animations));
	if (parent == nullptr)
		parent = &theAnimMgr->animGroup();
	anim->setParent(parent);
}

void deserialize(LuaSerializer &ls, const char *name, SaveAnim &saveAnim)
{
	lua_State *L = ls.luaState();
	nc::LuaUtils::retrieveGlobalTable(L, name);

	saveAnim.numFrames = deserialize<int>(ls, "num_frames");
	saveAnim.fps = deserialize<int>(ls, "fps");
	saveAnim.canvasResize = deserialize<float>(ls, "canvas_resize");

	nc::LuaUtils::pop(L);
}

}