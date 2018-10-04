#pragma once

#include <algorithm>
#include <type_traits>
#include <variant>
#include <vector>
#include <functional>

template<typename Container,typename Pred>
auto find_if( Container& _container, Pred&& _pred )
{
	auto beg = _container.begin();
	auto end = _container.end();
	return std::find_if( beg, end, _pred );
}

template<typename T, typename Pred>
auto erase_if( std::vector<T>& _container, Pred&& _pred )
{
	auto beg = _container.begin();
	auto end = _container.end();

	auto remit = std::remove_if( beg, end, _pred );
	return _container.erase( remit, end );
}

template<typename T>bool has_alt() { return true; }

template<typename T, typename Pred>
auto swap_and_pop_if( std::vector<T>& _container, Pred&& _pred )
{
	if( auto findit = std::find_if( _container.begin(), _container.end(), _pred );
		findit != _container.end() )
	{
		const auto offset = findit - _container.begin();

		if( _container.empty() )
		{
			return _container.end();
		}
		else if( _container.size() > 1 )
		{
			if constexpr( std::is_pointer_v<T> )
			{
				std::swap( **findit, *_container.back() );
			}
			else
			{
				std::swap( *findit, _container.back() );
			}
		}

		_container.erase( _container.end() - 1 );

		return offset < _container.size() ? _container.begin() + offset : _container.end();
	}
}

template</*template<typename...> typename Template,*/ typename Pred, typename...TemplateTypes>
auto swap_and_pop_if( std::vector<std::variant<TemplateTypes...>>& _container, Pred&& _pred )
{
	auto beg = _container.begin();
	auto end = _container.end();

	auto findit = std::find_if( beg, end, _pred );
	if( findit != end )
	{
		switch( _container.size() )
		{
			case 0:
				return findit;
			case 1:
				_container.clear();
				return _container.end();
			default:
			{
				using tbase = 
					std::remove_const<std::remove_pointer_t<std::variant<TemplateTypes...>>>;
				if constexpr( std::is_pointer_v<std::variant<TemplateTypes...>> )
				{
					_container.back()->swap( *findit );
				}
				else
				{
					_container.back().swap( *findit );
				}

				_container.pop_back();
				return _container.end();
			}
		}
	}
}

template<typename Fn> struct Finally
{
	Finally( Fn _lastcall ) : lastcall( _lastcall ){}
	~Finally(){ lastcall(); }

	Fn lastcall;
};
