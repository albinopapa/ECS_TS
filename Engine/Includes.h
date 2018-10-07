#pragma once

#include "ECS_Screws.h"
#include "Colors.h"
#include "Shapes.h"

class Graphics;

// Component tags
struct damage_tag {};
struct dimension_tag {};
struct health_tag {};
struct orientation_tag {};
struct position_tag {};
struct shape_tag {};
struct shield_tag {};
struct velocity_tag {};

// Entity tags
class enemy_tag {};
class player_tag {};

// System tags
class movable_tag {};
class drawable_tag {};

// Message tags
struct componentAdded_tag {};
struct componentRemoved_tag {};
struct entityAdded_tag {};
struct entityRemoved_tag {};
struct systemAdded_tag {};
struct systemRemoved_tag {};

// World tags
class world_tag {};

// Quality of life aliases
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

using Damage = screws::ECS_Component<damage_tag>;
using Dimesnion = screws::ECS_Component<dimension_tag>;
using Health = screws::ECS_Component<health_tag>;
using Orientation = screws::ECS_Component<orientation_tag>;
using Position = screws::ECS_Component<position_tag>;
using Shape = screws::ECS_Component<shape_tag>;
using Shield = screws::ECS_Component<shield_tag>;
using Velocity = screws::ECS_Component<velocity_tag>;

// Component list alias
using component_t = std::variant<
	Damage,
	Dimesnion,
	Health,
	Orientation,
	Position,
	Shape,
	Shield,
	Velocity
>;

// Entity list alias
using entity_t = std::variant<
	screws::ECS_Entity<player_tag, component_t, std::variant<
	screws::ECS_Message<componentAdded_tag>,
	screws::ECS_Message<componentRemoved_tag>,
	screws::ECS_Message<entityAdded_tag>,
	screws::ECS_Message<entityRemoved_tag>,
	screws::ECS_Message<systemAdded_tag>,
	screws::ECS_Message<systemRemoved_tag>>>,
	screws::ECS_Entity<enemy_tag, component_t, std::variant<
	screws::ECS_Message<componentAdded_tag>,
	screws::ECS_Message<componentRemoved_tag>,
	screws::ECS_Message<entityAdded_tag>,
	screws::ECS_Message<entityRemoved_tag>,
	screws::ECS_Message<systemAdded_tag>,
	screws::ECS_Message<systemRemoved_tag>>>>;

class MovableDispatcher
{
public:
	MovableDispatcher( float _dt );
	void operator()( screws::ECS_Entity<player_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>& _entity );
	void operator()( screws::ECS_Entity<enemy_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>& _entity );
private:
	float dt;
};
class DrawableDispatcher
{
public:
	DrawableDispatcher( Graphics& _graphics );

	void operator()( const screws::ECS_Entity<player_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>& _entity );
	void operator()( const screws::ECS_Entity<enemy_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>& _entity );
private:
	Graphics& gfx;
};

using SystemMessageFilter =
screws::ECS_MessageFilter<
	screws::ECS_Message<componentAdded_tag>,
	screws::ECS_Message<componentRemoved_tag>,
	screws::ECS_Message<entityAdded_tag>,
	screws::ECS_Message<entityRemoved_tag>>;

class MovableMessageHandler
{
public:
	MovableMessageHandler( screws::ECS_System<
		MovableDispatcher,
		MovableMessageHandler,
		std::variant<
		screws::ECS_Entity<player_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>,
		screws::ECS_Entity<enemy_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>>,
		std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>,
		SystemMessageFilter>& _parent );
	void operator()( const screws::ECS_Message<componentAdded_tag>& _message );
	void operator()( const screws::ECS_Message<componentRemoved_tag>& _message );
	void operator()( const screws::ECS_Message<entityAdded_tag>& _message );
	void operator()( const screws::ECS_Message<entityRemoved_tag>& _message );
	// Unhandled
	template<typename MessageType>void operator()( const MessageType& ) {}

private:
	screws::ECS_System<
		MovableDispatcher,
		MovableMessageHandler,
		std::variant<
		screws::ECS_Entity<player_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>,
		screws::ECS_Entity<enemy_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>>,
		std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>,
		SystemMessageFilter>& parent;
	float dt;
};
class DrawableMessageHandler
{
public:
	DrawableMessageHandler( screws::ECS_System<
		DrawableDispatcher,
		DrawableMessageHandler,
		std::variant<
		screws::ECS_Entity<player_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>,
		screws::ECS_Entity<enemy_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>>,
		std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>,
		SystemMessageFilter>& _parent );
	void operator()( const screws::ECS_Message<componentAdded_tag>& _message );
	void operator()( const screws::ECS_Message<componentRemoved_tag>& _message );
	void operator()( const screws::ECS_Message<entityAdded_tag>& _message );
	void operator()( const screws::ECS_Message<entityRemoved_tag>& _message );
	// Unhandled
	template<typename MessageType>void operator()( const MessageType& ) {}

private:
	screws::ECS_System<
		DrawableDispatcher,
		DrawableMessageHandler,
		std::variant<
			screws::ECS_Entity<player_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>,
			screws::ECS_Entity<enemy_tag, component_t, std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>>>,
		std::variant<
			screws::ECS_Message<componentAdded_tag>,
			screws::ECS_Message<componentRemoved_tag>,
			screws::ECS_Message<entityAdded_tag>,
			screws::ECS_Message<entityRemoved_tag>,
			screws::ECS_Message<systemAdded_tag>,
			screws::ECS_Message<systemRemoved_tag>>,
		SystemMessageFilter>& parent;
};

using system_t = std::variant<
	screws::ECS_System<
		MovableDispatcher, 
		MovableMessageHandler, 
		std::variant<
		screws::ECS_Entity<player_tag, component_t, std::variant<
	screws::ECS_Message<componentAdded_tag>,
	screws::ECS_Message<componentRemoved_tag>,
	screws::ECS_Message<entityAdded_tag>,
	screws::ECS_Message<entityRemoved_tag>,
	screws::ECS_Message<systemAdded_tag>,
	screws::ECS_Message<systemRemoved_tag>>>,
		screws::ECS_Entity<enemy_tag, component_t, std::variant<
	screws::ECS_Message<componentAdded_tag>,
	screws::ECS_Message<componentRemoved_tag>,
	screws::ECS_Message<entityAdded_tag>,
	screws::ECS_Message<entityRemoved_tag>,
	screws::ECS_Message<systemAdded_tag>,
	screws::ECS_Message<systemRemoved_tag>>>>,
		std::variant<
		screws::ECS_Message<componentAdded_tag>,
		screws::ECS_Message<componentRemoved_tag>,
		screws::ECS_Message<entityAdded_tag>,
		screws::ECS_Message<entityRemoved_tag>,
		screws::ECS_Message<systemAdded_tag>,
		screws::ECS_Message<systemRemoved_tag>>, 
	SystemMessageFilter>,
		screws::ECS_System<
			DrawableDispatcher, 
			DrawableMessageHandler, 
			std::variant<
				screws::ECS_Entity<player_tag, component_t, std::variant<
					screws::ECS_Message<componentAdded_tag>,
					screws::ECS_Message<componentRemoved_tag>,
					screws::ECS_Message<entityAdded_tag>,
					screws::ECS_Message<entityRemoved_tag>,
					screws::ECS_Message<systemAdded_tag>,
					screws::ECS_Message<systemRemoved_tag>>>,
				screws::ECS_Entity<enemy_tag, component_t, std::variant<
					screws::ECS_Message<componentAdded_tag>,
					screws::ECS_Message<componentRemoved_tag>,
					screws::ECS_Message<entityAdded_tag>,
					screws::ECS_Message<entityRemoved_tag>,
					screws::ECS_Message<systemAdded_tag>,
					screws::ECS_Message<systemRemoved_tag>>>>,
			std::variant<
				screws::ECS_Message<componentAdded_tag>,
				screws::ECS_Message<componentRemoved_tag>,
				screws::ECS_Message<entityAdded_tag>,
				screws::ECS_Message<entityRemoved_tag>,
				screws::ECS_Message<systemAdded_tag>,
				screws::ECS_Message<systemRemoved_tag>>, 
			SystemMessageFilter>
>;

// ECS_Message declarations
template<> struct screws::ECS_Message<componentAdded_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<entity_t> _entity );

	shared_resource<entity_t> entity;
};
template<> struct screws::ECS_Message<componentRemoved_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<entity_t> _entity );

	shared_resource<entity_t> entity;
};
template<> struct screws::ECS_Message<entityAdded_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<entity_t> _entity );

	shared_resource<entity_t> entity;
};
template<> struct screws::ECS_Message<entityRemoved_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<entity_t> _entity );

	shared_resource<entity_t> entity;
};
template<> struct screws::ECS_Message<systemAdded_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<system_t> _system );

	shared_resource<system_t> system;
};
template<> struct screws::ECS_Message<systemRemoved_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<system_t> _system );

	shared_resource<system_t> system;
};

// Message list alias
using message_t = std::variant<
	screws::ECS_Message<componentAdded_tag>,
	screws::ECS_Message<componentRemoved_tag>,
	screws::ECS_Message<entityAdded_tag>,
	screws::ECS_Message<entityRemoved_tag>,
	screws::ECS_Message<systemAdded_tag>,
	screws::ECS_Message<systemRemoved_tag>>;

// Receiver list alias
using receiver_t = std::variant<screws::ECS_Receiver<message_t>>;

// Sender list alias
using sender_t = std::variant<screws::ECS_Sender<message_t>>;



template<typename...ComponentList>
struct VerifyComponents
{
	template<typename T>
	auto operator()( const T& _entity )->
		std::enable_if_t<!std::is_same_v<T, std::monostate>, bool>
	{
		return _entity.has_all_components<ComponentList...>();
	}
};

class WorldDispatcher
{
public:
	WorldDispatcher( float _dt, Graphics& _graphics );
	void operator()( screws::ECS_System<
		MovableDispatcher,
		MovableMessageHandler,
		entity_t,
		message_t,
		SystemMessageFilter>& _system );
	void operator()( screws::ECS_System<
		DrawableDispatcher,
		DrawableMessageHandler,
		entity_t,
		message_t,
		SystemMessageFilter>& _system )const;
private:
	float dt;
	Graphics& gfx;
};

//using Movable = screws::ECS_System<
//	MovableDispatcher,
//	MovableMessageHandler,
//	entity_t,
//	message_t,
//	SystemMessageFilter>;
//
//using Drawable = screws::ECS_System<
//	DrawableDispatcher,
//	DrawableMessageHandler,
//	entity_t,
//	message_t,
//	SystemMessageFilter>;

using WorldMessageFilter =
screws::ECS_MessageFilter<
	screws::ECS_Message<systemAdded_tag>,
	screws::ECS_Message<systemRemoved_tag>>;


// World list alias
using world_t = std::variant<
	screws::ECS_World<
	std::variant<
		screws::ECS_System<
			MovableDispatcher,
			MovableMessageHandler,
			std::variant<
				screws::ECS_Entity<player_tag, component_t, std::variant<
					screws::ECS_Message<componentAdded_tag>,
					screws::ECS_Message<componentRemoved_tag>,
					screws::ECS_Message<entityAdded_tag>,
					screws::ECS_Message<entityRemoved_tag>,
					screws::ECS_Message<systemAdded_tag>,
					screws::ECS_Message<systemRemoved_tag>>>,
				screws::ECS_Entity<enemy_tag, component_t, std::variant<
					screws::ECS_Message<componentAdded_tag>,
					screws::ECS_Message<componentRemoved_tag>,
					screws::ECS_Message<entityAdded_tag>,
					screws::ECS_Message<entityRemoved_tag>,
					screws::ECS_Message<systemAdded_tag>,
					screws::ECS_Message<systemRemoved_tag>>>>,
			std::variant<
				screws::ECS_Message<componentAdded_tag>,
				screws::ECS_Message<componentRemoved_tag>,
				screws::ECS_Message<entityAdded_tag>,
				screws::ECS_Message<entityRemoved_tag>,
				screws::ECS_Message<systemAdded_tag>,
				screws::ECS_Message<systemRemoved_tag>>,
			SystemMessageFilter>,
		screws::ECS_System<
			DrawableDispatcher,
			DrawableMessageHandler,
			std::variant<
				screws::ECS_Entity<player_tag, component_t, std::variant<
					screws::ECS_Message<componentAdded_tag>,
					screws::ECS_Message<componentRemoved_tag>,
					screws::ECS_Message<entityAdded_tag>,
					screws::ECS_Message<entityRemoved_tag>,
					screws::ECS_Message<systemAdded_tag>,
					screws::ECS_Message<systemRemoved_tag>>>,
				screws::ECS_Entity<enemy_tag, component_t, std::variant<
					screws::ECS_Message<componentAdded_tag>,
					screws::ECS_Message<componentRemoved_tag>,
					screws::ECS_Message<entityAdded_tag>,
					screws::ECS_Message<entityRemoved_tag>,
					screws::ECS_Message<systemAdded_tag>,
					screws::ECS_Message<systemRemoved_tag>>>>,
			std::variant<
				screws::ECS_Message<componentAdded_tag>,
				screws::ECS_Message<componentRemoved_tag>,
				screws::ECS_Message<entityAdded_tag>,
				screws::ECS_Message<entityRemoved_tag>,
				screws::ECS_Message<systemAdded_tag>,
				screws::ECS_Message<systemRemoved_tag>>,
				SystemMessageFilter>>,
			std::variant<
				screws::ECS_Message<componentAdded_tag>,
				screws::ECS_Message<componentRemoved_tag>,
				screws::ECS_Message<entityAdded_tag>,
				screws::ECS_Message<entityRemoved_tag>,
				screws::ECS_Message<systemAdded_tag>,
				screws::ECS_Message<systemRemoved_tag>>,
	WorldMessageFilter>>;

// Alias for the ECS_Factory type used in application
using Factory = screws::ECS_Factory<
	component_t,
	entity_t,
	message_t,
	system_t,
	world_t>;

inline Factory& g_factory = Factory::instance();
