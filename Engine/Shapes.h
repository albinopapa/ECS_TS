#pragma once
#include "Vec2.h"
struct Rect { float left, top, right, bottom; };
struct Circle { Vec2 center; float radius; };

struct Translator
{
	Rect operator()( const Vec2& _offset, const Rect& _rect )
	{
		return { 
			_rect.left + _offset.x,
			_rect.top + _offset.y,
			_rect.right + _offset.x,
			_rect.bottom + _offset.y 
		};
	}
	Circle operator()( const Vec2& _offset, const Circle& _circle )
	{
		return { _circle.center + _offset, _circle.radius };
	}
};