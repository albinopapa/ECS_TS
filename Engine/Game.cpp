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


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	vworld( g_factory.create_world<screws::ECS_World<system_t,message_t,WorldMessageFilter>>() )
{
	auto& world = std::get<screws::ECS_World<system_t, message_t, WorldMessageFilter>>( *vworld );
	
	shared_resource<system_t> vmovable = 
		g_factory.create_system<screws::ECS_System<
		MovableDispatcher, 
		MovableMessageHandler, 
		entity_t, 
		message_t, 
		SystemMessageFilter>>();

	screws::ECS_System<
		MovableDispatcher,
		MovableMessageHandler,
		entity_t,
		message_t,
		SystemMessageFilter>& movable = 
			std::get<screws::ECS_System<
				MovableDispatcher,
				MovableMessageHandler,
				entity_t,
				message_t,
				SystemMessageFilter>>( *vmovable );

	
	world.add_receiver( movable.get_receiver() );
	
	auto position = 
		g_factory.create_component<screws::ECS_Component<position_tag>>( 400.f, 300.f );
	auto velocity = 
		g_factory.create_component<screws::ECS_Component<velocity_tag>>( 0.f, 0.f );
	auto shape =
		g_factory.create_component<screws::ECS_Component<shape_tag>>( Circle{ {0.f,0.f}, 20.f }, Colors::Blue );
	auto ball =
		g_factory.create_entity<screws::ECS_Entity<player_tag, component_t>>(
			position,
			velocity,
			shape );

	movable.add_entity( ball );

	world.add_system( vmovable );
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
	auto& v_world = *vworld;
	screws::ECS_World<
		system_t,
		message_t,
		WorldMessageFilter>& world = std::get<screws::ECS_World<
		system_t,
		message_t,
		WorldMessageFilter>>( v_world );

	world.tick( WorldDispatcher( .016f, gfx ) );
}

void Game::ComposeFrame()
{
	
}
