#pragma once

#include "Component.h"
#include "ECS_PostOffice.h"
#include "Entity.h"
#include "Graphics.h"
#include "Receiver.h"
#include "Sender.h"
#include <memory>
#include <optional>

class System :public ECS_Mailbox
{
public:
	using entity_resource_vector = std::vector<shared_resource<Entity>>;
	using entity_resource = shared_resource<Entity>;
public:

	template<typename...Components>
	entity_resource create_entity( Components&&... _components)
	{
		auto entity = entity_manager->create_entity();
		entity->add_components( std::forward<Components>( _components )... );
		return entity;
	}

	void add_entity( entity_resource _entity )
	{
		auto& entity = entities.emplace_back( std::move( _entity ) );
	}

	void remove_entity( const entity_resource& _entity )
	{
		erase_if( entities, [ & ]( const shared_resource<Entity>& _ent )
			{
				return _entity == _ent;
			} );
	}

	entity_resource_vector::iterator find_entity( const entity_resource& _entity )
	{
		auto findit = find_if( entities,
			[ & ]( const shared_resource<Entity>& _ent )
			{
				return _entity == _ent;
			} );

		return findit;
	}
	entity_resource_vector::const_iterator find_entity( const entity_resource& _entity )const
	{
		auto findit = find_if( entities,
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

	void remove_entity( entity_resource_vector::const_iterator _entity_iter )
	{
		entities.erase( _entity_iter );
	}

	template<typename...Components>
	auto is_compatible( const entity_resource& _entity )const->
		std::optional<std::pair<entity_resource_vector::const_iterator, bool>>
	{
		if( auto findit = find_entity( _entity ); findit != entities.end() )
		{
			if( _entity->has_all<Components>() )
			{
				return { {findit,true } };
			}
			else
			{
				return { {findit,false} };
			}
		}

		return { std::nullopt };
	}

protected:
	entity_resource_vector entities;
};

class Movable : public System
{
public:
	using System::System;

	void execute( float _dt )
	{
		process_messages( _dt );
	}
private:
	class MessageFilter
	{
		bool operator()( ComponentAdded ) { return true; }
		bool operator()( ComponentRemoved ) { return true; }
		bool operator()( EntityAdded ) { return true; }
		bool operator()( EntityRemoved ) { return true; }
		template<typename MessageType>
		bool operator()( MessageType ) { return false; }
	};
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
			if( auto result = parent.is_compatible<Position, Velocity>( _message.entity );
				result.has_value() )
			{
				if( auto[ findit, compatible ] = result.value(); compatible )
				{
					parent.add_entity( std::move( _message.entity ) );
				}
			}
		}
		void operator()( ComponentRemoved _message )
		{
			if( auto result = parent.is_compatible( _message.entity ); result.has_value() )
			{	
				if( auto[ findit, compatible ] = result.value(); !compatible )
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
				parent.remove_entity( findit );
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

private:
	class MessageHandler
	{
	public:
		MessageHandler( Drawable& _parent )
			:
			parent( _parent )
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
	};

private:
	void process_messages( Graphics& _graphics )
	{
		MessageHandler handler( *this, _graphics );
		for( auto& vmessage : receiver->get_messages() )
		{
			std::visit( handler, vmessage );
		}
		receiver->clear_messages();
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

