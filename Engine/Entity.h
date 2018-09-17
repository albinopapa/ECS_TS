#pragma once

#include <cassert>
#include "variant_helper.h"

class Human;
class AI;


using component_t =
	Collection<Damage, Dimension, Health, Orientation, Position, Shape, Shield, Velocity>;


template<typename..._AcceptedComponents>
class Entity
{
	using value_type = Entity<_AcceptedComponents...>;
	using iterator = CollectionIterator<_AcceptedComponents...>;
	using const_iterator = ConstCollectionIterator<_AcceptedComponents...>;
	using reference = Variant<_AcceptedComponents...>&;
	using const_reference = const Variant<_AcceptedComponents...>&;
	using pointer = Variant<_AcceptedComponents...>*;
	using const_pointer = const Variant<_AcceptedComponents...>*;

public:
	template<typename ComponentT, typename...Args> 
	void AddComponent( Args&&... _args )
	{
		static_assert(
			has_type<ComponentT, _AcceptedComponents...>(),
			"The component you are tring to add is not in the accepted\n"
			"list of components for this entity"
			);

		// TODO: Maybe assert if component already added?
		if( !HasComponent<ComponentT>() )
		{
			components.AddVariant<ComponentT>( std::forward<Args>( _args )... );
		}
	}
	template<typename ComponentT>
	void AddComponent( ComponentT&& _component )
	{
		static_assert(
			has_type<ComponentT, _AcceptedComponents...>(),
			"The component you are tring to add is not in the accepted\n"
			"list of components for this entity"
			);
		components.AddVariant( std::forward<ComponentT>( _component ) );
	}
	template<typename ComponentT> void RemoveComponent()
	{
		auto findit = components.FindVariant<ComponentT>();
		components.SwapAndPop( findit );
	}

	template<typename ComponentT> bool HasComponent()const noexcept
	{
		return components.IsValidIterator( components.FindVariant<ComponentT>() )
	}
	template<typename ComponentT> ComponentT& GetComponent()
	{
		static_assert( ( has_type<ComponentT, _AcceptedComponents...>() ),
			"Component isn't in the list of accepted types." );
		assert( ( !components.empty() ) && "Component collection is empty." );

		auto findit = components.FindVariant<ComponentT>();
		if( components.IsValidIterator( findit ) )
		{
			return ( *findit ).Extract_Ref_To<ComponentT>();
		}

		throw std::runtime_error( "Component not found in entity." );
	}
	template<typename ComponentT> const ComponentT& GetComponent()const
	{
		static_assert( ( has_type<ComponentT, _AcceptedComponents...>() ),
			"Component isn't in the list of accepted types." );
		assert( ( !components.empty() ) && "Component collection is empty." );

		auto findit = components.FindVariant<ComponentT>();
		if( components.IsValidIterator( findit ) )
		{
			auto& vref = ( *findit );
			
			return vref.Extract_Ref_To<ComponentT>();
		}

		throw std::runtime_error( "Component not found in entity." );
	}

private:
	Collection<_AcceptedComponents...> components;
};
using EntityMask = 
	Entity<Damage, Dimension, Health, Orientation, Position, Shape, Shield, Velocity>;

class Player :public Entity<Position,Velocity,Health,Shape>
{
public:
	using EntityType = Entity<Position, Velocity, Health, Shape>;
};