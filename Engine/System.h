#pragma once

#include "Component.h"
#include "Entity.h"
#include "Graphics.h"
#include "variant_helper.h"
#include <algorithm>
#include <tuple>


template<typename...Required>
class SystemBaseTemplate
{
	template<size_t Idx>
	using required_type = std::tuple_element_t<Idx, std::tuple<Required...>>;


	static constexpr auto requirement_count = std::tuple_size_v<std::tuple<Required...>>;
public:
	void AddEntity( Entity& _entity )
	{
		if( !CheckRequirements<0, required_type<0>>( &_entity ) )
			throw std::runtime_error( "Entity doesn't meet requirements for this system" );

		entities.push_back( &_entity );
	}
	void RemoveEntity( const Entity& _entity )noexcept
	{
		auto swap_and_pop = [ this ]( Entity& e0, Entity& e1 )
		{
			std::swap( e0, e1 );
			entities.pop_back();
		};

		if( entities.empty() ) { return; }

		auto findit =
			std::find_if( entities.begin(), entities.end(),
				[ & ]( const Entity* e ) { return &_entity == e; } );

		if( findit != entities.end() )
		{
			if( entities.size() == 1 )
				entities.clear();
			else
				swap_and_pop( **findit, *entities.back() );
		}
	}

protected:
	std::vector<Entity*> entities;

private:
	template<size_t Idx, typename T>
	bool CheckRequirements( Entity* _entity )
	{
		constexpr auto next = Idx + 1;
		
		bool hasRequired = _entity->HasComponent<T>();
		if constexpr( next < requirement_count )
		{
			hasRequired &=
				CheckRequirements<next, required_type<next>>( _entity );
		}

		return hasRequired;
	}
};

class MoveSystem : public SystemBaseTemplate<Position,Velocity>
{
public:
	void Move( float _delta_time )
	{
		for( auto& e : entities )
		{
			auto& pos = e->GetComponent<Position>();
			const auto& vel = e->GetComponent<Velocity>();
			pos.value += ( vel.value * _delta_time );
		}
	}
};

class DamageSystem : public SystemBaseTemplate<Damage, Health>
{
public:
private:
};
class HealthSystem : public SystemBaseTemplate<Damage, Health>
{
public:
private:
};
class DrawSystem : public SystemBaseTemplate<Position, Shape>
{
public:
	void Draw( Graphics& _graphics )noexcept
	{
		for( const auto& e : entities )
		{
			const auto& pos = e->GetComponent<Position>();
			const auto& shape= e->GetComponent<Shape>();
			

			visit_unary( shape.object, [ & ]( const auto& obj )noexcept
				{	
					_graphics.FillShape( Translator()( pos.value, obj ), shape.color );
				} );
		}
	}
};