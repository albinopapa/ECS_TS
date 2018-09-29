#pragma once

#include <cassert>

template<typename Container>
class const_random_access_iterator
{
public:
	using value_type = typename Container::value_type;
	using pointer = typename Container::const_pointer;
	using reference = typename Container::const_reference;
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;

	using container_ptr = const Container*;
	using type = const_random_access_iterator;
public:
	const_random_access_iterator() = default;
	const_random_access_iterator( container_ptr _cont, pointer _where )noexcept
		:
		cont( _cont ),
		ptr( _where )
	{
	}

	reference operator*()const noexcept
	{
		const auto pos = ptr - cont->begin().ptr;
		assert( pos < int( cont->size() ) );
		assert( pos >= 0 );
		return *ptr;
	}
	pointer operator->()const noexcept
	{
		const auto pos = ptr - cont->begin().ptr;
		assert( pos < int( cont->size() ) );
		assert( pos >= 0 );
		return ptr;
	}
	type& operator[]( difference_type _index )noexcept
	{
		*this = cont->begin() + _index;
		return *this;
	}

	type& operator++()noexcept
	{
		( *this ) += 1;
		return *this;
	}
	type operator++( int )noexcept
	{
		auto old = *this;
		++( *this );
		return old;
	}
	type& operator--()noexcept
	{
		( *this ) -= 1;
		return *this;
	}
	type operator--( int )noexcept
	{
		auto old = *this;
		--( *this );
		return old;
	}

	type& operator+=( difference_type _offset )noexcept
	{
		*this = *this + _offset;
		return ( *this );
	}
	type& operator-=( difference_type _offset )noexcept
	{
		*this = *this - _offset;
		return *this;
	}
	type operator+( difference_type _offset )const noexcept
	{
		return type( cont, ptr + _offset );
	}
	type operator-( difference_type _offset )const noexcept
	{
		return type( cont, ptr - _offset );
	}

	bool operator==( const type& _other )const noexcept
	{
		return ptr == _other.ptr;
	}
	bool operator!=( const type& _other )const noexcept
	{
		return !( ( *this ) == _other );
	}

	bool operator<( const type& _other )const
	{
		return ptr < _other.ptr;
	}
	bool operator>( const type& _other )const
	{
		return _other < ( *this );
	}
	bool operator<=( const type& _other )const
	{
		return !( ( *this ) > _other );
	}
	bool operator>=( const type& _other )const
	{
		return !( ( *this ) < _other );
	}
	std::ptrdiff_t operator-(
		const_random_access_iterator<Container> _other )
	{
		return ptr - _other.ptr;
	}
private:
	container_ptr cont = nullptr;
	pointer ptr = nullptr;
};

template<typename Container>
class random_access_iterator
{
public:
	using value_type = typename Container::value_type;
	using pointer = typename Container::pointer;
	using reference = typename Container::reference;
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;

	using container_ptr = Container * ;
	using type = random_access_iterator;
public:
	random_access_iterator() = default;
	random_access_iterator( container_ptr _cont, pointer _where )noexcept
		:
		cont( _cont ),
		ptr( _where )
	{
	}

	operator const_random_access_iterator<Container>()const
	{
		return const_random_access_iterator<Container>( cont, ptr );
	}
	reference operator*()const noexcept
	{
		const auto pos = ptr - cont->begin().ptr;
		assert( pos < int( cont->size() ) );
		assert( pos >= 0 );
		return *ptr;
	}
	pointer operator->()const noexcept
	{
		const auto pos = ptr - cont->begin().ptr;
		assert( pos < cont->size() );
		assert( pos >= 0 );
		return ptr;
	}
	type& operator[]( difference_type _index )noexcept
	{
		assert( _index >= 0 && _index < std::ptrdiff_t( cont->size() ) );
		ptr += _index;
		return *this;
	}

	type& operator++()noexcept
	{
		( *this ) += 1;
		return *this;
	}
	type operator++( int )noexcept
	{
		auto old = *this;
		++( *this );
		return old;
	}
	type& operator--()noexcept
	{
		( *this ) -= 1;
		return *this;
	}
	type operator--( int )noexcept
	{
		auto old = *this;
		--( *this );
		return old;
	}

	type& operator+=( difference_type _offset )noexcept
	{
		ptr += _offset;
		return *this;
	}
	type& operator-=( difference_type _offset )noexcept
	{
		type other = cont->begin();
		const auto index = ( ptr - other.ptr ) - _offset;
		return ( *this )[ index ];
	}
	type operator+( difference_type _offset )const noexcept
	{
		return type( *this ) += _offset;
	}
	type operator-( difference_type _offset )const noexcept
	{
		return type( *this ) -= _offset;
	}

	bool operator==( const type& _other )const noexcept
	{
		return ptr == _other.ptr;
	}
	bool operator!=( const type& _other )const noexcept
	{
		return !( ( *this ) == _other );
	}

	bool operator<( const type& _other )const
	{
		return ptr < _other.ptr;
	}
	bool operator>( const type& _other )const
	{
		return _other < ( *this );
	}
	bool operator<=( const type& _other )const
	{
		return !( ( *this ) > _other );
	}
	bool operator>=( const type& _other )const
	{
		return !( ( *this ) < _other );
	}

	std::ptrdiff_t operator-(
		random_access_iterator<Container> _other )const noexcept
	{
		return ptr - _other.ptr;
	}

private:
	container_ptr cont = nullptr;
	pointer ptr = nullptr;
};
