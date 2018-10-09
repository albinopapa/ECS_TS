#pragma once

#include "Includes.h"

class Graphics;
class Keyboard;

class WorldDispatcher
{
public:
	WorldDispatcher( float _dt, Graphics& _graphics, Keyboard& _keyboard );
	void operator()( Movable& _system );
	void operator()( Drawable& _system )const;
	void operator()( Collidable& _system );
	void operator()( Controllable& _system );
private:
	float dt;
	Graphics& gfx;
	Keyboard& kbd;
};
