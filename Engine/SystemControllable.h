#pragma once

#include "Includes.h"
#include "SystemMessageDispatchBase.h"
#include "Keyboard.h"
#include "Message.h"

class ControllableExecuteDispatch
{
public:
	ControllableExecuteDispatch( Controllable& _parent, Keyboard& _keyboard );
	template<typename T>
	auto operator()( T& _entity )->
		std::enable_if_t<is_entity<T>::value,void>
	{
		while( !kbd.KeyIsEmpty() )
		{
			const auto key_event = kbd.ReadKey();
			if( key_event.IsPress() )
				parent.send_message<KeyPress>( g_factory.find_entity( _entity ), key_event );
			else if( key_event.IsRelease() )
				parent.send_message<KeyRelease>( g_factory.find_entity( _entity ), key_event );
		}
	}
	//void operator()( Player& _entity );

	// Unhandled
	/*template<typename Type>
	void operator()( Type& _entity ) {}*/
private:
	Controllable& parent;
	Keyboard& kbd;
};

class ControllableMessageDispatch : public SystemMessageDispatchBase<Controllable, Velocity>
{
public:
	ControllableMessageDispatch( Controllable& _parent );
	/*void operator()( const ComponentAdded& _message );
	void operator()( const ComponentRemoved& _message );
	void operator()( const EntityAdded& _message );
	void operator()( const EntityRemoved& _message );*/
	void operator()( KeyPress& _message );
	void operator()( KeyRelease& _message );
	
	// Unhandled
	template<typename T>
	void operator()( const T& ) {}
private:
	//Controllable& parent;
};