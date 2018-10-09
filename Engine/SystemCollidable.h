#pragma once

#include "Includes.h"
#include "Component.h"
#include "Shapes.h"
#include "SystemMessageDispatchBase.h"

class CollidableExecuteDispatch
{
	struct collision_data 
	{
		collision_data() = default;
		collision_data( const Velocity& _v1, const Position& _p1, 
			const Velocity& _v2, const Position& _p2, const Vec2& _inc )
			:
			v1( _v1 ), p1( _p1 ),
			v2( _v2 ), p2( _p2 ),
			incident( _inc )
		{

		}

		Velocity v1, v2;
		Position p1, p2;
		Vec2 incident;
	};

public:
	CollidableExecuteDispatch( Collidable& _parent );
	void operator()( Player& _entity );
	void operator()( Enemy& _entity );
	void operator()( Wall& _entity );

	// Unhandled
	template<typename Type>
	void operator()( Type& _entity ) {}

private:
	bool is_colliding( const Rect& _left, const Rect& _right )noexcept;
	bool is_colliding( const Rect& _left, const Circle& _right )noexcept;
	bool is_colliding( const Circle& _left, const Rect& _right )noexcept;
	bool is_colliding( const Circle& _left, const Circle& _right )noexcept;


	collision_data solve( const Rect& _left, const Rect& _right, const collision_data& _data )const noexcept;
	collision_data solve( const Rect& _left, const Circle& _right, const collision_data& _data )const noexcept;
	collision_data solve( const Circle& _left, const Rect& _right, const collision_data& _data )const noexcept;
	collision_data solve( const Circle& _left, const Circle& _right, const collision_data& _data )const noexcept;
private:
	Collidable& parent;
	bool isHandled = false;
	
};

class CollidableMessageDispatch : public SystemMessageDispatchBase<Collidable, Position, Velocity, Shape>
{
public:
	CollidableMessageDispatch( Collidable& _parent );
	/*void operator()( const ComponentAdded& _message );
	void operator()( const ComponentRemoved& _message );
	void operator()( const EntityAdded& _message );
	void operator()( const EntityRemoved& _message );*/

	// Unhandled
	template<typename T>
	void operator()( const T& ) {}
private:
	//Collidable& parent;
};