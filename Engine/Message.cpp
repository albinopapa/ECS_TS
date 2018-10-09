#include "Message.h"

screws::ECS_Message<componentAdded_tag>::ECS_Message( shared_resource<entity_t> _entity )
	:
	entity( std::move( _entity ) )
{
}

screws::ECS_Message<componentRemoved_tag>::ECS_Message( shared_resource<entity_t> _entity )
	:
	entity( std::move( _entity ) )
{
}

screws::ECS_Message<entityAdded_tag>::ECS_Message( shared_resource<entity_t> _entity )
	:
	entity( std::move( _entity ) )
{
}

screws::ECS_Message<entityRemoved_tag>::ECS_Message( shared_resource<entity_t> _entity )
	:
	entity( std::move( _entity ) )
{
}

screws::ECS_Message<systemAdded_tag>::ECS_Message( shared_resource<system_t> _system )
	:
	system( std::move( _system ) )
{
}

screws::ECS_Message<systemRemoved_tag>::ECS_Message( shared_resource<system_t> _system )
	:
	system( std::move( _system ) )
{
}

screws::ECS_Message<key_press_tag>::ECS_Message( shared_resource<entity_t> _entity, Keyboard::Event _key_event )
	:
	key_event( _key_event ),
	entity( _entity )
{
}

screws::ECS_Message<key_release_tag>::ECS_Message( shared_resource<entity_t> _entity, Keyboard::Event _key_event )
	:
	key_event( _key_event ),
	entity( _entity )
{
}