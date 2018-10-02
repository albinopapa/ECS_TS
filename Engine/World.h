#pragma once

#include "ECS_PostOffice.h"
#include "ECS_System.h"
#include "ECS_World.h"
#include "Includes.h"

class Graphics;

using ECS_WorldMessageFilter =
ECS_PostOffice::ECS_MessageFilter<SystemAdded, SystemRemoved>;

class World : 
	public ECS_Mailbox<ECS_WorldMessageFilter>, 
	public screws::ECS_World<system_t>
{
public:
	class Dispatcher
	{
	public:
		Dispatcher( float _dt, Graphics& _graphics );
		void operator()( Movable& _system );
		void operator()( const Drawable& _system )const;
	private:
		float dt;
		Graphics& gfx;
	};

public:
	World();
};
