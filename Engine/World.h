#pragma once

#include "variant_helper.h"
#include <algorithm>
#include <variant>
#include <vector>


template<typename...AcceptedSystems>
class World
{
	using SystemVector = std::vector<std::variant<AcceptedSystems...>>;
public:
	template<typename System>
	World& AddSystem( System&& _system )
	{
		systems.emplace_back( std::forward<System>( _system ) );
		return *this;
	}
	template<typename System>
	World& RemoveSystem( const System& _system )
	{
		const auto findit = FindSystem<System>();

		if( findit != systems.end() )
		{
			if( systems.size() > 1 )
			{
				std::swap( *findit, systems.back() );
				systems.pop_back();
			}
			else
			{
				systems.clear();
			}
		}
	}
	template<typename System>
	bool HasSystem()
	{
		return FindSystem<System>() != systems.end();
	}

private:
	using SystemIterator = typename SystemVector::iterator;
	using ConstSystemIterator = typename SystemVector::const_iterator;

	template<typename System>
	SystemIterator FindSystem()
	{
		auto it =
			std::find_if( systems.begin(), systems.end(),
				[ & ]( const auto& v )
				{
					return std::holds_alternative<System>( v );
				} );
		return it;
	}

private:
	SystemVector systems;
};
