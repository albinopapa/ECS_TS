#include "SystemCollidable.h"
#include "Message.h"
#include "ChiliMath.h"

CollidableExecuteDispatch::CollidableExecuteDispatch( Collidable& _parent )
	:
	parent( _parent )
{
}

void CollidableExecuteDispatch::operator()( Player& _entity )
{
	auto shape_extractor = [ & ]( auto&& entity )
	{
		auto collision_visitor = [ & ]( auto&& _object1, auto&& _object2 )
		{
			auto v1 = _entity.get_component<Velocity>();
			auto v2 = entity.get_component<Velocity>();
			auto p1 = _entity.get_component<Position>();
			auto p2 = entity.get_component<Position>();

			if( is_colliding( _object1, _object2 ) )
			{
				const collision_data data_in = [ & ]
				{
					if( v1.has_value() )
					{
						if( v2.has_value() )
						{
							return collision_data(
								v1.value().get(),
								p1.value().get(),
								v2.value().get(),
								p2.value().get(),
								Vec2( 0.f, 0.f )
							);
						}
						else
						{
							return collision_data(
								v1.value().get(),
								p1.value().get(),
								Velocity( 0.f, 0.f ),
								p2.value().get(),
								Vec2( 0.f, 0.f )
							);
						}
					}
					else
					{
						return collision_data(
							Velocity( 0.f, 0.f ),
							p1.value().get(),
							Velocity( 0.f, 0.f ),
							p2.value().get(),
							Vec2( 0.f, 0.f ) );
					}
				}( );

				const collision_data data_out = solve( _object1, _object2, data_in );
				if( v1.has_value() )
				{
					v1.value().get() = data_out.v1;
				}
				if( v2.has_value() )
				{
					v2.value().get() = data_out.v2;
				}
				p1.value().get() = data_out.p1;
				p2.value().get() = data_out.p2;
			}
		};
		using type = std::decay_t<decltype( entity )>;

		// If entity is also Player, then skip collision
		// If T is a wall, then skip collision, will be checked by
		// Player or Enemy type
		if constexpr( std::is_same_v<Player, type> ) return;

		const Shape& entity_shape = ( *_entity.get_component<Shape>() ).get();
		const Shape& other_shape = ( *entity.get_component<Shape>() ).get();

		std::visit( collision_visitor, entity_shape.object, other_shape.object );
	};

	for( auto& ventity : parent.entities )
	{
		std::visit( shape_extractor, *ventity );
	}
}
void CollidableExecuteDispatch::operator()( Enemy& _entity )
{
	auto shape_extractor = [ & ]( auto&& entity )
	{
		auto collision_visitor = [ & ]( auto&& _object1, auto&& _object2 )
		{
			auto v1 = _entity.get_component<Velocity>();
			auto v2 = entity.get_component<Velocity>();
			auto p1 = _entity.get_component<Position>();
			auto p2 = entity.get_component<Position>();

			if( is_colliding( _object1, _object2 ) )
			{
				const collision_data data_in = [ & ]
				{
					if( v1.has_value() )
					{
						if( v2.has_value() )
						{
							return collision_data(
								v1.value().get(),
								p1.value().get(),
								v2.value().get(),
								p2.value().get(),
								Vec2( 0.f, 0.f )
							);
						}
						else
						{
							return collision_data(
								v1.value().get(),
								p1.value().get(),
								Velocity( 0.f, 0.f ),
								p2.value().get(),
								Vec2( 0.f, 0.f )
							);
						}
					}
					else
					{
						return collision_data( 
							Velocity( 0.f, 0.f ), 
							p1.value().get(), 
							Velocity( 0.f, 0.f ), 
							p2.value().get(), 
							Vec2( 0.f, 0.f ) );
					}
				}( );

				const collision_data data_out = solve( _object1, _object2, data_in );
				if( v1.has_value() )
				{
					v1.value().get() = data_out.v1;
				}
				if( v2.has_value() )
				{
					v2.value().get() = data_out.v2;
				}
				p1.value().get() = data_out.p1;
				p2.value().get() = data_out.p2;
			}
		};
		using type = std::decay_t<decltype( entity )>;

		// If entity is also Player, then skip collision
		// If T is a wall, then skip collision, will be checked by
		// Player or Enemy type
		if constexpr( std::is_same_v<Player, type> ) return;

		const Shape& entity_shape = ( *_entity.get_component<Shape>() ).get();
		const Shape& other_shape = ( *entity.get_component<Shape>() ).get();

		std::visit( collision_visitor, entity_shape.object, other_shape.object );
	};

	for( auto& ventity : parent.entities )
	{
		std::visit( shape_extractor, *ventity );
	}
}
void CollidableExecuteDispatch::operator()( Wall& _entity ) {}

bool CollidableExecuteDispatch::is_colliding( const Rect& _left, const Rect& _right ) noexcept
{
	return 
		_left.right >= _right.left && _left.left <= _right.right &&
		_left.bottom >= _right.top && _left.top <= _right.bottom;
}
bool CollidableExecuteDispatch::is_colliding( const Rect& _left, const Circle& _right ) noexcept
{
	const auto width = _left.right - _left.left;
	const auto height = _left.bottom - _left.top;
	const auto dx = _right.center.x - _left.left;
	const auto dy = _right.center.y - _left.top;

	return 
		dx < _right.radius || dx + width < _right.radius ||
		dy < _right.radius || dy + height < _right.radius;
}
bool CollidableExecuteDispatch::is_colliding( const Circle& _left, const Rect& _right ) noexcept
{
	return is_colliding( _right, _left );
}
bool CollidableExecuteDispatch::is_colliding( const Circle& _left, const Circle& _right ) noexcept
{
	return ( _right.center - _left.center ).LenSq() < 
		( chili::sq( _left.radius ) + chili::sq( _right.radius ) );
}

CollidableExecuteDispatch::collision_data CollidableExecuteDispatch::solve( const Rect& _left, const Rect& _right, const collision_data& _data )const noexcept
{
	const auto penatration = _data.p2.value - _data.p1.value;
	return [ & ]
	{
		if( std::fabsf( _data.v1.value.y * penatration.x ) >
			std::fabsf( _data.v1.value.x * penatration.y ) )
		{
			const auto halfDist = ( _left.bottom - _right.top ) * .5f;

			// Top/Bottom collision
			if( _data.v1.value.y > 0.f )
			{
				// Handle top collision
				return collision_data(
					{ _data.v1.value.x, 0.f },
					{ _data.p1.value.x, _data.p1.value.y - halfDist },
					{ _data.v2.value.x, 0.f },
					{ _data.p2.value.x, _data.p2.value.y + halfDist },
					Vec2( 0.f, 0.f ) );
			}
			else
			{
				// Handle bottom collision
				return collision_data(
					{ _data.v1.value.x, 0.f },
					{ _data.p1.value.x, _data.p1.value.y + halfDist },
					{ _data.v2.value.x, 0.f },
					{ _data.p2.value.x, _data.p2.value.y - halfDist },
					Vec2( 0.f, 0.f ) );
			}
		}
		else
		{
			const auto halfDist = ( _left.right - _right.left ) * .5f;
			// Left/Right collision
			if( _data.v1.value.x > 0.f )
			{
				return collision_data(
					{ 0.f, _data.v1.value.y },
					{ _data.p1.value.x + halfDist, _data.p1.value.y },
					{ 0.f, _data.v2.value.y },
					{ _data.p2.value.x - halfDist, _data.p2.value.y },
					Vec2( 0.f, 0.f )
				);
			}
			else
			{
				return collision_data(
					{ 0.f, _data.v1.value.y },
					{ _data.p1.value.x + halfDist, _data.p1.value.y },
					{ 0.f, _data.v2.value.y },
					{ _data.p2.value.x - halfDist, _data.p2.value.y },
					Vec2( 0.f, 0.f )
				);
			}
		}
	}( );
}
CollidableExecuteDispatch::collision_data CollidableExecuteDispatch::solve( const Rect& _left, const Circle& _right, const collision_data& _data )const noexcept
{
	Velocity v1 = _data.v1, v2 = _data.v2;
	Position p1 = _data.p1, p2 = _data.p2;
	Vec2 inc = { 0.f, 0.f };

	const auto c1 = Vec2(
		_left.left + ( _left.right - _left.left ) *.5f,
		_left.top + ( _left.bottom - _left.top )*.5f );

	// Angle from _left -> _right
	const auto angle = ( _right.center - c1 );
	// Angle from _right -> _left
	const auto rangle = -angle;


	// Left/Right collision
	if( std::fabsf( angle.x ) > std::fabsf( angle.y ) )
	{
		v1.value.x = 0.f;
		v2.value.x *= -1.f;

		const auto offset = ( _left.left - _right.center.x - _right.radius );
		if( rangle.x > 0.f)			// Hit left of rect
		{
			p1.value.x = _left.left + offset;
			p2.value.x = _right.center.x - offset;
		}
		else if( rangle.x < 0.f )	// Hit right of rect
		{
			p1.value.x = _left.left - offset;
			p2.value.x = _right.center.x + offset;
		}
	}
	else // Top/Bottom Collision
	{
		v2.value.y = 0.f;
		v2.value.y *= -1.f;

		const auto offset = ( _left.top - _right.center.y - _right.radius );
		if( rangle.y > 0.f )
		{
			p1.value.y = _left.top + offset;
			p2.value.y = _right.center.y - offset;
		}
		else
		{
			p1.value.y = _left.top - offset;
			p2.value.y = _right.center.y + offset;
		}
	}

	return collision_data( v1, p1, v2, p2, inc );
}
CollidableExecuteDispatch::collision_data CollidableExecuteDispatch::solve( const Circle& _left, const Rect& _right, const collision_data& _data )const noexcept
{
	const auto result = solve( _right, _left, _data );
	return collision_data(
		result.v2,
		result.p2,
		result.v1,
		result.p1,
		result.incident
	);
}
CollidableExecuteDispatch::collision_data CollidableExecuteDispatch::solve( const Circle& _left, const Circle& _right, const collision_data& _data )const noexcept
{
	const auto delta = ( _right.center - _left.center );
	// Angle from _left -> _right
	const auto angle = delta.Normalize();
	const auto dist = dot_product( delta, angle );
	const auto l_offset = -( angle * dist * .5f );
	const auto r_offset = -l_offset;
	Velocity v1, v2;
	v1.value = reflect( _data.v1.value, angle );
	v2.value = -v1.value;
	Position p1, p2;
	p1.value = _data.p1.value + l_offset;
	p2.value = _data.p2.value + r_offset;

	return collision_data( v1, p1, v2, p2, { 0.f, 0.f } );
}

CollidableMessageDispatch::CollidableMessageDispatch( Collidable& _parent )
	:
	SystemMessageDispatchBase<Collidable, Position, Velocity, Shape>( _parent )
{
}
//void CollidableMessageDispatch::operator()( const ComponentAdded& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.add_entity( std::move( _message.entity ) );
//		}
//	}
//}
//void CollidableMessageDispatch::operator()( const ComponentRemoved& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		if( !std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.remove_entity( _message.entity );
//		}
//	}
//}
//void CollidableMessageDispatch::operator()( const EntityAdded& _message )
//{
//	if( !parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.add_entity( std::move( _message.entity ) );
//		}
//	}
//}
//void CollidableMessageDispatch::operator()( const EntityRemoved& _message )
//{
//	if( parent.has_entity( _message.entity ) )
//	{
//		if( std::visit( VerifyComponents<Position, Shape>(), *_message.entity ) )
//		{
//			parent.remove_entity( _message.entity );
//		}
//	}
//}
