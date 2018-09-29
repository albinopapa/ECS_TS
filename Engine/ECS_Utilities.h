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


template<typename Type>
class shared_pool
{
public:
	class resource
	{
		class ref_counter
		{
		public:
			ref_counter( int _new_count )
				:
				count( _new_count )
			{
			}

			void inc() { ++count; }
			void dec() { --count; }
			int ref_count()const { return count; }

		private:
			int count = 0;
		};
	public:
		resource() = default;
		resource( const resource& _other )
			:
			guardian( _other.guardian ),
			ptr( new Type* )
		{
			counter = _other.counter;
			counter->inc();
			( *ptr ) = ( *_other.ptr );
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
		resource( shared_pool<Type>& _guardian, Type* _other )
			:
			guardian( std::addressof( _guardian ) ),
			counter( new ref_counter( 1 ) ),
			ptr( new Type* )
		{
			*ptr = _other;
		}
		~resource()
		{
			if( !counter )return;

			counter->dec();
			if( counter->ref_count() <= 0 )
			{
				auto* elem_pointer = *ptr;
				if( counter )
				{
					delete counter;
					counter = nullptr;
				}
				if( ptr )
				{
					delete ptr;
					ptr = nullptr;
				}
				guardian->on_backer_release( 
					random_access_iterator<shared_pool<Type>>( guardian, elem_pointer ) 
				);
			}
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
			return **ptr;
		}
		const Type& operator*()const
		{
			return **ptr;
		}

		Type* operator->()
		{
			return *ptr;
		}
		const Type* operator->()const
		{
			return *ptr;
		}

		operator bool()const
		{
			return ptr != nullptr && ( *ptr ) != nullptr;
		}

		bool operator==( const resource& _other )const
		{
			return *ptr == *_other.ptr;
		}
		bool operator!=( const resource& _other )const
		{
			return !( ( *this ) == _other );
		}

		Type* get()
		{
			return *ptr;
		}
		const Type* get()const
		{
			return *ptr;
		}

		int ref_count()const
		{
			return counter->ref_count();
		}
	private:
		mutable ref_counter* counter = nullptr;
		Type** ptr = nullptr;
		shared_pool<Type>* guardian;
		friend shared_pool<Type>;
	};

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
		container( std::make_unique<Type[]>( _count ) ),
		backer( _count ),
		count( _count ),
		cap( _count )
	{
		for( size_t i = 0; i < _count; ++i )
		{
			backer[ i ] = resource( *this, &container[ i ] );
		}
	}
	shared_pool( shared_pool&& _other )
		:
		count( _other.count ),
		cap( _other.cap ),
		container( std::move( _other.container ) ),
		backer( std::move( _other.backer ) )
	{
	}
	~shared_pool()
	{
		if( container )
		{
			for( int i = 0; i < count; ++i )
			{
				while( backer[ i ] )
				{
					backer[ i ].~resource();
				}
			}
			backer.clear();
		}
		container.reset();
	}

	shared_pool( const shared_pool& ) = delete;
	shared_pool& operator=( const shared_pool&  ) = delete;
	shared_pool& operator=( shared_pool&& _other )
	{
		if( this != std::addressof( _other ) )
		{
			for( int i = 0; i < size(); ++i )
			{
				backer[ i ].~resource();
			}
			container = std::move( _other.container );
			backer = std::move( _other.backer );
			count = _other.count;
			cap = _other.cap;

			_other.count = 0;
			_other.cap = 0;
		}

		return *this;
	}

	// Iteration
	iterator begin()
	{
		return random_access_iterator{ this, container.get() };
	}
	iterator end()
	{
		return random_access_iterator{ this, container.get() + size() };
	}
	const_iterator begin()const
	{
		return const_random_access_iterator{ this, container.get() };
	}
	const_iterator end()const
	{
		return const_random_access_iterator{ this, container.get() + size() };
	}
	const_iterator cbegin()const
	{
		return const_random_access_iterator{ this, container.get() };
	}
	const_iterator cend()const
	{
		return const_random_access_iterator{ this, container.get() + size() };
	}

	// Accessors
	resource& operator[]( size_t _index )
	{
		return backer[ _index ];
	}
	const resource& operator[]( size_t _index )const
	{
		return backer[ _index ];
	}
	resource& front() 
	{ 
		return backer[ 0 ]; 
	}
	resource& back() 
	{ 
		return backer[ count - 1 ]; 
	}
	const resource& front()const 
	{ 
		return backer[ 0 ]; 
	}
	const resource& back()const 
	{ 
		return backer[ count - 1 ]; 
	}

	// Insertions
	void push_back( const Type& t ) 
	{
		const int temp = count + 1;
		if( temp >= cap )
		{
			reserve( calculate_growth() );
		}
		if( temp < cap )
		{
			container[ count ] = t;
			backer[ count ] = resource( *this, std::addressof( container[ count ] ) );
			count = temp;
		}
	}
	void push_back( Type&& t )
	{
		const size_t temp = count + 1;
		if( temp >= cap )
		{
			reserve( calculate_growth() );
		}

		container[ count ] = std::move( t );
		backer[ count ] = resource( *this, std::addressof( container[ count ] ) );
		count = temp;
	}


	// Querys
	size_t size()const noexcept { return count; }
	size_t capacity()const noexcept { return cap; }
	bool empty()const noexcept { return count == 0; }

	// Erasure
	void pop_back()
	{
		if( empty() )return;
		checked_erase( count - 1 );
	}
	iterator erase( iterator _where )
	{
		const std::ptrdiff_t offset = _where - begin();

		if( empty() ||
			( ( offset <= 0 ) && ( offset >= std::ptrdiff_t( size() ) ) ) )
		{
			return end();
		}

		return checked_erase( offset );
	}

	void reserve( size_t _new_size )
	{
		if( _new_size < cap )
		{
			return;
		}

		container = grow_new_container( _new_size );

		reassign_backers( container );

		backer = grow_new_backer( _new_size );

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

		std::fill( container.get() + old_size, container.get() + size(), Type() );

		int count = old_size;
		std::for_each( backer.begin() + old_size, backer.end(),
			[ & ]( resource& _res )
			{
				_res = resource( *this, &container[ count++ ] );
			} );
	}

private:
	iterator checked_erase( size_t _offset )
	{
		release_backer( _offset );
		
		if( _offset > size() ) return end();

		return begin() + _offset;
	}
	void release_backer( size_t _offset )
	{
		auto& res = backer[ _offset ];
		res.~resource();
	}
	void on_backer_release( iterator& _where )
	{
		const std::ptrdiff_t offset = _where - begin();

		auto& old_back = back();
		auto& new_back = backer[ offset ];
		if( offset < std::ptrdiff_t( count - 1 ) )
		{
			std::swap( container[ offset ], cont_back() );
			std::swap( old_back, new_back );
			*new_back.ptr = &container[ offset ];
		}

		cont_back().~Type();
		backer.pop_back();
		--count;
	}

	size_t calculate_growth()const noexcept
	{
		return cap == 0 ? 10 : cap * 3 / 2;
	}
	Type& cont_back()
	{
		return container[ size() - 1 ];
	}

	std::unique_ptr<Type[]> grow_new_container( size_t _new_size )
	{
		auto t_container = std::make_unique<Type[]>( _new_size );
		std::copy(
			std::move_iterator( container.get() ),
			std::move_iterator( container.get() + size() ),
			t_container.get() );

		return t_container;
	}
	void reassign_backers( const std::unique_ptr<Type[]>& _new_container )
	{
		for( int i = 0; i < size(); ++i )
		{
			backer[ i ] = resource( *this, &_new_container[ i ] );
		}
	}
	std::vector<resource> grow_new_backer( size_t _new_size )
	{
		std::vector<resource> t_backer = std::vector<resource>( _new_size );
		std::copy(
			std::move_iterator( backer.begin() ),
			std::move_iterator( backer.end() ),
			t_backer.begin() );

		return t_backer;
	}
private:
	std::unique_ptr<Type[]> container;
	std::vector<resource> backer;
	size_t count = 0, cap = 0;
};

template<typename T>
using shared_resource = typename shared_pool<T>::resource;