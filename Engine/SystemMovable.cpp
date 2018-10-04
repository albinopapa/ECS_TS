#include "SystemMovable.h"
#include "Includes.h"

MovableDispatcher::MovableDispatcher( float _dt )
	:
	dt( _dt )
{
}

void MovableDispatcher::operator()( screws::ECS_Entity<player_tag, component_t>& _entity )
{
	screws::ECS_Component<position_tag>& pos = 
		_entity.get_component<screws::ECS_Component<position_tag>>().value().get();
	const screws::ECS_Component<velocity_tag>& vel = 
		_entity.get_component<screws::ECS_Component<velocity_tag>>().value().get();
	pos += ( vel * dt );
}

void MovableDispatcher::operator()( screws::ECS_Entity<enemy_tag, component_t>& _entity )
{
	screws::ECS_Component<position_tag>& pos = 
		_entity.get_component<screws::ECS_Component<position_tag>>().value().get();
	const screws::ECS_Component<velocity_tag>& vel =
		_entity.get_component<screws::ECS_Component<velocity_tag>>().value().get();
	pos += ( vel * dt );
}

MovableMessageHandler::MovableMessageHandler( screws::ECS_System<
	MovableDispatcher,
	MovableMessageHandler,
	entity_t,
	message_t,
	SystemMessageFilter>& _parent )
	:
	parent( _parent )
{
}

void MovableMessageHandler::operator()( const screws::ECS_Message<componentAdded_tag>& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>, 
			screws::ECS_Component<velocity_tag>>(), 
			*_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void MovableMessageHandler::operator()( const screws::ECS_Message<componentRemoved_tag>& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		if( !std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>,
			screws::ECS_Component<velocity_tag>>( ),
			*_message.entity ) )
		{
			parent.remove_entity( std::move( _message.entity ) );
		}
	}
}

void MovableMessageHandler::operator()( const screws::ECS_Message<entityAdded_tag>& _message )
{
	if( !parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>,
			screws::ECS_Component<velocity_tag>>( ),
			*_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void MovableMessageHandler::operator()( const screws::ECS_Message<entityRemoved_tag>& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		parent.remove_entity( _message.entity );
	}
}
