#pragma once

#include "algors.h"
#include "Receiver.h"

class Sender
{
public:
	void add_receiver( Receiver* _receiver )
	{	
		auto findit = find_receiver( _receiver );

		if( findit == receivers.end() )
			receivers.push_back( _receiver );
	}
	void remove_receiver( Receiver*_receiver )
	{
		erase_if( receivers, [ & ]( const Receiver* _recv )
			{
				return _receiver == _recv;
			} );
	}

protected:
	template<typename MessageType, typename...Args> void send( Args&&... _args )
	{
		for( auto* receiver : receivers )
		{
			receiver->receive( MessageType( std::forward<Args>( _args )... ) );
		}
	}

private:
	template<typename ReceiverType>
	std::vector<Receiver*>::iterator find_receiver(ReceiverType* _receiver)
	{
		return std::find_if(
			receivers.begin(),
			receivers.end(),
			[ & ]( const Receiver* _recv )
			{
				return _receiver == _recv;
			} );
	}
private:
	std::vector<Receiver*> receivers;
};