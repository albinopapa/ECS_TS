#pragma once

#include "ECS_Factory.h"

struct Position;
struct Velocity;
struct Orientation;
struct Health;
struct Shape;
struct Shield;
struct Damage;
struct Dimension;

class Player;
class Enemy;

class Movable;
class Drawable;

struct ComponentAdded;
struct ComponentRemoved;
struct EntityAdded;
struct EntityRemoved;
struct SystemAdded;
struct SystemRemoved;

class World;

using component_t = std::variant<
	std::monostate,
	Position,
	Velocity,
	Orientation,
	Health,
	Shape,
	Shield,
	Damage,
	Dimension
>;

using entity_t = std::variant<
	std::monostate,
	Player,
	Enemy
>;

using system_t = std::variant<
	std::monostate,
	Movable,
	Drawable
>;

using world_t = std::variant<World>;

using message_t = std::variant<
	std::monostate,
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved,
	SystemAdded,
	SystemRemoved
>;

using Factory = screws::ECS_Factory<entity_t, component_t, message_t, system_t, world_t>;
