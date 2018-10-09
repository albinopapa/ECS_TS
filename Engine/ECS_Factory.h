#pragma once

#include "ECS_Receiver.h"
#include "ECS_Sender.h"
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
		typename SystemList, 
		typename WorldList
	>
	class ECS_Factory
	{
	public:
		using component_t = ComponentList;
		using entity_t = EntityList;
		using message_t = MessageList;
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
			auto result = entity_pool.back();
			return result;
		}
		template<typename EntityType>
		shared_resource<entity_t> find_entity( EntityType& _entity )
		{
			for( size_t i = 0; i < entity_pool.size(); ++i )
			{
				if( std::visit( [ & ]( auto& ent )
					{	
						using type = std::decay_t<decltype( ent )>;
						if constexpr( std::is_same_v<type, EntityType> )
						{
							return &_entity == &ent;
						}
						return false;
					}, *entity_pool[ i ] ) )
				{
					return entity_pool[ i ];
				}
				else
				{
					return shared_resource<entity_t>();
				}
			}
		}
		template<typename ComponentType, typename...Args>
		shared_resource<component_t> create_component( Args&&... _args )
		{
			component_pool.push_back( ComponentType( std::forward<Args>( _args )... ) );
			auto result = component_pool.back();
			return result;
		}

		template<typename MessageType, typename...Args>
		shared_resource<message_t> create_message( Args&&... _args )
		{
			message_pool.push_back( MessageType( std::forward<Args>( _args )... ) );
			auto result = message_pool.back();
			return result;
		}

		template<typename SystemType, typename...Args>
		shared_resource<system_t> create_system( Args&&... _args )
		{
			system_pool.push_back( SystemType( std::forward<Args>( _args )... ) );
			auto result = system_pool.back();
			return result;
		}

		template<typename WorldType, typename...Args>
		shared_resource<world_t> create_world()
		{
			world_pool.push_back( WorldType() );
			auto result = world_pool.back();
			return result;
		}

		shared_resource<ECS_Receiver<message_t>> create_receiver()
		{
			receiver_pool.push_back( ECS_Receiver<message_t>() );
			auto result = receiver_pool.back();
			return result;
		}

		shared_resource<ECS_Sender<message_t>> create_sender()
		{
			sender_pool.push_back( ECS_Sender<message_t>() );
			auto result = sender_pool.back();
			return result;
		}

		void release_all()
		{
			world_pool.clear();
			system_pool.clear();
			sender_pool.clear();
			receiver_pool.clear();
			message_pool.clear();
			entity_pool.clear();
			component_pool.clear();
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
		shared_pool<ECS_Receiver<message_t>> receiver_pool;
		shared_pool<ECS_Sender<message_t>> sender_pool;
		shared_pool<system_t> system_pool;
		shared_pool<world_t> world_pool;
	};
}
