#include "SystemDrawable.h"
#include "Includes.h"
#include "Graphics.h"

DrawableDispatcher::DrawableDispatcher( Graphics& _graphics )
	:
	gfx( _graphics )
{
}

void DrawableDispatcher::operator()( const screws::ECS_Entity<player_tag, component_t>& _entity )
{
	const screws::ECS_Component<position_tag>& pos = 
		_entity.get_component<screws::ECS_Component<position_tag>>().value().get();

	const screws::ECS_Component<shape_tag>& shape = 
		_entity.get_component<screws::ECS_Component<shape_tag>>().value().get();

	auto fill_shape = [ & ]( const auto& _shape )
	{
		const auto shape_trans = Translator{}( pos.value, _shape );
		gfx.FillShape( shape_trans, shape.color );
	};

	std::visit( fill_shape, shape.object );
}
void DrawableDispatcher::operator()( const screws::ECS_Entity<enemy_tag, component_t>& _entity )
{
	const screws::ECS_Component<position_tag>& pos =
		_entity.get_component<screws::ECS_Component<position_tag>>().value().get();

	const screws::ECS_Component<shape_tag>& shape =
		_entity.get_component<screws::ECS_Component<shape_tag>>().value().get();

	auto fill_shape = [ & ]( const auto& _shape )
	{
		const auto shape_trans = Translator{}( pos.value, _shape );
		gfx.FillShape( shape_trans, shape.color );
	};

	std::visit( fill_shape, shape.object );
}

DrawableMessageHandler::DrawableMessageHandler( screws::ECS_System<
	DrawableDispatcher,
	DrawableMessageHandler,
	entity_t,
	message_t,
	SystemMessageFilter> & _parent )
	:
	parent( _parent )
{
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<componentAdded_tag>& _message )
{
	if( parent.has_entity(_message.entity) )
	{
		if( std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>,
			screws::ECS_Component<shape_tag>>(), 
			*_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<componentRemoved_tag>& _message )
{
	if( parent.has_entity(_message.entity) )
	{
		if( !std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>,
			screws::ECS_Component<shape_tag>>( ),
			*_message.entity ) )
		{
			parent.remove_entity( _message.entity );
		}
	}
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<entityAdded_tag>& _message )
{
	if( !parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>,
			screws::ECS_Component<shape_tag>>( ),
			*_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<entityRemoved_tag>& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<
			screws::ECS_Component<position_tag>,
			screws::ECS_Component<shape_tag>>( ),
			*_message.entity ) )
		{
			parent.remove_entity( _message.entity );
		}
	}
}
