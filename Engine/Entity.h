#pragma once

#include "Component.h"
#include "ECS_Utilities.h"
#include "Message.h"
#include "Receiver.h"
#include "Sender.h"
#include <cassert>
#include <stdexcept>
#include <vector>
#include <variant>


class EntityBase 
{
public:
	using iterator = shared_pool<component_t>::iterator;
	using const_iterator = shared_pool<component_t>::const_iterator;

public:
	EntityBase()noexcept
	{
		components.reserve( 10 );
		components.push_back( std::monostate() );
	}
	iterator begin()noexcept { return components.begin(); }
	iterator end()noexcept { return components.end()-1; }
	const_iterator begin()const noexcept { return components.begin(); }
	const_iterator end()const noexcept { return components.end()-1; }

	size_t count()const { return components.size() - 1; }

	template<typename ComponentType> ComponentType& get_component()
	{
		auto findit = find_component<ComponentType>();

		return std::get<ComponentType>( *findit );
	}
	template<typename ComponentType>const ComponentType& get_component()const
	{
		auto findit = find_component<ComponentType>();

		return std::get<ComponentType>( *findit );
	}

	template<typename ComponentType>
	bool has_component()const noexcept
	{
		return find_component<ComponentType>() != end();
	}

	template<typename ComponentType, typename...Rest>
	bool has_all()const
	{
		bool hasAll = has_component<ComponentType>();
		if( hasAll )
		{
			if constexpr( sizeof...( Rest ) > 0 )
			{
				hasAll &= has_all<Rest...>();
			}
		}

		return hasAll;
	}
protected:
	template<typename ComponentType, typename Source>
	ComponentType& add_component( Source* _this )
	{
		const auto last = count();
		*components[ last ] = ComponentType();
		components.push_back( std::monostate() );

		send<ComponentAdded>( _this );

		return std::get<ComponentType>( *components[ last ] );
	}

	template<typename Source, typename Components, typename...Rest>
	void add_components( Source* _this, Components _component, Rest&&... _rest )
	{
		auto& component = components.back();
		components.push_back( std::monostate() );
		*component = std::move( _component );
		if constexpr( sizeof...( Rest ) > 0 )
		{
			add_components(_this, std::forward<Rest>( _rest )... );
		}
	}

	template<typename ComponentType, typename Source>
	void remove_component( Source* _this )
	{
		send<ComponentRemoved>( _this );
		auto holds_component = [ & ]( auto& _component )
		{
			return  std::holds_alternative<ComponentType>( _component );
		};
		swap_and_pop_if( components, holds_component );
	}
	template<typename ComponentType>
	iterator find_component()noexcept
	{
		return std::find_if(
			begin(),
			end(),
			[ & ]( auto& _component ) 
			{ 
				return std::holds_alternative<ComponentType>( _component ); 
			}
		);
	}
	template<typename ComponentType>
	const_iterator find_component()const noexcept
	{
		return std::find_if(
			begin(),
			end(),
			[ & ]( auto& _component )
			{
				return std::holds_alternative<ComponentType>( _component );
			}
		);
	}
protected:
	shared_pool<component_t> components;
};


class Entity : public EntityBase
{
public:
	Entity() = default;

	template<typename ComponentType>
	ComponentType& add_component()
	{
		return EntityBase::add_component<ComponentType>( this );
	}

	template<typename Components, typename...Rest>
	void add_components( Components&& _component, Rest&&... _rest )
	{
		EntityBase::add_components( 
			this, std::forward<Components>( _component ), std::forward<Rest>( _rest )... );
	}

	template<typename ComponentType> void remove_component()
	{
		EntityBase::remove_component<ComponentType>( this );
	}

};

class EntityManager
{
public:
	shared_resource<Entity> create_entity()
	{
		const size_t index = entities.size();
		entities.push_back( Entity() );
		return entities[ index ];
	}
	void remove_entity( const shared_resource<Entity>& _entity )
	{
		if( auto findit = find_if( entities,
			[ & ]( const Entity& _e )
			{
				return &_e == _entity.get();
			} ); findit != entities.end() )
		{
			entities.erase( findit );
		}
	}
private:
	shared_pool<Entity> entities;
};