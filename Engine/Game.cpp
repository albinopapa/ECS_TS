/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "ChiliMath.h"
#include <cassert>
#include "FrameTimer.h"

template<typename...Variants>
class VarTest
{
public:
	VarTest() = default;
	template<typename T>
	VarTest( T _arg )
		:
		v( _arg )
	{
	}
	/*template<typename RetType, template<typename...> typename Callable, typename...Args>
	auto Visit( Callable<Args...>&& _callback, Args&&..._args )->RetType
	{
		auto NoReturnCall = [ & ]( auto& _object )
		{
			using type = decltype( _object );
			_callback(
				std::forward<type>( _object ),
				std::forward<Args>( _args )... );
		};
		auto ReturnCall = [ & ]( auto& _object )
		{
			using type = decltype( _object );
			return _callback(
				std::forward<type>( _object ),
				std::forward<Args>( _args )... );
		};
		if constexpr( std::is_same_v<RetType, void> )
		{
			std::visit( NoReturnCall, v );
		}
		else
		{
			return std::visit( ReturnCall, v );
		}
	}*/

	template< typename CallOn, typename Callable, typename...Args>
	auto Visit( Callable _callback, Args&&..._args )
	{
		static_assert( std::is_member_function_pointer_v<Callable>,
			"Variant::Visit calls a member function on it's active member,\n"
			"therefore, Callable must be a member function of the active type\n"
			"stored in this Variant\n" );
		auto Call = [ & ]( auto& _object )
		{
			if constexpr( is_similar_v<decltype( _object ), CallOn> )
			{
				using RetType =
					decltype( ( _object.*_callback )( std::forward<Args>( _args )... ) );
				if constexpr( std::is_void_v<RetType> )
				{
					( _object.*_callback )( std::forward<Args>( _args )... );
				}
				else
				{
					return ( _object.*_callback )( std::forward<Args>( _args )... );
				}
			}
		};

		using CallRetType =
			decltype( std::visit( Call, v ) );

		if constexpr( std::is_same_v<CallRetType, void> )
		{
			std::visit( Call, v );
		}
		else
		{
			return std::visit( Call, v );
		}
	}

	template<typename CallWith, typename Callable, typename...Args>
	auto PassTo( Callable&& _callback, Args&&..._args )
	{
		static_assert(
			std::is_function_v<Callable> &&
			!std::is_member_function_pointer_v<Callable>,
			"Varaint::PassTo Callable must not be a member function pointer."
			);
		auto Call = [ & ]( auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;
			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				_callback( std::forward<ActiveType>( _object ), std::forward<Args>( _args )... );
			}
		};
		std::visit( Call, v );
	}

	template<typename CallWith, typename Class, typename Callable, typename...Args>
	auto PassTo( Class& _class, Callable _callback, Args&&..._args )
	{
		static_assert( std::is_member_function_pointer_v<Callable>,
			"Variant::PassTo calls a member function from the Class argument,\n"
			"therefore, Callable must be a member function of the Class argument\n");

		auto Call = [ & ]( auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;
			
			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				using RetType =
					decltype( ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... ) );

				if constexpr(std::is_void_v<RetType>)
				{
					( _class.*_callback)( 
						std::forward<ActiveType>( _object ), 
						std::forward<Args>( _args )... );
				}
				else
				{
					return ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
			}
		};

		using CallReturnType = decltype( std::visit( Call, v ) );
		if constexpr( std::is_void_v<CallReturnType> )
			std::visit( Call, v );
		else
			return std::visit( Call, v );
	}

	template<typename CallWith, typename Callable, typename...Args>
	auto PassTo( Callable&& _callback, Args&&..._args )const
	{
		static_assert(
			std::is_function_v<Callable> &&
			!std::is_member_function_pointer_v<Callable>,
			"Varaint::PassTo Callable must not be a member function pointer."
			);
		auto Call = [ & ]( const auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;
			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				_callback( std::forward<ActiveType>( _object ), std::forward<Args>( _args )... );
			}
		};
		std::visit( Call, v );
	}

	template<typename CallWith, typename Class, typename Callable, typename...Args>
	auto PassTo( const Class& _class, Callable _callback, Args&&..._args )const
	{
		static_assert( std::is_member_function_pointer_v<Callable>,
			"Variant::PassTo calls a member function from the Class argument,\n"
			"therefore, Callable must be a member function of the Class argument\n" );

		auto Call = [ & ]( const auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;

			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				using RetType =
					decltype( ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... ) );

				if constexpr( std::is_void_v<RetType> )
				{
					( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
				else
				{
					return ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
			}
		};

		using CallReturnType = decltype( std::visit( Call, v ) );
		if constexpr( std::is_void_v<CallReturnType> )
			std::visit( Call, v );
		else
			return std::visit( Call, v );
	}

private:
	std::variant<Variants...> v;
};
#include <functional>
Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	player.AddComponent<Position>( 400.f, 300.f );
	player.AddComponent<Velocity>( 0.f, 0.f );
	player.AddComponent<Shape>( Rect{ -16.f,-64.f,16.f,0.f }, Colors::Red );
	constexpr auto has = has_type<Position, Damage, Dimension, Health, Orientation, Position, Shape, Shield, Velocity>();
	constexpr auto idx = get_index<0, Position, Damage, Dimension, Health, Orientation, Position, Shape, Shield, Velocity>();

	/*world.AddSystem<MoveSystem>();
	world.AddSystem<DrawSystem>();
	world.AddSystem<CollisionSystem>();*/

	Collection<Position, Velocity, Orientation, Health, Shield, Damage, Dimension, Shape> collection;

	collection.AddVariant<Position>( 400.f, 300.f );
	
	int a = 0;
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
}

void Game::ComposeFrame()
{
	/*if( world.HasSystem<DrawSystem>() )
		world.GetSystem<DrawSystem>().Draw( gfx );*/
}
