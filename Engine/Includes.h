#pragma once

#include "ECS_Screws.h"

// Tags to help forward declarations

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
struct enemy_tag {};
struct player_tag {};
struct wall_tag {};

// System tags
struct movable_tag {};
struct drawable_tag {};

// Message tags
struct componentAdded_tag {};
struct componentRemoved_tag {};
struct entityAdded_tag {};
struct entityRemoved_tag {};
struct systemAdded_tag {};
struct systemRemoved_tag {};
struct key_press_tag {};
struct key_release_tag {};
struct mouse_button_press_tag {};
struct mouse_button_release_tag {};
struct mouse_wheel_scroll_tag {};

// World tags
struct world_tag {};

// Quality of life aliases

// Component aliases
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

// TODO: Look into making add/remove messages internal
// Message aliases
using ComponentAdded = screws::ECS_Message<componentAdded_tag>;
using ComponentRemoved = screws::ECS_Message<componentRemoved_tag>;
using EntityAdded = screws::ECS_Message<entityAdded_tag>;
using EntityRemoved = screws::ECS_Message<entityRemoved_tag>;
using SystemAdded = screws::ECS_Message<systemAdded_tag>;
using SystemRemoved = screws::ECS_Message<systemRemoved_tag>;
using KeyPress = screws::ECS_Message<key_press_tag>;
using KeyRelease = screws::ECS_Message<key_release_tag>;

// Message list alias
using message_t = std::variant<
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved,
	SystemAdded,
	SystemRemoved,
	KeyPress,
	KeyRelease
>;

// Entity aliases / forward declarations
using Player = screws::ECS_Entity<player_tag, component_t, message_t>;
using Enemy  = screws::ECS_Entity<enemy_tag, component_t, message_t>;
using Wall   = screws::ECS_Entity<wall_tag, component_t, message_t>;

// Type trait
template<typename T> struct is_entity :std::conditional_t<std::disjunction_v<
	std::conditional_t<std::is_same_v<T, Player >,std::true_type,std::false_type>,
	std::conditional_t<std::is_same_v<T, Enemy  >,std::true_type,std::false_type>,
	std::conditional_t<std::is_same_v<T, Wall   >,std::true_type,std::false_type>
>, std::true_type, std::false_type>
{
};

// Entity list alias
using entity_t = std::variant<Player, Enemy, Wall>;

// Message filter alias
using MovableMessageFilter = screws::ECS_MessageFilter<
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved>;

using DrawableMessageFilter = screws::ECS_MessageFilter<
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved>;

using CollidableMessageFilter = screws::ECS_MessageFilter<
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved
>;

using ControllableMessageFilter = screws::ECS_MessageFilter<
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved,
	KeyPress,
	KeyRelease
>;

using WorldMessageFilter = screws::ECS_MessageFilter<SystemAdded, SystemRemoved>;

// System classes forward declarations
class DrawableExecuteDispatcher;
class DrawableMessageDispatcher;
class CollidableExecuteDispatch;
class CollidableMessageDispatch;
class ControllableExecuteDispatch;
class ControllableMessageDispatch;
class MovableExecuteDispatcher;
class MovableMessageDispatcher;


using Drawable = screws::ECS_System<
	DrawableExecuteDispatcher,
	DrawableMessageDispatcher,
	entity_t,
	message_t,
	DrawableMessageFilter>;

using Collidable = screws::ECS_System<
	CollidableExecuteDispatch,
	CollidableMessageDispatch,
	entity_t,
	message_t,
	CollidableMessageFilter
>;

using Controllable = screws::ECS_System<
	ControllableExecuteDispatch,
	ControllableMessageDispatch,
	entity_t,
	message_t,
	ControllableMessageFilter
>;

using Movable = screws::ECS_System<
	MovableExecuteDispatcher,
	MovableMessageDispatcher,
	entity_t,
	message_t,
	MovableMessageFilter>;



// System list alias
using system_t = std::variant<Collidable, Controllable, Drawable, Movable>;

// Receiver list alias
using receiver_t = std::variant<screws::ECS_Receiver<message_t>>;

// Sender list alias
using sender_t = std::variant<screws::ECS_Sender<message_t>>;

using World = screws::ECS_World<system_t, message_t, WorldMessageFilter>;

// World list alias
using world_t = std::variant<World>;

// Alias for the ECS_Factory type used in application
using Factory = screws::ECS_Factory<
	component_t,
	entity_t,
	message_t,
	system_t,
	world_t>;

inline Factory& g_factory = Factory::instance();
