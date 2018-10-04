#include "World.h"
#include "Graphics.h"
#include "Includes.h"


WorldDispatcher::WorldDispatcher( float _dt, Graphics & _graphics )
	:
	dt( _dt ),
	gfx( _graphics )
{
}

void WorldDispatcher::operator()( screws::ECS_System<
	MovableDispatcher,
	MovableMessageHandler,
	entity_t,
	message_t,
	SystemMessageFilter>& _system )
{
	_system.execute( MovableMessageHandler( _system ), MovableDispatcher( dt ) );
}

void WorldDispatcher::operator()( screws::ECS_System<
	DrawableDispatcher,
	DrawableMessageHandler,
	entity_t,
	message_t,
	SystemMessageFilter>& _system ) const
{
	_system.execute( DrawableMessageHandler(_system), DrawableDispatcher( gfx ) );
}
