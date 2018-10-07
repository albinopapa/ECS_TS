#pragma once

#include <memory>
#include <variant>
#include <vector>
#include "ECS_type_traits.h"
#include "ECS_iterators.h"


template<class Type>
class shared_ref
{
public:
	shared_ref() = default;

	shared_ref( Type&& _value )
		:
		ptr( std::make_shared( std::forward<Type>( _value ) ) )
	{
	}

	Type& operator&()
	{
		return *ptr;
	}
	const Type& operator&()const
	{
		return *ptr;
	}

private:
	std::shared_ptr<Type> ptr;
};

template<class Type>
shared_ref( Type&& )->shared_ref<Type>;


template<typename...Base>
class visitor : public Base...
{
public:
	using Base::operator()...;
};

template<typename...Base> visitor( Base... )->visitor<Base...>;


template<typename...Types>
class collection
{
public:
	using storage = std::variant<Types...>;

	using iterator = random_access_iterator<collection<Types...>>;
	using const_iterator = const_random_access_iterator<collection<Types...>>;
	using pointer = storage * ;
	using const_pointer = const storage*;
	using reference = storage & ;
	using const_reference = const storage&;
	using value_type = storage;

public:
	collection() = default;
	collection( size_t _count )
		:
		container( _count )
	{
	}

	// Iteration
	iterator begin()
	{
		pointer data = container.data();
		return random_access_iterator{ this, data };
	}
	iterator end()
	{
		pointer data = container.data() + size();
		return random_access_iterator{ this, data };
	}
	const_iterator begin()const
	{
		const_pointer data = container.data();
		return const_random_access_iterator{ this, data };
	}
	const_iterator end()const
	{
		const_pointer data = container.data() + size();
		return const_random_access_iterator{ this, data };
	}
	const_iterator cbegin()const
	{
		const_pointer data = container.data();
		return const_random_access_iterator{ this, data };
	}
	const_iterator cend()const
	{
		const_pointer data = container.data() + size();
		return const_random_access_iterator{ this, data };
	}

	// Accessors
	template<typename VisitorType>
	void visit_at( VisitorType&& _visitor, size_t _index )
	{
		std::visit( _visitor, container[ _index ] );
	}
	template<typename VisitorType>
	void operator()( VisitorType _visitor, size_t _index )
	{
		std::visit( _visitor, container[ _index ] );
	}
	reference operator[]( size_t _index )
	{
		return container[ _index ];
	}
	const_reference operator[]( size_t _index )const
	{
		return container[ _index ];
	}
	auto front() { return container.front(); }
	auto back() { return container.back(); }
	auto front()const { return container.front(); }
	auto back()const { return container.back(); }

	// Insertions
	template<typename T>void push_back( const T& t ) { container.push_back( t ); }

	// Querys
	size_t size()const noexcept { return container.size(); }
	bool empty()const noexcept { return container.empty(); }

	// Erasure
	void pop_back() { container.pop_back(); }
	void erase( iterator _where )
	{
		if( ( !iterator_in_range( _where ) ) || size() == 0 )
		{
			return;
		}

		if( ( _where == ( end() - 1 ) ) || size() == 1 )
		{
			pop_back();
			return;
		}


		( *_where ).swap( back() ); pop_back();
	}

	template<typename T, typename Fn, typename...Args>
	void each_of_type( Fn _do_this, Args&&... _args )
	{
		int index = 0;
		auto type_lookup = [ & ]( auto& _object )
		{
			index++;
			if constexpr( std::is_same_v<remove_cv_ref_t<decltype( _object )>, T> )
			{
				_do_this( _object, index, std::forward<Args>( _args )... );
			}
		};

		for( auto& v : ( *this ) )
		{
			std::visit( type_lookup, v );
		}
	}

private:
	template<typename Iter>
	bool iterator_in_range( Iter _iter )const
	{
		return ( _iter >= begin() ) && ( _iter < end() );
	}
private:
	std::vector<storage> container;
};

template<typename Type> class shared_pool;

class ref_counter
{
public:
	ref_counter() = default;
	ref_counter( int _new_count ):count( _new_count ){}

	void inc() { ++count; }
	void dec() { --count; }
	int ref_count()const { return count; }

private:
	int count = 0;
};

template<typename Type>
class resource
{
public:
public:
	resource() = default;
	resource( const resource& _other )
		:
		guardian( _other.guardian ),
		ptr( _other.ptr ),
		counter( _other.counter )
	{
		counter->inc();
		ptr = _other.ptr;
	}
	resource( resource&& _other )
		:
		guardian( _other.guardian ),
		counter( _other.counter ),
		ptr( _other.ptr )
	{
		_other.counter = nullptr;
		_other.ptr = nullptr;
		_other.guardian = nullptr;
	}
	resource( shared_pool<Type>& _guardian, Type* _other, ref_counter* _counter )
		:
		guardian( std::addressof( _guardian ) ),
		counter( _counter ),
		ptr( _other )
	{
		counter->inc();
	}
	~resource()
	{
		if( counter )
			counter->dec();
	}

	resource& operator=( const resource& _other )
	{
		if( std::addressof( _other ) != this )
		{
			*this = resource( _other );
		}

		return *this;
	}
	resource& operator=( resource&& _other )
	{
		if( this != std::addressof( _other ) )
		{
			( *this ).~resource();

			counter = _other.counter;
			ptr = _other.ptr;
			guardian = _other.guardian;

			_other.counter = nullptr;
			_other.ptr = nullptr;
			_other.guardian = nullptr;
		}

		return *this;
	}

	Type& operator*()
	{
		return *ptr;
	}
	const Type& operator*()const
	{
		return *ptr;
	}

	Type* operator->()
	{
		return ptr;
	}
	const Type* operator->()const
	{
		return ptr;
	}

	operator bool()const
	{
		return ptr != nullptr;
	}

	bool operator==( const resource& _other )const
	{
		return ptr == _other.ptr;
	}
	bool operator!=( const resource& _other )const
	{
		return !( ( *this ) == _other );
	}

	Type* get()
	{
		return ptr;
	}
	const Type* get()const
	{
		return ptr;
	}

	int ref_count()const
	{
		return counter->ref_count();
	}
private:
	mutable ref_counter* counter = nullptr;
	Type* ptr = nullptr;
	shared_pool<Type>* guardian = nullptr;
	friend shared_pool<Type>;
};

template<typename Type>
class shared_pool
{
public:
public:
	using iterator = random_access_iterator<shared_pool<Type>>;
	using const_iterator = const_random_access_iterator<shared_pool<Type>>;
	using value_type = Type;
	using pointer = value_type * ;
	using const_pointer = const value_type*;
	using reference = value_type & ;
	using const_reference = const value_type&;

public:
	shared_pool() = default;
	shared_pool( size_t _count )
		:
		container( _count ),
		counters( _count, ref_counter( 0 ) ),
		backer( _count ),
		count( _count ),
		cap( _count )
	{
		for( size_t i = 0; i < _count; ++i )
		{
			backer[ i ] = resource(
					*this,
					std::addressof( container[ i ] ),
					std::addressof( counters[ i ] ) 
				);
		}
	}
	shared_pool( shared_pool&& _other )
		:
		count( _other.count ),
		cap( _other.cap ),
		container( std::move( _other.container ) ),
		backer( std::move( _other.backer ) ),
		counters( std::move( _other.counters ) )
	{
		_other.count = 0;
		_other.cap = 0;
	}

	shared_pool( const shared_pool& ) = delete;
	shared_pool& operator=( const shared_pool&  ) = delete;
	shared_pool& operator=( shared_pool&& _other )
	{
		if( this != std::addressof( _other ) )
		{
			count = _other.count;
			cap = _other.cap;
			container = std::move( _other.container );
			backer = std::move( _other.backer );
			counters = std::move( _other.counters );
			
			int counter = 0;
			for( auto& element : backer )
			{
				element = resource(
					*this,
					std::addressof( container[ counter ] ),
					std::addressof( counters[ counter ] )
				);
				++counter;
			}

			_other.count = 0;
			_other.cap = 0;
		}

		return *this;
	}

	// Iteration
	iterator begin()
	{
		return random_access_iterator{ this, container.data() };
	}
	iterator end()
	{
		return random_access_iterator{ this, container.data() + size() };
	}
	const_iterator begin()const
	{
		return const_random_access_iterator{ this, container.data() };
	}
	const_iterator end()const
	{
		return const_random_access_iterator{ this, container.data() + size() };
	}
	const_iterator cbegin()const
	{
		return const_random_access_iterator{ this, container.data() };
	}
	const_iterator cend()const
	{
		return const_random_access_iterator{ this, container.data() + size() };
	}

	// Accessors
	resource<Type>& operator[]( size_t _index )
	{
		return backer[ _index ];
	}
	const resource<Type>& operator[]( size_t _index )const
	{
		return backer[ _index ];
	}
	resource<Type>& front()
	{ 
		return backer[ 0 ]; 
	}
	resource<Type>& back()
	{ 
		return backer[ count - 1 ]; 
	}
	const resource<Type>& front()const
	{ 
		return backer[ 0 ]; 
	}
	const resource<Type>& back()const
	{ 
		return backer[ count - 1 ]; 
	}

	// Insertions
	template<typename...Args>
	resource<Type>& emplace_back( Args&&... _args )
	{
		Finally do_last( [ & ] { ++count; } );
		if( will_reallocate( size() + 1 ) )
		{
			reserve( calculate_growth() );
		}

		auto& result = container.emplace_back( Type( std::forward<Args>( _args )... ) );
		backer[ count ] = resource<Type>(
			*this,
			std::addressof( container[ count ] ),
			std::addressof( counters[ count ] )
		);
		return backer[ count ];
	}
	void push_back( const Type& t ) 
	{
		Finally do_last( [ & ] { ++count; } );
		if( will_reallocate( size() + 1 ) )
		{
			reserve( calculate_growth() );
		}

		container[ count ] = t;
		backer[ count ] = resource(
			*this,
			std::addressof( container[ count ] ),
			std::addressof( counters[ count ] )
		);
	}
	void push_back( Type&& t )
	{
		Finally do_last( [ & ] { ++count; } );
		if( will_reallocate( size() + 1 ) )
		{
			reserve( calculate_growth() );
		}

		container.push_back( std::move( t ) );
		counters.push_back( ref_counter() );
		backer.push_back( resource(
			*this,
			std::addressof( container[ count ] ),
			std::addressof( counters[ count ] )
		) );


	}

	// Querys
	size_t size()const noexcept { return count; }
	size_t capacity()const noexcept { return cap; }
	bool empty()const noexcept { return count == 0; }

	// Erasure
	void pop_back()
	{
		erase( end() - 1 );
	}
	iterator erase( iterator _where )
	{
		const std::ptrdiff_t offset = _where - begin();

		if( ( empty() ) ||
			( offset <= 0 ) ||
			( offset >= std::ptrdiff_t( size() ) ) )
		{
			return end();
		}

		backer[ offset ].~resource();

		return ( offset > size() ) ? end() : begin() + offset;
	}
	void clear()
	{
		backer.clear();
		counters.clear();
		container.clear();
	}

	// Resizing
	void reserve( size_t _new_size )
	{
		if( _new_size < cap )
		{
			return;
		}


		container = grow_container( container, _new_size );
		counters = grow_container( counters, _new_size );
		backer = grow_container( backer, _new_size );
		reassign_backers();

		cap = _new_size;
	}
	void resize( size_t _new_size )
	{
		reserve( _new_size );
		if( _new_size < size() )
		{
			return;
		}

		const int old_size = size();
		count = _new_size;

		std::fill( container.begin() + old_size, container.begin() + size(), Type() );

		int count = old_size;
		std::for_each( backer.begin() + old_size, backer.end(),
			[ & ]( resource<Type>& _res )
			{
				Finally do_last( [ & ] { ++count; } );
				_res = resource(
					*this,
					std::addressof( container[ count ] ),
					std::addressof( counters[ count ] )
				);
			} );
	}

private:
	bool will_reallocate(size_t _new_size)const noexcept
	{
		return _new_size > cap;
	}
	size_t calculate_growth()const noexcept
	{
		return cap == 0 ? 10 : cap * 3 / 2;
	}

	template<typename VecType>
	std::vector<VecType> grow_container( std::vector<VecType>& vec, size_t _new_size )
	{
		std::vector<VecType> t_vector;
		t_vector.reserve( _new_size );
		for( size_t i = 0; i < count; ++i )
		{
			t_vector[ i ] = std::move( vec[ i ] );
		}

		return t_vector;
	}
	void reassign_backers()
	{
		int counter = 0;
		for( auto& element : backer )
		{
			Finally do_last( [ & ] { ++counter; } );
			element = resource(
				*this,
				std::addressof( container[ counter ] ),
				std::addressof( counters[ counter ] )
			);
		}
	}
private:
	std::vector<Type> container;
	std::vector<ref_counter> counters;
	std::vector<resource<Type>> backer;
	size_t count = 0, cap = 0;
};

template<typename T>
using shared_resource = typename resource<T>;

template<typename ResourceType>
struct is_same_resource
{
	is_same_resource( const ResourceType& _left ) :left( _left ) {}
	bool operator()( const ResourceType& right )
	{
		return left == right;
	}
	const ResourceType& left;
};

template<typename ResourceType>is_same_resource( ResourceType )->
is_same_resource<ResourceType>;

