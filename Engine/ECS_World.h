#pragma once

#include "ECS_Mailbox.h"
#include "ECS_Utilities.h"
#include <vector>

namespace screws
{
	template<
		typename SystemVaraint, 
		typename MessageVariant, 
		typename MessageFilter>
	class ECS_World :
		public ECS_Mailbox<MessageVariant, MessageFilter>
	{
	public:
		using system_resource = shared_resource<SystemVaraint>;

	public:
		void add_system( system_resource _system )
		{
			systems.push_back( _system );
		}

		void remove_system( const system_resource _system )
		{
			if( auto it = find_if( systems, is_same_resource( _system ) ); 
				it != systems.end() )
			{
				systems.erase( it );
			}
		}

		template<typename Visitor>
		void tick( Visitor _visitor )
		{
			for( auto& vsystem : systems )
			{
				std::visit( _visitor, *vsystem );
			}
		}

		void set_mailbox( 
			shared_resource<ECS_Receiver<MessageVariant>> _receiver,
			shared_resource<ECS_Sender<MessageVariant>> _sender )
		{
			receiver = std::move( _receiver );
			sender = std::move( _sender );
		}
	private:
		void process_messages()
		{

		}

	private:
		std::vector<system_resource> systems;
	};
}
