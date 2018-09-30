#pragma once

#include <variant>
#include <vector>

#if defined(DEBUG) || defined(_DEBUG)
#define ThrowAssert(cond,message) assert((cond)&&(message))
#else
#define ThrowAssert(cond,message) if((cond))throw std::runtime_error(message);
#endif
template<typename T>
struct is_moveable
{
	static constexpr bool value =
		std::is_move_constructible_v<T>&&
		std::is_move_assignable_v<T>;
};

template<typename T, typename U>
void assign_pointer( T& t, U*& u )
{
	if constexpr( is_similar_v<T, U> )
	{
		u = &t;
	}
	else
	{
		u = nullptr;
	}
}


template<typename...Variants>
class Variant
{
public:
	static constexpr size_t num_variants = sizeof...( Variants );
public:
	template<typename...Args>
	Variant( Args&&... _args )noexcept
		:
		v( std::variant<Variants...>( std::forward<Args>( _args )... ) )
	{
	}
	Variant( const std::variant<Variants...>& _source )noexcept
		:
		v( _source )
	{
	}
	Variant( std::variant<Variants...>&& _source )noexcept
		:
		v( std::move( _source ) )
	{
	}

	template<typename T>static constexpr bool IsTypeAccepted() noexcept
	{
		return has_type<T, Variants...>();
	}
	template<typename T>bool IsActiveType()const noexcept
	{
		return std::holds_alternative<Type>( v );
	}

	template<typename T>auto Extract_Ptr_To()noexcept->T*
	{
		if constexpr( has_type<T, Variants...>() )
		{
			auto lookup = []( auto& _object )
			{
				if constexpr( is_similar_v<decltype( _object ), T> )
				{
					return true;
				}

				return false;
			};
			if( std::visit( lookup, v ) )
			{
				constexpr auto idx = get_index<0, T, Variants...>();
				return &std::get<idx>( v );
			}
		}

		return nullptr;
	}
	template<typename T>auto Extract_Ptr_To()const noexcept->const T* 
	{
		if constexpr( has_type<T, Variants...>() )
		{
			auto lookup = []( auto& _object )
			{
				if constexpr( is_similar_v<decltype( _object ), T> )
				{
					return true;
				}

				return false;
			};
			if( std::visit( lookup, v ) )
			{
				constexpr auto idx = get_index<0, T, Variants...>();
				return &std::get<idx>( v );
			}
		}

		return nullptr;
	}

	template<typename T>auto Extract_Ref_To()
	{
		static_assert( IsTypeAccepted<T>(),
			"Variant does not have a T in Variants...");

		return *Extract_Ptr_To<T>();
	}
	template<typename T>auto Extract_Ref_To()const
	{
		static_assert( IsTypeAccepted<T>(),
			"Variant does not have a T in Variants..." );

		return *Extract_Ptr_To<T>();
	}

	template<typename CallWith, typename Callable, typename...Args>
	auto PassTo( Callable&& _callback, Args&&..._args )noexcept
	{
		static_assert(
			std::is_function_v<Callable> &&
			!std::is_member_function_pointer_v<Callable>,
			"Varaint::PassTo Callable must not be a member function pointer."
			);
		auto Call = [ & ]( auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;
			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				_callback(
					std::forward<ActiveType>( _object ),
					std::forward<Args>( _args )... );
			}
		};

		std::visit( Call, v );
	}

	template<typename CallWith, typename Class, typename Callable, typename...Args>
	auto PassTo( Class&& _class, Callable _callback, Args&&..._args )noexcept
	{
		static_assert( std::is_member_function_pointer_v<Callable>,
			"Variant::PassTo calls a member function from the Class argument,\n"
			"therefore, Callable must be a member function of the Class argument\n" );

		auto Call = [ & ]( auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;

			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				using RetType =
					decltype( ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... ) );

				if constexpr( std::is_void_v<RetType> )
				{
					( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
				else
				{
					return ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
			}
		};

		using CallReturnType = decltype( std::visit( Call, v ) );
		if constexpr( std::is_void_v<CallReturnType> )
			std::visit( Call, v );
		else
			return std::visit( Call, v );
	}

	template<typename CallWith, typename Callable, typename...Args>
	auto PassTo( Callable&& _callback, Args&&..._args )const noexcept
	{
		static_assert(
			std::is_function_v<Callable> &&
			!std::is_member_function_pointer_v<Callable>,
			"Varaint::PassTo Callable must not be a member function pointer."
			);
		auto Call = [ & ]( const auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;
			if constexpr( is_similar_v<ActiveType, CallWith> )
			{
				_callback( std::forward<ActiveType>( _object ), std::forward<Args>( _args )... );
			}
		};
		std::visit( Call, v );
	}

	template<typename CallWith, typename Class, typename Callable, typename...Args>
	auto PassTo( const Class& _class, Callable _callback, Args&&..._args )const noexcept
	{
		static_assert( std::is_member_function_pointer_v<Callable>,
			"Variant::PassTo calls a member function from the Class argument,\n"
			"therefore, Callable must be a member function of the Class argument\n" );

		auto Call = [ & ]( const auto&& _object )
		{
			using ActiveType = std::decay_t<decltype( _object )>;

			if constexpr( is_similar_v<decltype( _object ), CallWith> )
			{
				using RetType =
					decltype( ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... ) );

				if constexpr( std::is_void_v<RetType> )
				{
					( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
				else
				{
					return ( _class.*_callback )(
						std::forward<ActiveType>( _object ),
						std::forward<Args>( _args )... );
				}
			}
		};

		using CallReturnType = decltype( std::visit( Call, v ) );
		if constexpr( std::is_void_v<CallReturnType> )
			std::visit( Call, v );
		else
			return std::visit( Call, v );
	}

	template<typename CallOn, typename Callable, typename...Args>
	auto Visit( Callable&& _callback, Args&&..._args )noexcept
	{
		auto Call = [ & ]( auto& _object )
		{
			if constexpr( is_similar_v<decltype( _object ), CallOn> )
			{
				using RetType =
					decltype( ( _object.*_callback )( std::forward<Args>( _args )... ) );
				if constexpr( std::is_same_v< RetType, void> )
				{
					( _object.*_callback )( std::forward<Args>( _args )... );
				}
				else
				{
					return ( _object.*_callback )( std::forward<Args>( _args )... );
				}
			}
		};

		using CallRetType =
			decltype( std::visit( Call, v ) );

		if constexpr( std::is_same_v<CallRetType, void> )
		{
			std::visit( Call, v );
		}
		else
		{
			return std::visit( Call, v );
		}
	}

	void SwapWith( Variant& _other )noexcept
	{
		v.swap( _other.v );
	}
private:
	std::variant<Variants...> v;
};

template<typename...CollectionTypes> class Collection;

template<
	typename container_pointer_type,
	typename iterator_category_tag,
	typename diff_type,
	typename val_type,
	typename pointer_type,
	typename reference_type,
	typename container_value_type
>
	class IteratorBase
{
public:
	using container = container_pointer_type;
	using type = IteratorBase;
	using iterator_category = iterator_category_tag;
	using difference_type = diff_type;
	using value_type = val_type;
	using pointer = pointer_type;
	using reference = reference_type;
	using container_value_t = container_value_type;
public:
	IteratorBase() = default;
	IteratorBase( container _cont, pointer _where )noexcept
		:
		cont( _cont ),
		ptr( _where )
	{
	}

	operator IteratorBase<
		const container_value_t *,
		std::random_access_iterator_tag,
		std::ptrdiff_t,
		value_type,
		const value_type*,
		const value_type&,
		container_value_t
	>()const
	{
		return { cont,ptr };
	}
	bool operator==( const IteratorBase& _other )const noexcept
	{
		return ptr == _other.ptr;
	}
	bool operator!=( const IteratorBase& _other )const noexcept
	{
		return !( ( *this ) == _other );
	}

	std::enable_if_t<!std::is_const_v<reference>, reference> operator*()
	{
		ThrowAssert( 
			( current_idx() >= 0 && current_idx() < cont->size() ), 
			"Iterator is out of bounds." );

		return *ptr;
	}
	reference operator*()const
	{
		ThrowAssert(
			( current_idx() >= 0 && current_idx() < cont->size() ),
			"Iterator is out of bounds." );

		return *ptr;
	}
	IteratorBase& operator[]( difference_type _index )
	{
		ThrowAssert(
			( ( ( *this ) - cont->begin() ) + _index >= 0 ),
			"Cannot seek past beginning"
		);
		ThrowAssert(
			( ( ( *this ) - cont->begin() ) + _index < cont->size() ),
			"Cannot seek past end"
		);

		ptr += _index;
		return *this;
	}

	IteratorBase& operator++()
	{
		( *this )[ current_idx() + 1 ];
		return *this;
	}
	IteratorBase& operator--()
	{
		( *this )[ current_idx() - 1 ];
		return *this;
	}
	IteratorBase  operator++( int )
	{
		auto old = *this;
		++( *this );
		return old;
	}
	IteratorBase  operator--( int )
	{
		auto old = *this;
		--( *this );
		return old;
	}

	IteratorBase& operator+=( difference_type _index )
	{
		( *this )[ current_idx() + _index ];
		return *this;
	}
	IteratorBase& operator-=( difference_type _index )
	{
		( *this )[ current_idx() - _index ];
		return *this;
	}
	IteratorBase operator+( difference_type _index )const
	{
		return IteratorBase( *this ) += _index;
	}
	IteratorBase operator-( difference_type _index )const
	{
		return IteratorBase( *this ) -= _index;
	}

	difference_type operator-( type& _other )const
	{
		return ptr - _other.ptr;
	}

	std::enable_if_t<!std::is_const_v<pointer>, pointer> operator->()
	{
		return ptr;
	}
	pointer operator->()const
	{
		return ptr;
	}
private:
	difference_type current_idx()const
	{
		return ( *this ) - cont->begin();
	}

private:
	container cont = nullptr;
	pointer ptr = nullptr;
};

template<typename...CollectionTypes>
using ConstCollectionIterator = IteratorBase<
	const Collection<CollectionTypes...>*,
	std::random_access_iterator_tag,
	std::ptrdiff_t,
	Variant<CollectionTypes...>,
	const Variant<CollectionTypes...>*,
	const Variant<CollectionTypes...>&,
	Collection<CollectionTypes...>
>;

template<typename...CollectionTypes>
using CollectionIterator = IteratorBase<
	Collection<CollectionTypes...>*,
	std::random_access_iterator_tag,
	std::ptrdiff_t,
	Variant<CollectionTypes...>,
	Variant<CollectionTypes...>*,
	Variant<CollectionTypes...>&,
	Collection<CollectionTypes...>
>;

//template<typename...CollectionTypes>
//class ConstCollectionIterator : IteratorBase<
//	typename Collection<CollectionTypes...>::variant_t,
//	const Collection<CollectionTypes...>*,
//	std::random_access_iterator_tag,
//	std::ptrdiff_t,
//	typename Collection<CollectionTypes...>::value_type,
//	typename Collection<CollectionTypes...>::const_pointer,
//	typename Collection<CollectionTypes...>::const_reference,
//	CollectionTypes...>
//{
//public:
//	using variant_t = typename Collection<CollectionTypes...>::variant_t;
//	using container = Collection<CollectionTypes...>;
//	using Type = ConstCollectionIterator;
//
//	using iterator_category = std::random_access_iterator_tag;
//	using difference_type = std::ptrdiff_t;
//	using value_type = typename Collection<CollectionTypes...>::value_type;
//	using pointer = typename Collection<CollectionTypes...>::const_pointer;
//	using reference = typename Collection<CollectionTypes...>::const_reference;
//
//public:
//	ConstCollectionIterator() = default;
//	ConstCollectionIterator( const container& _cont, pointer _where )
//		:
//		cont( &_cont ),
//		ptr( _where )
//	{
//	}
//
//private:
//	const container* cont;
//	pointer ptr;
//};

//template<typename...CollectionTypes>
//class CollectionIterator
//{
//public:
//	using value_type = typename Collection<CollectionTypes...>::value_type;
//	using iterator_category = std::random_access_iterator_tag;
//	using difference_type = std::ptrdiff_t;
//	using pointer = typename Collection<CollectionTypes...>::pointer;
//	using reference = typename Collection<CollectionTypes...>::reference;
//	using container = Collection<CollectionTypes...>;
//	using Type = CollectionIterator;
//public:
//	CollectionIterator() = default;
//	CollectionIterator( container& _cont, pointer _where )
//		:
//		cont( &_cont ),
//		ptr( _where )
//	{
//	}
//
//	operator ConstCollectionIterator<CollectionTypes...>()const
//	{
//		return { *cont,ptr };
//	}
//	bool operator==( const Type& _other )const
//	{
//		return ptr == _other.ptr;
//	}
//	bool operator!=( const Type& _other )const
//	{
//		return !( ( *this ) == _other );
//	}
//
//	reference operator*()
//	{
//		return *ptr;
//	}
//	Type& operator[]( difference_type _index )
//	{
//		assert( ( ( *this ) - cont->begin() ) + _index >= 0 && "Cannot seek past beginning" );
//		assert( ( ( *this ) - cont->begin() ) + _index < cont->size() && "Cannot seek past end" );
//		ptr += _index;
//		return *this;
//	}
//
//	Type& operator++()
//	{
//		( *this )[ current_idx() + 1 ];
//		return *this;
//	}
//	Type& operator--()
//	{
//		( *this )[ current_idx() - 1 ];
//		return *this;
//	}
//	Type  operator++( int )
//	{
//		auto old = *this;
//		++( *this );
//		return old;
//	}
//	Type  operator--( int )
//	{
//		auto old = *this;
//		--( *this );
//		return old;
//	}
//
//	Type& operator+=( difference_type _index )
//	{
//		( *this )[ current_idx() + _index ];
//		return *this;
//	}
//	Type& operator-=( difference_type _index )
//	{
//		( *this )[ current_idx() - _index ];
//		return *this;
//	}
//	Type operator+( difference_type _index )const
//	{
//		return Type( *this ) += _index;
//	}
//	Type operator-( difference_type _index )const
//	{
//		return Type( *this ) -= _index;
//	}
//
//	difference_type operator-( Type& _other )const
//	{
//		return ptr - _other.ptr;
//	}
//
//private:
//	difference_type current_idx()const
//	{
//		return ( *this ) - cont->begin();
//	}
//private:
//	container* cont = nullptr;
//	pointer ptr = nullptr;
//};

template<typename...CollectionTypes>
class Collection
{
public:
	using variant_t = Variant<CollectionTypes...>;

	using iterator = CollectionIterator<CollectionTypes...>;
	using const_iterator = ConstCollectionIterator<CollectionTypes...>;

	using value_type = Variant<CollectionTypes...>;

	using pointer = Variant<CollectionTypes...> *;
	using const_pointer = const Variant<CollectionTypes...>*;

	using reference = Variant<CollectionTypes...> &;
	using const_reference = const Variant<CollectionTypes...>&;

	using size_type = std::size_t;

	reference front()
	{
		return *begin();
	}
	const_reference front()const
	{
		return *begin();
	}
	reference back()
	{
		return *( end() - 1 );
	}
	const_reference back()const
	{
		return *( end() - 1 );
	}

	iterator begin()
	{
		return iterator( this, &variants[ 0 ] );
	}
	const_iterator begin()const
	{
		return const_iterator( this, &variants[ 0 ] );
	}
	iterator end()
	{
		return iterator( this, &variants[ variants.size() - 1 ] );
	}
	const_iterator end()const
	{
		return const_iterator( this, &variants[ variants.size() - 1 ] );
	}

	const_iterator cbegin()const
	{
		return begin();
	}
	const_iterator cend()const
	{
		return end();
	}

	template<typename T, typename...Args>
	void AddVariant( Args&&... _args )
	{
		variants.emplace_back( variant_t{ std::forward<Args>( _args )... } )
	}
	template<typename T>
	void AddVariant( T&& _value )
	{
		variants.push_back( variant_t{ std::move( _value ) } );
	}

	template<typename T>
	void RemoveMatchingVariant( const T& _value )
	{
		auto findit = std::find_if( begin(), end(),
			[ & ]( const variant_t& _element )
			{
				return std::addressof( _element ) == std::addressof( _value );
			} );

		if( findit != end() )
			variants.erase( findit );
	}
	void RemoveElement( const_iterator _which )
	{
		const auto idx = _which - cbegin();
		variants.erase( variants.begin() + idx );
	}
	void RemoveAll()
	{
		variants.clear();
	}
	template<typename T>
	void RemoveMatchingTypes()
	{
		auto remit =
			std::remove_if( begin(), end(), []( const variant_t& _element )
				{
					return _element.IsActiveType<T>();
				} );

		variants.erase( remit, end() );
	}
	template<typename T>
	void RemoveNotMatchingTypes()
	{
		auto remit =
			std::remove_if( begin(), end(), []( const variant_t& _element )
				{
					return !_element.IsActiveType<T>();
				} );

		variants.erase( remit, end() );
	}

	void SwapAndPop( iterator _iter )
	{
		if( IsValidIterator( _iter ) )
		{
			if( variants.size() == 1 )
			{
				RemoveAll();
			}
			else
			{
				auto iter = variants.begin() + ( _iter - end() );

				iter->SwapWith( back() );
				RemoveElement( cend() - 1 );
			}
		}
	}
	template<typename Pred>
	void SwapAndPop( iterator _iter, Pred&& _condition )
	{
		if( _condition( *_iter ) )
		{
			SwapAndPop( _iter );
		}
	}

	template<typename T>
	iterator FindVariant()noexcept
	{
		if constexpr( has_type<T, CollectionTypes...>() )
		{
			auto findit = std::find_if(
				variants.begin(), variants.end(), []( const auto& _object )
				{
					using object_t =
						std::decay_t<decltype( _object )>;
					if constexpr( std::is_pointer_v<object_t> )
					{
						return _object->IsActiveType<T>();
					}
					else
					{
						return _object.IsActiveType<T>();
					}
				}
			);
			const auto offset = findit - variants.begin();
			return begin() + offset;
		}

		return end();
	}
	template<typename T>
	const_iterator FindVariant()const noexcept
	{
		if constexpr( has_type<T, CollectionTypes...>() )
		{
			auto findit = std::find_if(
				variants.begin(), variants.end(), std::holds_alternative<T>
			);
			const auto offset = findit - variants.begin();
			return begin() + offset;
		}

		return end();
	}
	template<typename T>
	iterator FindVariant( const T& _value )noexcept
	{
		auto findit = std::find_if( begin(), end(),
			[ & ]( const variant_t& _element )
			{
				return std::addressof( _element ) == std::addressof( _value );
			} );
		const auto offset = findit - variants.begin();
		return begin() + offset;
	}
	template<typename T>
	const_iterator FindVariant( const T& _value )const noexcept
	{
		auto findit = std::find_if( begin(), end(),
			[ & ]( const variant_t& _element )
			{
				return std::addressof( _element ) == std::addressof( _value );
			} );
		const auto offset = findit - variants.begin();
		return begin() + offset;
	}

	bool IsValidIterator( iterator _iter )
	{
		return _iter != end();
	}
	bool IsValidIterator( const_iterator _iter )const
	{
		return _iter != end();
	}

	size_type size()const
	{
		return variants.size();
	}
	bool empty()const
	{
		return variants.empty();
	}
private:
	std::vector<variant_t> variants;
};
