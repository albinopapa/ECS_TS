#pragma once

#include <variant>

template<typename T, typename Callable>
void visit_unary( T& _variant, Callable&& _callable )
{
	std::visit( _callable, _variant );
}
template<typename T, typename Callable>
void visit_binary( T&& _variant1, T&& _variant2, Callable&& _callable )
{
	std::visit( [ & ]( auto& v1 )
		{
			std::visit( [ & ]( auto& v2 )
				{
					_callable( v1, v2 );
				}, _variant2 );
		}, _variant1 );
}