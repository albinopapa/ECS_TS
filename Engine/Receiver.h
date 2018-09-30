#pragma once

#include "Message.h"
#include <variant>
#include <vector>

class Receiver
{
public:
	template<typename MessageType> void receive( MessageType _message )
	{
		messages.push_back( _message );
	}
	const std::vector<message_t>& get_messages()const { return messages; }
	void clear_messages() { messages.clear(); }
protected:
	std::vector<message_t> messages;
};