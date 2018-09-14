#pragma once

#include <cassert>
#include <variant>
#include <vector>


class Entity
{
public:
	static Entity Create() 
	{
		return {};	
	}
	template<typename ComponentT, typename...Args> Entity& AddComponent( Args&&...args )
	{
		components.emplace_back( ComponentT( std::forward<Args>( args )... ) );
		return *this;
	}
	template<typename ComponentT> Entity& AddComponent( ComponentT&& _component )
	{
		components.push_back( std::forward<ComponentT>( _component ) );
		return *this;
	}
	template<typename ComponentT> void RemoveComponent( ComponentT _component )
	{
		auto findit = std::find_if(components.begin(),components.end(),
			[]( const auto& v )
			{
				bool comp_found = false;
				std::visit( [ & ]( const auto& comp )
					{
						using type = std::decay_t<decltype( comp )>;
						if constexpr( std::is_same_v<type, ComponentT> )
						{
							comp_found = true;
						}
					}, v );

				return comp_found;
			} );

		if( findit != components.end() )
		{
			if( components.size() == 1 )
			{
				components.clear();
			}
			else
			{
				std::swap( *findit, components.back() );
				components.pop_back();
			}
			
		}
	}

	template<typename ComponentT> bool HasComponent()const noexcept
	{
		for( auto& v : components )
		{
			if( std::holds_alternative<ComponentT>( v ) )
			{
				return true;
			}
		}
		return false;
	}
	template<typename ComponentT> ComponentT& GetComponent()
	{
		assert( ( !components.empty() ) && "Component vector is empty." );

		ComponentT* comp = nullptr;
		for( auto& v : components )
		{
			auto lookup = [ & ]( auto& obj )noexcept
			{
				using type = std::decay_t<decltype( obj )>;
				if constexpr( std::is_same_v<type, ComponentT> )
				{
					comp = &obj;
				}
			};

			std::visit( lookup, v );

			if( comp ) return *comp;
		}

		throw std::runtime_error( "Component not found in entity." );
	}
	template<typename ComponentT> const ComponentT& GetComponent()const
	{
		ComponentT* comp = nullptr;
		for( auto& v : components )
		{
			std::visit(
				[ & ]( auto& obj )
				{
					if constexpr( std::is_same_v<decltype( obj ), ComponentT> )
					{
						comp = &obj;
					}
				}, v );

			if( comp )
				return *comp;
		}

		throw std::runtime_error( "Component not found in entity." );
	}

private:
	using ComponentVector =
		std::vector<
			std::variant<
				Position,
				Velocity,
				Orientation,
				Health,
				Shield,
				Damage,
				Dimension,
				Shape
			>
		>;
private:
	ComponentVector components;
};

class Player :public Entity
{

};