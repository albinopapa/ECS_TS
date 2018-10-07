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
#include "ChiliMath.h"
#include "FrameTimer.h"
#include "Game.h"
#include "Aliases.h"
#include <cassert>

#include <memory>
void fn()
{
	auto t = std::make_shared<int>( 32 );

}

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	vworld( g_factory.create_world<World>() )
{
	fn();
	auto position = g_factory.create_component<Position>( 400.f, 300.f );
	auto velocity = g_factory.create_component<Velocity>( 0.f, 0.f );
	auto shape = g_factory.create_component<Shape>( Circle{ {0.f,0.f}, 20.f }, Colors::Blue );
	
	auto vball = g_factory.create_entity<Player>( position, velocity, shape );
	auto& ball = std::get<Player>( *vball );
	ball.set_mailbox( g_factory.create_receiver(), g_factory.create_sender() );

	auto vmovable = g_factory.create_system<Movable>();
	Movable& movable = std::get<Movable>( *vmovable );
	movable.set_mailbox( g_factory.create_receiver(), g_factory.create_sender() );
	movable.add_entity( vball );
	ball.add_receiver( movable.get_receiver() );

	auto vdrawable = g_factory.create_system<Drawable>();
	Drawable& drawable = std::get<Drawable>( *vdrawable );
	drawable.set_mailbox( g_factory.create_receiver(), g_factory.create_sender() );
	drawable.add_entity( vball );
	ball.add_receiver( drawable.get_receiver() );

	auto& world = std::get<World>( *vworld );
	world.set_mailbox( g_factory.create_receiver(), g_factory.create_sender() );
	world.add_receiver( movable.get_receiver() );
	world.add_receiver( drawable.get_receiver() );
	world.add_system( vmovable );
	world.add_system( vdrawable );
	
	ball.remove_component<Velocity>();
	ball.send_message<ComponentRemoved>( vball );
}

Game::~Game()
{
	g_factory.release_all();
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
	World& world = std::get<World>( *vworld );
	world.tick( WorldDispatcher( .016f, gfx ) );
}

void Game::ComposeFrame()
{
	
}
