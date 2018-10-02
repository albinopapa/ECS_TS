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
#include <cassert>
#include "ECS_Utilities.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	vworld( factory.create_world<World>() )
{
	World& world = std::get<World>( *vworld );
	
	shared_resource<system_t> vmove_system = factory.create_system<Movable>();
	Movable& move_system = std::get<Movable>( *vmove_system );

	world.add_receiver( move_system.get_receiver() );
	
	shared_resource<entity_t> ball = factory.create_entity<Player>( 
			Position( 400.f, 300.f ), 
			Velocity( 0.f, 0.f ), 
			Shape( Circle{ {0.f,0.f}, 20.f }, Colors::Blue ) );

	move_system.add_entity( ball );

	world.add_system( vmove_system );
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
	std::get<World>( *vworld ).tick( World::Dispatcher( .016f, gfx ) );
}

void Game::ComposeFrame()
{
	
}
