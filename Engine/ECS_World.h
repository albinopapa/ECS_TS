#pragma once

#include "ECS_Utilities.h"
#include <vector>

namespace screws
{
	template<typename SystemVaraint>
	class ECS_World
	{
	public:
		using system_resource = shared_resource<SystemVaraint>;

	public:
		void add_system( system_resource _system )
		{
			systems.push_back( _system );
		}

		void remove_system( const system_resource _system )
		{
			if( auto it = find_if( systems, is_same_resource( _system ) ); 
				it != systems.end() )
			{
				systems.erase( it );
			}
		}

		template<typename WorldVisitor>
		void tick( WorldVisitor _visitor )
		{
			for( auto& vsystem : systems )
			{
				std::visit( _visitor, *vsystem );
			}
		}

	private:
		void process_messages()
		{

		}

	private:
		std::vector<system_resource> systems;
	};
}