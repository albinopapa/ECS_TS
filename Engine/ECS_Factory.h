#pragma once

#include "ECS_Utilities.h"

/*
	Sender
	Component
	Receiver
	Entity
	World
	System
*/

namespace screws
{
	template<
		typename ComponentList,
		typename EntityList,
		typename MessageList, 
		typename ReceiverList,
		typename SenderList,
		typename SystemList, 
		typename WorldList
	>
	class ECS_Factory
	{
	public:
		using component_t = ComponentList;
		using entity_t = EntityList;
		using message_t = MessageList;
		using receiver_t = ReceiverList;
		using sender_t = SenderList;
		using system_t = SystemList;
		using world_t = WorldList;

	public:
		static ECS_Factory& instance()
		{
			static ECS_Factory factory;
			return factory;
		}

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

		template<typename ReceiverType>
		shared_resource<receiver_t> create_receiver()
		{
			receiver_pool.push_back( ReceiverType() );
			return receiver_pool.back();
		}

		template<typename SenderType>
		shared_resource<sender_t> create_sender()
		{
			sender_pool.push_back( SenderType() );
			return sender_pool.back();
		}

	private:
		ECS_Factory() = default;
		ECS_Factory( const ECS_Factory& ) = delete;
		ECS_Factory( ECS_Factory&& ) = delete;
		ECS_Factory& operator=( const ECS_Factory& ) = delete;
		ECS_Factory& operator=( ECS_Factory&& ) = delete;

	private:
		shared_pool<component_t> component_pool;
		shared_pool<entity_t> entity_pool;
		shared_pool<message_t> message_pool;
		shared_pool<receiver_t> receiver_pool;
		shared_pool<sender_t> sender_pool;
		shared_pool<system_t> system_pool;
		shared_pool<world_t> world_pool;
	};
}
