#pragma once

#include "ECS_Message.h"
#include <optional>
#include <queue>

namespace screws
{
	template<typename MessageVariant>
	class ECS_Receiver
	{
	public:
		template<typename MessageType> 
		void on_receive( MessageType _message )
		{
			messages.emplace( std::move( _message ) );
		}
		std::optional<MessageVariant> get_next_message()noexcept
		{
			if( messages.empty() )
			{
				return std::optional<MessageVariant>{std::nullopt};
			}

			Finally do_last( [ & ]{ messages.pop(); } );
			return std::optional<MessageVariant>{ messages.front() };
		}

	protected:
		std::queue<MessageVariant> messages;
	};
}