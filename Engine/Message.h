#pragma once

#include <memory>
#include <variant>
#include "ECS_Utilities.h"

class Entity;

struct ComponentAdded
{
	ComponentAdded() = default;
	ComponentAdded( shared_resource<Entity> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<Entity> entity;
};
struct ComponentRemoved
{
	ComponentRemoved() = default;
	ComponentRemoved( shared_resource<Entity> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<Entity> entity;
};
struct EntityAdded
{
	EntityAdded() = default;
	EntityAdded( shared_resource<Entity> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<Entity> entity;
};
struct EntityRemoved
{
	EntityRemoved() = default;
	EntityRemoved( shared_resource<Entity> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<Entity> entity;
};
struct SystemAdded 
{ 
	SystemAdded() = default;
	SystemAdded( shared_resource<System> _system )
		:
		system( _system )
	{
	}

	shared_resource<System> system;
};
struct SystemRemoved
{
	SystemRemoved() = default;
	SystemRemoved( shared_resource<System> _system )
		:
		system( _system )
	{
	}

	shared_resource<System> system;
};

using message_t = std::variant<
	std::monostate,
	ComponentAdded,
	ComponentRemoved,
	EntityAdded,
	EntityRemoved,
	SystemAdded,
	SystemRemoved
>;
