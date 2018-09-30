#pragma once

#include "algors.h"
#include "Receiver.h"
#include "ECS_Utilities.h"

class Sender
{
public:
	using receiver_resource = shared_resource<Receiver>;
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
		erase_if( receivers, 
			[ & ]( const receiver_resource _recv ){ return _receiver == _recv; } );
	}

	template<typename MessageType, typename...Args> 
	void send( Args&&... _args )
	{
		for( auto* receiver : receivers )
		{
			receiver->receive( MessageType( std::forward<Args>( _args )... ) );
		}
	}

private:
	auto find_receiver( receiver_resource _receiver)const noexcept->
		std::vector<receiver_resource>::const_iterator
	{
		return find_if( receivers, 
			[ & ]( receiver_resource _recv ){ return _receiver == _recv; } );
		
		/*
		return std::find_if(
			receivers.begin(),
			receivers.end(),
			[ & ]( receiver_resource _recv )
			{
				return _receiver == _recv;
			} );*/
	}
private:
	std::vector<receiver_resource> receivers;
};