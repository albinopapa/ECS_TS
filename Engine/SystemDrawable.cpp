#include "SystemDrawable.h"
#include "Aliases.h"
#include "Graphics.h"

DrawableDispatcher::DrawableDispatcher( Graphics& _graphics )
	:
	gfx( _graphics )
{
}

void DrawableDispatcher::operator()( const Player& _entity )
{
	const Position& pos = 
		_entity.get_component<Position>().value().get();

	const Shape& shape =
		_entity.get_component<Shape>().value().get();

	auto fill_shape = [ & ]( const auto& _shape )
	{
		const auto shape_trans = Translator{}( pos.value, _shape );
		gfx.FillShape( shape_trans, shape.color );
	};

	std::visit( fill_shape, shape.object );
}
void DrawableDispatcher::operator()( const Enemy& _entity )
{
	const Position& pos =
		_entity.get_component<Position>().value().get();

	const Shape& shape =
		_entity.get_component<Shape>().value().get();

	auto fill_shape = [ & ]( const auto& _shape )
	{
		const auto shape_trans = Translator{}( pos.value, _shape );
		gfx.FillShape( shape_trans, shape.color );
	};

	std::visit( fill_shape, shape.object );
}

DrawableMessageHandler::DrawableMessageHandler( Drawable& _parent )
	:
	parent( _parent )
{
}

void DrawableMessageHandler::operator()( const ComponentAdded& _message )
{
	if( parent.has_entity(_message.entity) )
	{
		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<componentRemoved_tag>& _message )
{
	if( parent.has_entity(_message.entity) )
	{
		if( !std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
		{
			parent.remove_entity( _message.entity );
		}
	}
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<entityAdded_tag>& _message )
{
	if( !parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
		{
			parent.add_entity( std::move( _message.entity ) );
		}
	}
}

void DrawableMessageHandler::operator()( const screws::ECS_Message<entityRemoved_tag>& _message )
{
	if( parent.has_entity( _message.entity ) )
	{
		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
		{
			parent.remove_entity( _message.entity );
		}
	}
}
