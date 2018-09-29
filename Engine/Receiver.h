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
protected:
	std::vector<message_t> messages;
};