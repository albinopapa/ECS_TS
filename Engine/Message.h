#pragma once

#include "Includes.h"

struct ComponentAdded
{
	ComponentAdded() = default;
	ComponentAdded( shared_resource<entity_t> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<entity_t> entity;
};
struct ComponentRemoved
{
	ComponentRemoved() = default;
	ComponentRemoved( shared_resource<entity_t> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<entity_t> entity;
};
struct EntityAdded
{
	EntityAdded() = default;
	EntityAdded( shared_resource<entity_t> _entity )
		:
		entity( std::move( _entity ) )
	{
	}

	shared_resource<entity_t> entity;
};
struct EntityRemoved
{
	EntityRemoved() = default;
	EntityRemoved( shared_resource<entity_t> _entity )
		:
		entity( _entity )
	{
	}

	shared_resource<entity_t> entity;
};
struct SystemAdded 
{ 
	SystemAdded() = default;
	SystemAdded( shared_resource<system_t> _system )
		:
		system( _system )
	{
	}

	shared_resource<system_t> system;
};
struct SystemRemoved
{
	SystemRemoved() = default;
	SystemRemoved( shared_resource<system_t> _system )
		:
		system( _system )
	{
	}

	shared_resource<system_t> system;
};
