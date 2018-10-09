#pragma once

#include "Includes.h"
#include "SystemMessageDispatchBase.h"

class Graphics;

class DrawableExecuteDispatcher 
{
public:
	DrawableExecuteDispatcher( Graphics& _graphics );
	
	void operator()( const Player& _entity );
	void operator()( const Enemy& _entity );
	void operator()( const Wall& _entity );
private:
	Graphics& gfx;
};

class DrawableMessageDispatcher : public SystemMessageDispatchBase<Drawable, Position, Shape>
{
public:
	DrawableMessageDispatcher( Drawable& _parent );
	/*void operator()( const ComponentAdded& _message );
	void operator()( const ComponentRemoved& _message );
	void operator()( const EntityAdded& _message );
	void operator()( const EntityRemoved& _message );*/
	// Unhandled
	template<typename MessageType>void operator()( const MessageType& ) {}

private:
	//Drawable& parent;
};
