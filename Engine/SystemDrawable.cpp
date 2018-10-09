#include "SystemDrawable.h"
#include "Component.h"
#include "Message.h"
#include "Graphics.h"

DrawableExecuteDispatcher::DrawableExecuteDispatcher( Graphics& _graphics )
	:
	gfx( _graphics )
{
}

void DrawableExecuteDispatcher::operator()( const Player& _entity )
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

void DrawableExecuteDispatcher::operator()( const Enemy& _entity )
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

void DrawableExecuteDispatcher::operator()( const Wall& _entity )
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

DrawableMessageDispatcher::DrawableMessageDispatcher( Drawable& _parent )
	:
	SystemMessageDispatchBase<Drawable, Position, Shape>( _parent )
{
}

//void DrawableMessageDispatcher::operator()( const ComponentAdded& _message )
//{
//	if( parent.has_entity(_message.entity) )
//	{
//		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.add_entity( std::move( _message.entity ) );
//		}
//	}
//}
//
//void DrawableMessageDispatcher::operator()( const ComponentRemoved& _message )
//{
//	if( parent.has_entity(_message.entity) )
//	{
//		if( !std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.remove_entity( _message.entity );
//		}
//	}
//}
//
//void DrawableMessageDispatcher::operator()( const EntityAdded& _message )
//{
//	if( !parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.add_entity( std::move( _message.entity ) );
//		}
//	}
//}
//
//void DrawableMessageDispatcher::operator()( const EntityRemoved& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.remove_entity( _message.entity );
//		}
//	}
//}
