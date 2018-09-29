#pragma once

#include "Component.h"
#include "Entity.h"
#include "Graphics.h"
#include "Receiver.h"
#include "Sender.h"
#include <memory>

class System : public Sender, public Receiver
{
public:
	using entity_resource_vector = std::vector<shared_resource<Entity>>;

public:
	System( EntityManager& _entity_manager )
		:
		entity_manager( &_entity_manager )
	{
	}

	template<typename...Components>
	shared_resource<Entity> create_entity( Components&&... _components)
	{
		auto entity = entity_manager->create_entity();
		entity->add_components( std::forward<Components>( _components )... );
		return entity;
	}

	void add_entity( shared_resource<Entity> _entity )
	{
		auto& entity = entities.emplace_back( std::move( _entity ) );
	}

	void remove_entity( const shared_resource<Entity>& _entity )
	{
		erase_if( entities, [ & ]( const shared_resource<Entity>& _ent )
			{
				return _entity == _ent;
			} );
	}

	entity_resource_vector::iterator find_entity( const shared_resource<Entity>& _entity )
	{
		auto findit = std::find_if(
			entities.begin(),
			entities.end(),
			[ & ]( const shared_resource<Entity>& _ent )
			{
				return _entity == _ent;
			} );

		return findit;
	}
protected:
	template<typename MessageType, typename Pred, typename...Args>
	void process_messages( message_t& _vobject, Pred&& _execute, Args&&... _args )
	{
		auto handle_message = [ & ]( auto& _object )
		{
			using InnerType = std::decay_t<decltype( _object )>;

			if constexpr( std::is_same_v<Type, InnerType> )
			{
				_execute( std::forward<Args>( _args )... );
			}
		};

		std::visit( handle_message, _vobject );
	}

	template<typename MessageType, typename Pred, typename...Args>
	void process_message( const message_t& _vmessage, Pred&& _execute, Args&&... _args )
	{
		auto handle_message = [ & ]( const auto& _object )
		{
			using InnerType = std::decay_t<decltype( _object )>;

			if constexpr( std::is_same_v<MessageType, InnerType> )
			{
				_execute( _object, std::forward<Args>( _args )... );
			}
		};

		std::visit( handle_message, _vmessage );
	}
	
	void remove_entity( entity_resource_vector::iterator _entity_iter )
	{
		entities.erase( _entity_iter );
	}

protected:
	entity_resource_vector entities;
	EntityManager* entity_manager = nullptr;

private:
	friend class World;
};

class Movable : public System
{
public:
	using System::System;

	void execute( float _dt )
	{
		process_messages( _dt );
	}
	static shared_resource<Entity> create_compatible_entity()
	{
		shared_resource<Entity> entity;
		entity->add_components( Position( 0.f, 0.f ), Velocity( 0.f, 0.f ) );
		return entity;
	}
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
		void operator()( std::monostate _message ) {}
		void operator()( ComponentAdded _message ) {}
		void operator()( ComponentRemoved _message ){}
		void operator()( EntityAdded _message ){}
		void operator()( EntityRemoved _message ){}
		void operator()( SystemAdded _message ) {}
		void operator()( SystemRemoved _message ) {}

	private:
		Movable& parent;
		float dt;
	};
private:
	void process_messages( float _dt )
	{	
		MessageHandler handler( *this, _dt );
		for( auto& vmessage : messages )
		{
			std::visit( handler, vmessage );
		}
		messages.clear();
	}
	void Update( shared_resource<Entity> _entity, const float _dt )
	{
		_entity->get_component<Position>().value +=
			( _entity->get_component<Velocity>().value * _dt );
	}
};

class Drawable : public System
{
public:
	using System::System;

	void execute( Graphics& _graphics )
	{
		process_messages( _graphics );
		Draw( _graphics );
	}
	template<typename ShapeType>
	static shared_resource<Entity> create_compatible_entity()
	{
		auto get_shape = [ & ]()
		{
			if constexpr( std::is_same_v<ShapeType, Rect> )
			{	
				return Rect{ 0.f,0.f,0.f,0.f };
			}
			else 
			{	
				return Circle{ Vec2( 0.f,0.f ),10.f };
			}
		};

		shared_resource<Entity> entity;
		entity->add_components( Position( 0.f, 0.f ), Shape( get_shape(), Colors::Red ) );
		return entity;
	}

private:
	class MessageHandler
	{
	public:
		MessageHandler( Drawable& _parent, Graphics& _graphics )
			:
			parent( _parent ),
			graphics( _graphics )
		{}
		void operator()( std::monostate _message ) {}
		void operator()( ComponentAdded _message ) 
		{
			if( auto findit = parent.find_entity( _message.entity );
				findit == parent.entities.end() )
			{
				if( _message.entity->has_all<Position, Shape>() )
				{
					parent.add_entity( std::move( _message.entity ) );
				}
			}
		}
		void operator()( ComponentRemoved _message ) 
		{
			if( auto findit = parent.find_entity( _message.entity );
				findit != parent.entities.end() )
			{
				if( !_message.entity->has_all<Position, Shape>() )
				{
					parent.remove_entity( findit );
				}
			}
		}
		void operator()( EntityAdded _message ) 
		{
			if( _message.entity->has_all<Position, Shape>() )
			{
				parent.add_entity( std::move( _message.entity ) );
			}
		}
		void operator()( EntityRemoved _message )
		{	
			if( auto findit = parent.find_entity( _message.entity ); 
				findit != parent.entities.end() )
			{
				if( !_message.entity->has_all<Position, Shape>() )
				{
					parent.remove_entity( findit );
				}
			}
		}
		void operator()( SystemAdded _message ) {}
		void operator()( SystemRemoved _message ) {}

	private:
		Drawable& parent;
		Graphics& graphics;
	};

private:
	void process_messages( Graphics& _graphics )
	{
		MessageHandler handler( *this, _graphics );
		for( auto& vmessage : messages )
		{
			std::visit( handler, vmessage );
		}
		messages.clear();
	}
	void Draw( Graphics& _graphics )const
	{
		Translator translator;
		for( const auto& entity : entities )
		{
			const Shape& shape = entity->get_component<Shape>();

			std::visit( [ & ]( const auto& _shape )
				{
					const auto shape_trans =
						translator(
							entity->get_component<Position>().value, _shape
						);

					_graphics.FillShape( shape_trans, shape.color );
				}, shape.object );
		}
	}
};

using system_t = std::variant<
	Movable,
	Drawable
>;

