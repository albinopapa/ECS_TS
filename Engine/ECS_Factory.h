#pragma once

#include "ECS_Utilities.h"

/*
	System
	Component
	Receiver
	Entity
	World
	Sender
*/

namespace screws
{
	template<typename EntityList, typename ComponentList, typename MessageList, typename SystemList, typename WorldList>
	class ECS_Factory
	{
	public:
		using entity_t = EntityList;
		using component_t = ComponentList;
		using message_t = MessageList;
		using system_t = SystemList;
		using world_t = WorldList;

	public:
		template<typename EntityType, typename...Args>
		shared_resource<entity_t> create_entity( Args&&... _args )
		{
			entity_pool.push_back( EntityType( std::forward<Args>( _args )... ) );
			return entity_pool.back();
		}

		template<typename ComponentType, typename...Args>
		shared_resource<component_t> create_component( Args&&... _args )
		{
			component_pool.push_back( ComponentType( std::forward<Args>( _args )... ) );
			return component_pool.back();
		}

		template<typename MessageType, typename...Args>
		shared_resource<message_t> create_message( Args&&... _args )
		{
			message_pool.push_back( MessageType( std::forward<Args>( _args )... ) );
			return message_pool.back();
		}

		template<typename SystemType, typename...Args>
		shared_resource<system_t> create_system( Args&&... _args )
		{
			system_pool.push_back( SystemType( std::forward<Args>( _args )... ) );
			return system_pool.back();
		}

		template<typename WorldType, typename...Args>
		shared_resource<world_t> create_world()
		{
			world_pool.push_back( WorldType() );
			return world_pool.back();
		}

	private:
		shared_pool<entity_t> entity_pool;
		shared_pool<component_t> component_pool;
		shared_pool<message_t> message_pool;
		shared_pool<system_t> system_pool;
		shared_pool<world_t> world_pool;
	};
}
