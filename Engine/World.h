#pragma once

#include "Sender.h"
#include "Receiver.h"
#include "Message.h"
#include "System.h"
#include <algorithm>
#include <variant>
#include <vector>
#include "ECS_PostOffice.h"


struct WorldMsgFilter
{
	void operator()( EntityAdded ) {}
	void operator()( EntityRemoved ) {}
	void operator()( SystemAdded ) {}
	void operator()( SystemRemoved ) {}
};

class WorldBase : public ECS_Mailbox<WorldMsgFilter>
{
public:
	WorldBase()
		:
		ECS_Mailbox<WorldMsgFilter>( ECS_PostOffice::mailbox_factory<WorldMsgFilter>() )
	{
	}

	template<typename SystemType, typename...Args> 
	SystemType create_system(Args&&... _args)
	{
		return SystemType( std::forward<Args>( _args )... );		
	}
	template<typename SystemType>
	SystemType& add_system( SystemType _system )
	{
		systems.push_back( std::move( _system ) );
		shared_resource<system_t>& vsystem = systems.back();
		return std::get<SystemType>( *vsystem );
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
	bool send_if( shared_resource<Entity> _entity )
	{
		if( auto it = find_system<SystemType>(); it != systems.end() )
		{
			if( _entity->has_all<Req...>() )
			{
				send_message<MessageType>( std::move( _entity ) );
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
		for( auto& message : receiver->get_messages() )
		{
		}
		receiver->clear_messages();
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
