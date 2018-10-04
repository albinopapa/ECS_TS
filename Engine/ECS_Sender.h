#pragma once

#include "ECS_Algorithms.h"
#include "ECS_Receiver.h"
#include "ECS_Utilities.h"

namespace screws
{
	template<typename MessageVariant>
	class ECS_Sender
	{
	public:
		using receiver_resource = shared_resource<ECS_Receiver<MessageVariant>>;
	public:
		void add_receiver( receiver_resource _receiver )
		{	
			if( auto findit = find_receiver( _receiver ); 
				findit == receivers.end() )
			{
				receivers.push_back( _receiver );
			}
		}
		void remove_receiver( receiver_resource _receiver )
		{
			erase_if( receivers, is_same_resource( _receiver ) );
		}

		template<typename MessageType, typename...Args> 
		void send( Args&&... _args )
		{
			for( auto* receiver : receivers )
			{
				receiver->on_receive( MessageType( std::forward<Args>( _args )... ) );
			}
		}

	private:
		auto find_receiver( receiver_resource _receiver)const noexcept->
			typename std::vector<receiver_resource>::const_iterator
		{	
			return find_if( receivers, is_same_resource( _receiver ) );
		}

	private:	
		std::vector<receiver_resource> receivers;
	};
}