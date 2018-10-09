#include "World.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "SystemDrawable.h"
#include "SystemMovable.h"
#include "SystemCollidable.h"
#include "SystemControllable.h"

WorldDispatcher::WorldDispatcher( float _dt, Graphics & _graphics, Keyboard& _keyboard )
	:
	dt( _dt ),
	gfx( _graphics ),
	kbd( _keyboard )
{
}

void WorldDispatcher::operator()( Movable& _system )
{
	_system.execute( MovableMessageDispatcher( _system ), MovableExecuteDispatcher( dt ) );
}

void WorldDispatcher::operator()( Drawable& _system ) const
{
	_system.execute( DrawableMessageDispatcher( _system ), DrawableExecuteDispatcher( gfx ) );
}

void WorldDispatcher::operator()( Collidable& _system )
{
	_system.execute( CollidableMessageDispatch( _system ), CollidableExecuteDispatch( _system ) );
}

void WorldDispatcher::operator()( Controllable& _system )
{
	_system.execute( ControllableMessageDispatch( _system ), ControllableExecuteDispatch( _system, kbd ) );
}
