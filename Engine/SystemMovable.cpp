#include "SystemMovable.h"
#include "Component.h"
#include "Message.h"

MovableExecuteDispatcher::MovableExecuteDispatcher( float _dt )
	:
	dt( _dt )
{
}

void MovableExecuteDispatcher::operator()( Player& _entity )
{
	Position& pos = _entity.get_component<Position>().value().get();
	const Velocity& vel = _entity.get_component<Velocity>().value().get();
	pos += ( vel * dt );
}

void MovableExecuteDispatcher::operator()( Enemy& _entity )
{
	Position& pos = 
		_entity.get_component<Position>().value().get();
	const Velocity& vel =
		_entity.get_component<Velocity>().value().get();
	pos += ( vel * dt );
}

MovableMessageDispatcher::MovableMessageDispatcher( Movable& _parent )
	:
	SystemMessageDispatchBase<Movable, Position, Velocity>( _parent )
{
}

//void MovableMessageDispatcher::operator()( const ComponentAdded& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Velocity>(), *_message.entity ) )
//		{
//			parent.add_entity( std::move( _message.entity ) );
//		}
//	}
//}
//
//void MovableMessageDispatcher::operator()( const ComponentRemoved& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		if( !std::visit( VerifyComponents<Position, Velocity>( ), *_message.entity ) )
//		{
//			parent.remove_entity( std::move( _message.entity ) );
//		}
//	}
//}
//
//void MovableMessageDispatcher::operator()( const EntityAdded& _message )
//{
//	if( !parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Velocity>( ), *_message.entity ) )
//		{
//			parent.add_entity( std::move( _message.entity ) );
//		}
//	}
//}
//
//void MovableMessageDispatcher::operator()( const EntityRemoved& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		parent.remove_entity( _message.entity );
//	}
//}
