#include "Message.h"
#include "Includes.h"

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
