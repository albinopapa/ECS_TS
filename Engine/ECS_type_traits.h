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
