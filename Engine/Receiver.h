#pragma once

#include "Message.h"
#include <optional>
#include <queue>

class Receiver
{
public:
	template<typename MessageType> void on_receive( MessageType _message )
	{
		messages.emplace( std::move( _message ) );
	}
	std::optional<message_t> get_message()noexcept
	{
		Finally do_last( [ & ]{ messages.pop(); } );
		
		return messages.empty() ?
			std::optional<message_t>{std::nullopt} :
			std::optional<message_t>{ messages.front() };
	}
	const std::queue<message_t>& get_messages()const { return messages; }
	
protected:

	std::queue<message_t> messages;
};