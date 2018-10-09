#pragma once

#include "Includes.h"
#include "Component.h"
#include "Message.h"

template<typename Parent, typename...Requirements>
class SystemMessageDispatchBase
{
protected:
	SystemMessageDispatchBase( Parent& _parent )
		:
		parent( _parent )
	{
	}
	void operator()( const ComponentAdded& _message )
	{
		if( parent.has_entity( _message.entity ) )
		{
			if( std::visit( VerifyComponents<Requirements...>(), *_message.entity ) )
			{
				parent.add_entity( std::move( _message.entity ) );
			}
		}
	}

	void operator()( const ComponentRemoved& _message )
	{
		if( parent.has_entity( _message.entity ) )
		{
			if( !std::visit( VerifyComponents<Requirements...>(), *_message.entity ) )
			{
				parent.remove_entity( _message.entity );
			}
		}
	}

	void operator()( const EntityAdded& _message )
	{
		if( !parent.has_entity( _message.entity ) )
		{
			if( std::visit( VerifyComponents<Requirements...>(), *_message.entity ) )
			{
				parent.add_entity( std::move( _message.entity ) );
			}
		}
	}

	void operator()( const EntityRemoved& _message )
	{
		if( parent.has_entity( _message.entity ) )
		{
			if( std::visit( VerifyComponents<Requirements...>(), *_message.entity ) )
			{
				parent.remove_entity( _message.entity );
			}
		}
	}

protected:
	Parent& parent;
};