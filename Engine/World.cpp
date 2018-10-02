#include "World.h"
#include "Graphics.h"
#include "Entity.h"
#include "System.h"


World::Dispatcher::Dispatcher( float _dt, Graphics & _graphics )
	:
	dt( _dt ),
	gfx( _graphics )
{
}

void World::Dispatcher::operator()( Movable & _system )
{
	_system.execute( Movable::Dispatcher( dt ) );
}

void World::Dispatcher::operator()( const Drawable & _system ) const
{
	_system.execute( Drawable::Dispatcher( gfx ) );
}

World::World()
	:
	ECS_MailBox<ECS_WorldMessageFilter>( *this )
{
}