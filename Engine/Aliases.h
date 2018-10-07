#pragma once

#include "Includes.h"

using Player = screws::ECS_Entity<player_tag, component_t, message_t>;
using Enemy = screws::ECS_Entity<enemy_tag, component_t, message_t>;

using ComponentAdded = screws::ECS_Message<componentAdded_tag>;
using ComponentRemoved = screws::ECS_Message<componentRemoved_tag>;
using EntityAdded = screws::ECS_Message<entityAdded_tag>;
using EntityRemoved = screws::ECS_Message<entityRemoved_tag>;
using SystemAdded = screws::ECS_Message<systemAdded_tag>;
using SystemRemoved = screws::ECS_Message<systemRemoved_tag>;

using Movable = screws::ECS_System<
	MovableDispatcher,
	MovableMessageHandler,
	entity_t,
	message_t,
	SystemMessageFilter>;

using Drawable = screws::ECS_System<
	DrawableDispatcher,
	DrawableMessageHandler,
	entity_t,
	message_t,
	SystemMessageFilter>;

using World = screws::ECS_World<system_t, message_t, WorldMessageFilter>;
