#pragma once

#include "Shapes.h"
#include <variant>

struct Position 
{ 
	Position() = default; 
	constexpr Position( float _x, float _y )noexcept
		:
		value( _x,_y )
	{} 
	Vec2 value; 
};
struct Velocity 
{ 
	Velocity() = default;
	constexpr Velocity( float _x, float _y )noexcept
		:
		value( _x, _y )
	{
	}
		
	Vec2 value; 
};
struct Orientation 
{ 
	Orientation() = default;
	Orientation( float _x, float _y )
		:
		value( _x, _y )
	{
	}
	Vec2 value; 
};
struct Health 
{ 
	Health() = default; 
	Health( float _value )
		:
		value( _value )
	{
	}

	float value = 100.f; };
struct Shape
{
	Shape() = default;
	Shape( Circle _circle, Color _color )
		:
		object( _circle ),
		color( _color )
	{
	}
	Shape( Rect _rect, Color _color )
		:
		object( _rect ),
		color( _color )
	{}
	std::variant<Circle, Rect> object;
	Color color;
};
struct Shield
{ 
	Shape shape;
	float strength = 0.f; 
};
struct Damage
{
	float _value = 2.f;
};
struct Dimension
{
	float width = 0.f, height = 0.f;
};
