#pragma once

#include "ECS_Algorithms.h"
#include "ECS_Utilities.h"



template<typename EntityVariant>
class ECS_System
{
public:
	using entity_resource = shared_resource<EntityVariant>;
	using iterator = std::vector<entity_resource>::iterator;
	using const_iterator = std::vector<entity_resource>::const_iterator;

public:
	void add_entity( entity_resource _entity )
	{
		entities.push_back( std::move( _entity ) );
	}

	void remove_entity( const entity_resource& _entity )
	{
		if( auto it = find_if( entities, is_same_resource( _entity ) ); it != entities.end() )
		{
			entities.erase( it );
		}
	}

	template<typename SystemVisitor>
	void execute( SystemVisitor& _visitor )
	{
		for( auto& ventity : entities )
		{
			std::visit( _visitor, *ventity );
		}
	}

	template<typename SystemVisitor>
	void execute( const SystemVisitor& _visitor )const
	{
		for( auto& ventity : entities )
		{
			std::visit( _visitor, *ventity );
		}
	}


	iterator find_entity( const entity_resource& _entity )
	{
		return find_if( entities, is_same_resource<entity_resource>( _entity ) );
	}

	const_iterator find_entity( const entity_resource& _entity )const
	{
		return find_if( entities, is_same_resource<entity_resource>( _entity ) );
	}

protected:
	std::vector<entity_resource> entities;
};
