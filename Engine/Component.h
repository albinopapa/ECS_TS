#pragma once

#include "Includes.h"
#include "Colors.h"
#include "Shapes.h"

// ECS_Component declarations
template<> struct screws::ECS_Component<velocity_tag>
{
	ECS_Component() = default;
	constexpr ECS_Component( float _x, float _y )noexcept
		:
		value( _x, _y )
	{
	}
	ECS_Component operator*( float _s )const
	{
		return { value.x * _s,value.y * _s };
	}
	Vec2 value;
};
template<> struct screws::ECS_Component<position_tag>
{
	ECS_Component() = default;
	constexpr ECS_Component( float _x, float _y )noexcept
		:
		value( _x, _y )
	{
	}
	ECS_Component& operator+=( const screws::ECS_Component<velocity_tag>& _vel )
	{
		value += _vel.value;
		return *this;
	}
	Vec2 value;
};
template<> struct screws::ECS_Component<orientation_tag>
{
	ECS_Component() = default;
	ECS_Component( float _angle )
		:
		angle( _angle )
	{
	}
	float angle;
};
template<> struct screws::ECS_Component<health_tag>
{
	ECS_Component() = default;
	ECS_Component( float _value )
		:
		value( _value )
	{
	}

	float value = 100.f;
};
template<> struct screws::ECS_Component<shape_tag>
{
	ECS_Component() = default;
	ECS_Component( Circle _circle, Color _color )
		:
		object( _circle ),
		color( _color )
	{
	}
	ECS_Component( Rect _rect, Color _color )
		:
		object( _rect ),
		color( _color )
	{
	}
	std::variant<Circle, Rect> object;
	Color color;
};
template<> struct screws::ECS_Component<shield_tag>
{
	ECS_Component() = default;
	ECS_Component( ECS_Component<shape_tag> _shape, const float _strength )
		:
		shape( _shape ), strength( _strength )
	{
	}
	ECS_Component<shape_tag> shape;
	float strength = 0.f;
};
template<> struct screws::ECS_Component<damage_tag>
{
	ECS_Component() = default;
	ECS_Component( const float _value )
		:
		value( _value )
	{
	}
	float value = 2.f;
};
template<> struct screws::ECS_Component<dimension_tag>
{
	ECS_Component() = default;
	ECS_Component( float _width, float _height )
		:
		width( _width ), height( _height )
	{
	}
	float width = 0.f, height = 0.f;
};

