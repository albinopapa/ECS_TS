#pragma once

#include "Component.h"
#include "ECS_Utilities.h"
#include "ECS_PostOffice.h"
#include "ECS_Entity.h"
#include "ECS_Factory.h"
#include "Includes.h"
#include <cassert>
#include <stdexcept>
#include <vector>
#include <variant>


//class Entity : public ECS_Mailbox<ECS_PostOffice::ECS_NullMessageFilter>
//{
//public:
//	using iterator = std::vector<component_t>::iterator;
//	using const_iterator = std::vector<component_t>::const_iterator;
//
//public:
//	Entity()noexcept
//		:
//		ECS_Mailbox<ECS_PostOffice::ECS_NullMessageFilter>(
//			ECS_PostOffice::mailbox_factory<ECS_PostOffice::ECS_NullMessageFilter>() )
//	{
//		components.reserve( 10 );
//		components.push_back( std::monostate() );
//	}
//
//	template<typename ComponentType>
//	ComponentType& add_component()
//	{
//		const auto last = count();
//		*components[ last ] = ComponentType();
//		components.push_back( std::monostate() );
//
//		return std::get<ComponentType>( *components[ last ] );
//	}
//
//	template<typename Components, typename...Rest>
//	void add_components( Components _component, Rest&&... _rest )
//	{
//		auto& component = components.back();
//		components.push_back( std::monostate() );
//		*component = std::move( _component );
//		if constexpr( sizeof...( Rest ) > 0 )
//		{
//			add_components( _this, std::forward<Rest>( _rest )... );
//		}
//	}
//
//	template<typename ComponentType>
//	void remove_component()
//	{
//		auto holds_component = [ & ]( auto& _component )
//		{
//			return  std::holds_alternative<ComponentType>( _component );
//		};
//		swap_and_pop_if( components, holds_component );
//	}
//
//	template<typename ComponentType>
//	iterator find_component()noexcept
//	{
//		return find_if( components,
//			[ & ]( auto& _component )
//			{
//				return std::holds_alternative<ComponentType>( _component );
//			}
//		);
//	}
//
//	template<typename ComponentType>
//	const_iterator find_component()const noexcept
//	{
//		return std::find_if(
//			begin(),
//			end(),
//			[ & ]( auto& _component )
//			{
//				return std::holds_alternative<ComponentType>( _component );
//			}
//		);
//	}
//
//	size_t count()const { return components.size() - 1; }
//
//	template<typename ComponentType>
//	ComponentType& get_component()
//	{
//		return std::get<ComponentType>( *find_component<ComponentType>() );
//	}
//
//	template<typename ComponentType>
//	const ComponentType& get_component()const
//	{
//		return std::get<ComponentType>( *find_component<ComponentType>() );
//	}
//
//	template<typename ComponentType>
//	bool has_component()const noexcept
//	{
//		return find_component<ComponentType>() != end();
//	}
//
//	template<typename ComponentType, typename...Rest>
//	bool has_all_components()const
//	{
//		bool hasAll = has_component<ComponentType>();
//		if( hasAll )
//		{
//			if constexpr( sizeof...( Rest ) > 0 )
//			{
//				hasAll &= has_all_components<Rest...>();
//			}
//		}
//
//		return hasAll;
//	}
//
//private:
//	friend class EntityManager;
//	std::vector<component_t> components;
//};

template<typename...ComponentList>
struct VerifyComponents
{
	template<typename T>
	auto operator()( const T& _entity )->
		std::enable_if_t<!std::is_same_v<T, std::monostate>, bool>
	{
		return _entity.has_all_components<ComponentList...>();
	}
	bool operator()( const std::monostate& _entity )
	{
		return false;
	}
};


class Player : public screws::ECS_Entity<component_t>
{
public:
	Player( Factory& _factory );
};

class Enemy : public screws::ECS_Entity<component_t>
{
public:
	Enemy( Factory& _factory );
};