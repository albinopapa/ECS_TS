#pragma once

#include "Includes.h"
#include "Keyboard.h"

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

template<> struct screws::ECS_Message<key_press_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<entity_t> _entity, Keyboard::Event _key_event );

	Keyboard::Event key_event;
	shared_resource<entity_t> entity;
};
template<> struct screws::ECS_Message<key_release_tag>
{
	ECS_Message() = default;
	ECS_Message( shared_resource<entity_t> _entity, Keyboard::Event _key_event );

	Keyboard::Event key_event;
	shared_resource<entity_t> entity;
};

