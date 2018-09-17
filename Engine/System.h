#pragma once

#include "Component.h"
#include "Entity.h"
#include "Graphics.h"
#include "variant_helper.h"
#include <algorithm>
#include <tuple>

template<typename T> T& identify( const T& t ) { return t; }

template<typename EntityType, typename...Required>
class SystemBaseTemplate
{
	template<size_t Idx>
	using required_type = std::tuple_element_t<Idx, std::tuple<Required...>>;

	static constexpr auto requirement_count = std::tuple_size_v<std::tuple<Required...>>;
public:
	void AddEntity( EntityType& _entity )
	{
		if( !HasRequirements<0, required_type<0>>( &_entity ) )
			throw std::runtime_error( "Entity doesn't meet requirements for this system" );

		entities.push_back( &_entity );
	}
	
	void RemoveEntity( const EntityType& _entity )noexcept
	{
		auto iter = entities.FindVariant( _entity );
		entities.SwapAndPop( iter );
	}

protected:
	Collection<EntityType*> entities;

private:
	template<size_t Idx, typename ComponentT, typename EntityT>
	bool HasRequirements( EntityT& _entity )const noexcept
	{	
		constexpr auto next = Idx + 1;
		
		bool hasRequired = _entity.HasComponent<ComponentT>();
		if constexpr( next < requirement_count )
		{
			hasRequired &=
				HasRequirements<next, required_type<next>>( _entity );
		}

		return hasRequired;
	}
};

class MoveSystem : public SystemBaseTemplate<EntityMask, Position, Velocity>
{
public:
	void Move( float _delta_time )
	{
		for( auto& e : entities )
		{
			auto entity = e.Extract_Ptr_To<EntityMask>();

			auto& pos = entity->GetComponent<Position>();
			const auto& vel = entity->GetComponent<Velocity>();
			pos.value += ( vel.value * _delta_time );
		}
	}
};

class CollisionSystem : public SystemBaseTemplate<EntityMask, Position, Shape>
{
public:

	template<typename WorldType>
	void Check( WorldType& _world)
	{
		for( int j = 0; j < int( entities.size() ); ++j )
		{
			auto* ent1 = entities[ j ];
			for( int i = j + 1; i < int( entities.size() ); ++i )
			{
				auto* ent2 = entities[ i ];
				if( _world.HasSystem<DamageSystem>() )
				{
					const auto& dmg = _world.GetSystem<DamageSystem>();
					
				}
			}
		}
	}
	bool IsColliding( const Rect& _lRect, const Rect& _rRect )
	{
		return
			_lRect.left <= _rRect.right && _lRect.right >= _rRect.left &&
			_lRect.top <= _rRect.bottom && _lRect.bottom >= _rRect.top;
	}
	bool IsColliding( const Rect& _rect, const Circle& _circle )
	{
		return
			( _rect.left - _circle.center.x ) < _circle.radius ||
			( _rect.top - _circle.center.y ) < _circle.radius ||
			( _rect.right - _circle.center.x ) < _circle.radius ||
			( _rect.bottom - _circle.center.y ) < _circle.radius;
	}
	bool IsColliding( const Circle& _circle, const Rect& _rect )
	{
		return IsColliding( _rect, _circle );
	}
	bool IsColliding( const Circle& _lCircle, const Circle& _rCircle )
	{
		auto sq = []( float val ) { return val * val; };
		const auto dx = sq( _lCircle.center.x - _rCircle.center.x );
		const auto dy = sq( _lCircle.center.y - _rCircle.center.y );
		const auto combRad = sq( _lCircle.radius ) + sq( _rCircle.radius );
		return ( dx + dy ) <= combRad;
	}

private:
};

class DamageSystem : public SystemBaseTemplate<EntityMask, Damage, Health>
{
public:
private:
};

class HealthSystem : public SystemBaseTemplate<EntityMask, Damage, Health>
{
public:

private:
};

class DrawSystem : public SystemBaseTemplate<EntityMask, Position, Shape>
{
public:
	void Draw( Graphics& _graphics )noexcept
	{
		for( const auto& e : entities )
		{
			const auto* entity = e.Extract_Ptr_To<EntityMask>();
			const auto& pos = entity->GetComponent<Position>();
			const auto& shape= entity->GetComponent<Shape>();
			

			/*visit_unary( shape.object, [ & ]( const auto& obj )noexcept
				{	
					_graphics.FillShape( Translator()( pos.value, obj ), shape.color );
				} );*/
		}
	}
};