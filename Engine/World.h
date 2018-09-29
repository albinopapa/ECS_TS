#pragma once

#include "Sender.h"
#include "Receiver.h"
#include "Message.h"
#include "System.h"
#include <algorithm>
#include <variant>
#include <vector>


class WorldBase : public Sender, public Receiver
{
public:
	WorldBase()
	{}
	template<typename SystemType, typename...Args> 
	SystemType create_system()
	{
		return SystemType( entity_manager );
	}
	template<typename SystemType>
	SystemType& add_system( SystemType _system )
	{
		auto& vsystem = systems.emplace_back( std::move( _system ) );
		auto& system = std::get<SystemType>( vsystem );
		add_receiver( system.get_receiver() );
		return system;
	}
	template<typename SystemType> void remove_system()
	{
		erase_if( systems, [ & ]( auto& _object )
			{
				return std::holds_alternative<std::unique_ptr<SystemType>>( _object );
			} );
	}
	template<typename SystemType> shared_pool<system_t>::iterator find_system()
	{
		return std::find_if(
			systems.begin(),
			systems.end(),
			[ & ]( auto& _vsystem )
			{
				return std::holds_alternative<SystemType>( _vsystem );
			}
		);
	}

	template<typename SystemType>
	shared_pool<system_t>::const_iterator find_system()const
	{
		return std::find_if(
			systems.begin(),
			systems.end(),
			[ & ]( const auto& _vsystem )
			{
				return std::holds_alternative<SystemType>( _vsystem );
			}
		);
	}

protected:
	template<typename SystemType, typename...Args> auto execute_if( Args&&... _args )
	{
		if( auto findit = find_system<SystemType>(); findit != systems.end() )
		{
			std::get<SystemType>( *findit ).execute( std::forward<Args>( _args )... );
		}
	}
	template<typename SystemType, typename MessageType, typename...Req> 
	bool send_if( Entity* _entity )
	{
		if( auto it = find_system<SystemType>(); it != systems.end() )
		{
			if( _entity->has_all<Req...>() )
			{
				send<MessageType>( _entity );
				return true;
			}
		}

		return false;
	}
	template<typename SystemType, typename Pred, typename...Args>
	void process_messages( SystemType& _vsystem, Pred&& _execute, Args&&... _args )
	{
		auto handle_message = [ & ]( auto& _system )
		{
			using Type = std::decay_t<decltype( _system )>;

			if constexpr( std::is_same_v<SystemType, Type> )
			{
				_execute( std::forward<Args>( _args )... );
			}
		};

		std::visit( handle_message, system );
	}

protected:
	EntityManager entity_manager;
	shared_pool<system_t> systems;
};

class World : public WorldBase
{
public:
	void process_messages()
	{
		for( auto& message : messages )
		{
		}
	}
	void update( float _dt )
	{
		process_messages();
		if( systems.empty() ) return;
		
		execute_if<Movable>( _dt );
	}
	void draw( Graphics& _graphics )
	{
		execute_if<Drawable>( _graphics );
	}
};
