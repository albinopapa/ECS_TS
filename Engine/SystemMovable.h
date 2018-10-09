#pragma once

#include "Includes.h"
#include "SystemMessageDispatchBase.h"

class MovableExecuteDispatcher
{
public:
	MovableExecuteDispatcher( float _dt );
	void operator()( Player& _entity );
	void operator()( Enemy& _entity );

	// Unhandled - for types that don't move, but are a part of entity_t
	template<typename T>
	void operator()( T& _entity ) {}

private:
	float dt;
};
class MovableMessageDispatcher : public SystemMessageDispatchBase<Movable, Position, Velocity>
{
public:
	MovableMessageDispatcher( Movable& _parent );
	/*void operator()( const ComponentAdded& _message );
	void operator()( const ComponentRemoved& _message );
	void operator()( const EntityAdded& _message );
	void operator()( const EntityRemoved& _message );*/
	
	// Unhandled
	template<typename MessageType>void operator()( const MessageType& ) {}

private:
	//Movable& parent;
};


