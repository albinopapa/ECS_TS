#pragma once

#include "Colors.h"
#include "Shapes.h"
#include <variant>

struct Velocity 
{ 
	Velocity() = default;
	constexpr Velocity( float _x, float _y )noexcept
		:
		value( _x, _y )
	{
	}
	Velocity operator*( float _s )const
	{		
		return { value.x * _s,value.y * _s };
	}
	Vec2 value; 
};
struct Position 
{ 
	Position() = default; 
	constexpr Position( float _x, float _y )noexcept
		:
		value( _x,_y )
	{}
	Position& operator+=( const Velocity& _vel )
	{
		value += _vel.value;
		return *this;
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
	Shield() = default;
	Shield( Shape _shape, const float _strength )
		:
		shape( _shape ), strength( _strength )
	{
	}
	Shape shape;
	float strength = 0.f; 
};
struct Damage
{
	Damage() = default;
	Damage( const float _value )
		:
		value( _value )
	{
	}
	float value = 2.f;
};
struct Dimension
{
	Dimension() = default;
	Dimension( float _width, float _height )
		:
		width( _width ), height( _height )
	{
	}
	float width = 0.f, height = 0.f;
};
