#pragma once

#include "Entity.h"
#include "ECS_System.h"

using ECS_SystemMessageFilter =
ECS_PostOffice::ECS_MessageFilter<
	ComponentAdded, 
	ComponentRemoved, 
	EntityAdded, 
	EntityRemoved>;


class Movable : public ECS_System<entity_t>, public ECS_Mailbox<ECS_SystemMessageFilter>
{
public:
	class Dispatcher
	{
	public:
		Dispatcher( float _dt )
			:
			dt( _dt )
		{
		}
		void operator()( Player& _entity )
		{
			Position& pos = _entity.get_component<Position>().value().get();
			const Velocity& vel = _entity.get_component<Velocity>().value().get();
			pos += ( vel * dt );
		}
		void operator()( Enemy& _entity )
		{
			Position& pos = _entity.get_component<Position>().value().get();
			const Velocity& vel = _entity.get_component<Velocity>().value().get();
			pos += ( vel * dt );
		}
	private:
		float dt;
	};
private:

	class MessageHandler
	{
	public:
		MessageHandler( Movable& _parent, float _dt )
			:
			parent( _parent ),
			dt( _dt )
		{
		}
		void operator()( ComponentAdded _message )
		{	
			if( auto it = parent.find_entity( _message.entity );
				it != parent.entities.end() )
			{	
				if( std::visit( VerifyComponents<Position, Velocity>(), **it ) )
				{
					parent.add_entity( std::move( _message.entity ) );
				}
			}
		}
		void operator()( ComponentRemoved _message )
		{
			if( auto it = parent.find_entity( _message.entity );
				it != parent.entities.end() )
			{
				if( !std::visit( VerifyComponents<Position, Velocity>(), **it ) )
				{
					parent.entities.erase( it );
				}
			}
		}
		void operator()( EntityAdded _message )
		{
			if( auto it = parent.find_entity( _message.entity ); 
				it == parent.entities.end() )
			{
				if( std::visit( VerifyComponents<Position, Velocity>(), **it ) )
				{
					parent.add_entity( std::move( _message.entity ) );
				}
			}
		}
		void operator()( EntityRemoved _message )
		{
			if( auto it = parent.find_entity( _message.entity );
				it != parent.entities.end() )
			{
				parent.remove_entity( _message.entity );
			}
		}
		// Unhandled
		template<typename MessageType>void operator()( MessageType ) {}

	private:
		Movable& parent;
		float dt;
	};
private:
	void process_messages( float _dt )
	{	
		MessageHandler handler( *this, _dt );
		for( auto& vmessage : receiver->get_messages())
		{
			std::visit( handler, vmessage );
		}
		receiver->clear_messages();
	}
};

class Drawable : public ECS_System<entity_t>, public ECS_Mailbox<ECS_SystemMessageFilter>
{
public:
	class Dispatcher
	{
	public:
		Dispatcher( Graphics& _graphics )
			:
			gfx( _graphics )
		{
		}
		void operator()( Player& _entity )
		{
			const Position& pos = _entity.get_component<Position>().value().get();
			
			const Shape& shape = _entity.get_component<Shape>().value().get();

			auto fill_shape = [ & ]( const auto& _shape )
			{
				const auto shape_trans = Translator{}( pos.value, _shape );
				gfx.FillShape( shape_trans, shape.color );
			};

			std::visit( fill_shape, shape.object );
		}

	private:
		Graphics& gfx;
	};
private:
	class MessageHandler
	{
	public:
		MessageHandler( Drawable& _parent )
			:
			parent( _parent )
		{}
		void operator()( ComponentAdded _message )
		{
			if( auto it = parent.find_entity( _message.entity );
				it != parent.entities.end() )
			{
				if( std::visit( VerifyComponents<Position, Shape>(), **it ) )
				{
					parent.add_entity( std::move( _message.entity ) );
				}
			}
		}
		void operator()( ComponentRemoved _message )
		{
			if( auto it = parent.find_entity( _message.entity );
				it != parent.entities.end() )
			{
				if( !std::visit( VerifyComponents<Position, Shape>(), **it ) )
				{
					parent.entities.erase( it );
				}
			}
		}
		void operator()( EntityAdded _message )
		{
			if( auto it = parent.find_entity( _message.entity );
				it == parent.entities.end() )
			{
				if( std::visit( VerifyComponents<Position, Shape>(), **it ) )
				{
					parent.add_entity( std::move( _message.entity ) );
				}
			}
		}
		void operator()( EntityRemoved _message )
		{
			if( auto it = parent.find_entity( _message.entity );
				it != parent.entities.end() )
			{
				parent.remove_entity( _message.entity );
			}
		}
		// Unhandled
		template<typename MessageType>void operator()( MessageType ) {}

	private:
		Drawable& parent;
	};

private:
	void process_messages( Graphics& _graphics )
	{
		MessageHandler handler( *this );
		for( auto& vmessage : receiver->get_messages() )
		{
			std::visit( handler, vmessage );
		}
		receiver->clear_messages();
	}
};

