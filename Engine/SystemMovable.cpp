#include "SystemMovable.h"
#include "Aliases.h"

MovableDispatcher::MovableDispatcher( float _dt )
	:
	dt( _dt )
{
}

void MovableDispatcher::operator()( Player& _entity )
{
	Position& pos = _entity.get_component<Position>().value().get();
	const Velocity& vel = _entity.get_component<Velocity>().value().get();
	pos += ( vel * dt );
}

void MovableDispatcher::operator()( Enemy& _entity )
{
	Position& pos = 
		_entity.get_component<Position>().value().get();
	const Velocity& vel =
		_entity.get_component<Velocity>().value().get();
	pos += ( vel * dt );
}

MovableMessageHandler::MovableMessageHandler( Movable& _parent )
	:
	parent( _parent )
{
}

void MovableMessageHandler::operator()( const ComponentAdded& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<Position, Velocity>(), *_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void MovableMessageHandler::operator()( const ComponentRemoved& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		if( !std::visit( VerifyComponents<Position, Velocity>( ), *_message.entity ) )
		{
			parent.remove_entity( std::move( _message.entity ) );
		}
	}
}

void MovableMessageHandler::operator()( const EntityAdded& _message )
{
	if( !parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<Position, Velocity>( ), *_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void MovableMessageHandler::operator()( const EntityRemoved& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		parent.remove_entity( _message.entity );
	}
}
