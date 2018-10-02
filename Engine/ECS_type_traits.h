#pragma once

#include <type_traits>

template<typename Ty>
struct is_intrinsic_type
{
	static constexpr bool value = std::disjunction_v<
		std::conditional_t<std::is_same_v<Ty, bool>, std::true_type, std::false_type>,
		std::conditional_t<std::is_same_v<std::make_signed_t<Ty>, char>, std::true_type, std::false_type>,
		std::conditional_t<std::is_same_v<std::make_signed_t<Ty>, short>, std::true_type, std::false_type>,
		std::conditional_t<std::is_same_v<std::make_signed_t<Ty>, int>, std::true_type, std::false_type>,
		std::conditional_t<std::is_same_v<std::make_signed_t<Ty>, long>, std::true_type, std::false_type>,
		std::conditional_t<std::is_floating_point_v<Ty>, std::true_type, std::false_type>
	>;
};

template<typename Ty>
constexpr bool is_intrinsic_type_v = is_intrinsic_type<Ty>::value;

template<typename Ty> struct remove_cv_ref
{
	using type = Ty;
};
template<typename Ty> struct remove_cv_ref<const volatile Ty>
{
	using type = std::remove_cv_t<Ty>;
};
template<typename Ty> struct remove_cv_ref<const Ty>
{
	using type = std::remove_const_t<Ty>;
};
template<typename Ty> struct remove_cv_ref<volatile Ty>
{
	using type = std::remove_volatile_t<Ty>;
};
template<typename Ty> struct remove_cv_ref<const volatile Ty&>
{
	using type = std::remove_reference_t<std::remove_cv_t<Ty>>;
};
template<typename Ty> struct remove_cv_ref<const Ty&>
{
	using type = std::remove_reference_t<std::remove_const_t<Ty>>;
};
template<typename Ty> struct remove_cv_ref<volatile Ty&>
{
	using type = std::remove_volatile_t<std::remove_reference_t<Ty>>;
};
template<typename Ty> struct remove_cv_ref<Ty&>
{
	using type = std::remove_reference_t<Ty>;
};

template<typename Ty> using remove_cv_ref_t = typename remove_cv_ref<Ty>::type;

template<typename T, typename U>
struct is_similar
{
	static constexpr bool value = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;
};

template<typename T, typename U>
constexpr bool is_similar_v = is_similar<T, U>::value;

template<typename T, typename U, typename...Rest>
constexpr bool has_type()noexcept
{
	bool hasType = is_similar_v<T, U>;
	if constexpr( sizeof...( Rest ) > 0 )
	{
		hasType |= has_type<T, Rest...>();
	}
	return hasType;
}

template<size_t idx, typename T, typename U, typename...Rest>
constexpr size_t get_index_varified()noexcept
{
	if constexpr( std::is_same_v<std::decay_t<T>, std::decay_t<U>> )
	{
		return idx;
	}
	if constexpr( sizeof...( Rest ) > 0 )
	{
		return get_index_varified<idx + 1, T, Rest...>();
	}
}

template<size_t idx, typename T, typename U, typename...Rest>
constexpr size_t get_index()noexcept
{
	if constexpr( !has_type<T, U, Rest...>() )
	{
		return -1;
	}
	return get_index_varified<idx, T, U, Rest...>();
}

template<typename CompareTo, typename ListType> struct find_index;
template<typename CompareTo, typename...Types>
struct find_index<CompareTo, std::variant<Types...>>
{
	static constexpr bool values[] = { std::is_same_v<CompareTo,Types>... };
	static constexpr size_t numValues = sizeof( values );
	template<size_t idx = 0>
	static constexpr size_t _index()
	{
		constexpr size_t next = idx + 1;
		if constexpr( values[ idx ] ) return idx;
		if constexpr( next < numValues )
		{
			return _index<next>();
		}

		return std::size_t( -1 );
	}
public:
	static constexpr size_t index = _index();

};

//template<typename T, typename First, typename...Rest>
//struct has_required : has_required<T, Rest...>
//{
//	static constexpr bool value = has_type<T, First, Rest...>();
//};

template<typename T, typename U, typename...Types>
struct has_required
{
	static constexpr bool has()
	{
		if constexpr( std::is_same_v<T, U> )
			return true;
		else if constexpr( sizeof...( Types ) == 0 )
			return false;
		else
			return has_required<T, Types...>::has();
	}
	static constexpr bool value = has();
};

template<typename T, typename...Types>
constexpr bool has_required_v = has_required<T, Types...>::value;

