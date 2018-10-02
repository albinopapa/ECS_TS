#pragma once

#include "ECS_Algorithms.h"
#include "ECS_Utilities.h"
#include <optional>

namespace screws
{
	template<typename ComponentVariant>
	class ECS_Entity
	{
	public:
		using component_resource = shared_resource<ComponentVariant>;
		using iterator = std::vector<component_resource>::iterator;
		using const_iterator = std::vector<component_resource>::const_iterator;

	public:
		ECS_Entity() = default;

		template<typename...ComponentResources>
		ECS_Entity( ComponentResources&&... _components )
		{
			add_components( std::forward<Components>( _components )... );
		}

		void add_component( component_resource _component )
		{
			components.push_back( _component );
		}

		template<typename First, typename...Rest>
		void add_components( First&& _first, Rest&&... _rest )
		{
			static_assert( std::is_same_v<First, component_t>,
				"add_components requires: shared_resource<component_t> types." );

			components.push_back( _first );
			if constexpr( sizeof...( Rest ) > 0 )
			{
				add_components( std::forward<Rest>( _rest )... );
			}
		}

		void remove_component( component_resource _component ) {}

		template<typename ComponentType>
		iterator find_component()noexcept
		{
			return find_if( components,
				[ & ]( const shared_resource<component_t>& _component )
				{
					return std::holds_alternative<ComponentType>( *_component );
				} );
		}

		template<typename ComponentType>
		const_iterator find_component()const noexcept
		{
			return find_if( components,
				[ & ]( const shared_resource<component_t>& _component )
				{
					return std::holds_alternative<ComponentType>( *_component );
				} );
		}

		template<typename ComponentType>
		std::optional<std::reference_wrapper<ComponentType>> get_component()
		{
			if( auto it = find_component<ComponentType>();
				it != components.end() )
			{
				return { std::ref( std::get<ComponentType>( **it ) ) };
			}

			return { std::nullopt };
		}

		template<typename ComponentType>
		std::optional<std::reference_wrapper<const ComponentType>> get_component()const
		{
			if( auto it = find_component<ComponentType>();
				it != components.end() )
			{
				return { std::cref( std::get<ComponentType>( **it ) ) };
			}

			return { std::nullopt };
		}

		template<typename ComponentType>
		bool has_component()const noexcept
		{
			return find_component<ComponentType>() != end();
		}

		template<typename ComponentType, typename...Rest>
		bool has_all_components()const
		{
			bool hasAll = has_component<ComponentType>();

			if constexpr( sizeof...( Rest ) > 0 )
			{
				if( hasAll )
				{
					hasAll &= has_all_components<Rest...>();
				}
			}

			return hasAll;
		}

	private:
		std::vector<component_resource> components;
	};
}
