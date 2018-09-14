#pragma once

#include <cmath>

namespace chili
{
	template<typename T>
	constexpr auto isnan( T t )noexcept->
		std::enable_if_t<std::is_floating_point_v<T>, bool>;

	template<typename T>
	constexpr bool signbit( T t )noexcept;

	template<typename T>
	constexpr auto abs( const T _value )noexcept->
		std::enable_if_t<std::is_arithmetic_v<T>, T>;

	template<typename T>
	constexpr auto floor( const T _value )noexcept->
		std::enable_if_t<std::is_floating_point_v<T>, T>;

	template<typename T>
	constexpr auto isinfinite( const T _value )noexcept->
		std::enable_if_t<std::is_floating_point_v<T>, bool>;

	namespace details
	{
		static constexpr float sinangles[]{
			0.00000000000f,	//   0.0 degrees, 0
			0.08715574274f, //   5.0 degrees, 1
			0.17364817766f, //  10.0 degrees, 2
			0.25881904510f, //  15.0 degrees, 3
			0.34202014332f, //  20.0 degrees, 4
			0.42261826174f,	//  25.0 degrees, 5
			0.50000000000f,	//  30.0 degrees, 6
			0.57357643635f,	//  35.0 degrees, 7
			0.64278760968f,	//  40.0 degrees, 8
			0.70710678118f,	//  45.0 degrees, 9
			0.76604444311f,	//  50.0 degrees, 10 
			0.81915204428f,	//  55.0 degrees, 11
			0.86602540378f,	//  60.0 degrees, 12
			0.90630778703f,	//  65.0 degrees, 13
			0.93969262078f,	//  70.0 degrees, 14
			0.96592582628f,	//  75.0 degrees, 15
			0.98480775301f,	//  80.0 degrees, 16
			0.99619469809f,	//  85.0 degrees, 17
			1.00000000000f,	//  90.0 degrees, 18
			0.99619469809f, //  95.0 degrees, 19
			0.98480775301f, // 100.0 degrees, 20
			0.96592582628f, // 105.0 degrees, 21
			0.93969262078f, // 110.0 degrees, 22
			0.90630778703f, // 115.0 degrees, 23
			0.86602540378f, // 120.0 degrees, 24
			0.81915204428f, // 125.0 degrees, 25
			0.76604444311f, // 130.0 degrees, 26
			0.70710678118f, // 135.0 degrees, 27
			0.64278760968f, // 140.0 degrees, 28 
			0.57357643635f, // 145.0 degrees, 29
			0.50000000000f, // 150.0 degrees, 30
			0.42261826174f, // 155.0 degrees, 31
			0.34202014332f, // 160.0 degrees, 32
			0.25881904510f, // 165.0 degrees, 33
			0.17364817766f, // 170.0 degrees, 34
			0.08715574274f	// 175.0 degrees, 35
		};
		static constexpr float cosangles[]{
			sinangles[ 18 ],  //   0.0 degrees, 0
			sinangles[ 17 ],  //   5.0 degrees, 1
			sinangles[ 16 ],  //  10.0 degrees, 2
			sinangles[ 15 ],  //  15.0 degrees, 3
			sinangles[ 14 ],  //  20.0 degrees, 4
			sinangles[ 13 ],  //  25.0 degrees, 5
			sinangles[ 12 ],  //  30.0 degrees, 6
			sinangles[ 11 ],  //  35.0 degrees, 7
			sinangles[ 10 ],  //  40.0 degrees, 8
			sinangles[ 9 ],	  //  45.0 degrees, 9
			sinangles[ 8 ],	  //  50.0 degrees, 10
			sinangles[ 7 ],	  //  55.0 degrees, 11
			sinangles[ 6 ],	  //  60.0 degrees, 12
			sinangles[ 5 ],	  //  65.0 degrees, 13
			sinangles[ 4 ],	  //  70.0 degrees, 14
			sinangles[ 3 ],	  //  75.0 degrees, 15
			sinangles[ 2 ],	  //  80.0 degrees, 16
			sinangles[ 1 ],	  //  85.0 degrees, 17
			sinangles[ 0 ],	  //  90.0 degrees, 18
			-sinangles[ 35 ], //  95.0 degrees, 19
			-sinangles[ 34 ], // 100.0 degrees, 20
			-sinangles[ 33 ], // 105.0 degrees, 21
			-sinangles[ 32 ], // 110.0 degrees, 22
			-sinangles[ 31 ], // 115.0 degrees, 23
			-sinangles[ 30 ], // 120.0 degrees, 24
			-sinangles[ 29 ], // 125.0 degrees, 25
			-sinangles[ 28 ], // 130.0 degrees, 26
			-sinangles[ 27 ], // 135.0 degrees, 27
			-sinangles[ 26 ], // 140.0 degrees, 28
			-sinangles[ 25 ], // 145.0 degrees, 29
			-sinangles[ 24 ], // 150.0 degrees, 30
			-sinangles[ 23 ], // 155.0 degrees, 31
			-sinangles[ 22 ], // 160.0 degrees, 32
			-sinangles[ 21 ], // 165.0 degrees, 33
			-sinangles[ 20 ], // 170.0 degrees, 34
			-sinangles[ 19 ]  // 175.0 degrees, 35
		};
		enum class Dir { up, down };

		template<size_t N>
		struct _InterpolateParams
		{
			const float( &arr )[ N ];
			int index1 = 0, index2 = 0;
			Dir dir1 = Dir::down, dir2 = Dir::down;
			float t = 0.f;
			bool is_neg = false;
		};
		template<size_t N>
		std::pair<float,int> _calculate_t_and_index( const float( &_arr )[ N ], const float _radian )noexcept
		{
			auto zero_if = []( float _value )
			{
				return isinfinite( _value ) ? 0.f : _value;
			};
			
			const float absdeg = abs( to_degrees( _radian ) );
			const float moddeg = std::fmodf( absdeg, 360.f );
			const float absdeg2 = to_radians( floor( ( absdeg + 4.f ) / 5.f ) * 5.f );
			const float absdeg3 = to_radians( floor( moddeg / 5 ) * 5 );

			const float t = zero_if( ( _radian - absdeg3 ) / ( absdeg2 - absdeg3 ) );

			int index = int( moddeg / 5.f );
			return { t,index };
		}
		template<size_t N>
		float _interpolate_value( const _InterpolateParams<N>& _params )noexcept
		{
			const auto val1 = ( _params.dir1 == Dir::down ) ?
				_params.arr[ _params.index1 ] : -_params.arr[ _params.index1 ];
			const auto val2 = ( _params.dir2 == Dir::down ) ?
				_params.arr[ _params.index2 ] : -_params.arr[ _params.index2 ];

			// Interpolate between the two nearest angles
			const auto angle1 = _params.is_neg ? -val1 : val1;
			const auto angle2 = _params.is_neg ? -val2 : val2;

			const auto result = angle1 + ( ( angle2 - angle1 ) * _params.t );

			return result;
		}
	}

	constexpr float pi = 3.1415926f;
	constexpr float deg2rad = pi / 180.f;
	constexpr float rad2deg = 180.f / pi;

	constexpr float to_radians( float _angle )noexcept { return _angle * deg2rad; }
	constexpr float to_degrees( float _angle )noexcept { return _angle * rad2deg; }

	template<typename T, size_t N>
	constexpr size_t array_size( const T( &_array )[ N ] )noexcept { return N; }
	template<typename T> 
	constexpr T sq( T _value )noexcept { return _value * _value; }

	template<typename T>
	constexpr auto isnan( T t )noexcept->std::enable_if_t<std::is_floating_point_v<T>, bool>
	{
		return t != t;
	}

	template<typename T>
	constexpr bool signbit( T t )noexcept
	{
		if constexpr( std::is_unsigned_v<T> ) return false;
		return t < T( 0 );
	}

	template<typename T>
	constexpr auto abs( const T _value )noexcept->
		std::enable_if_t<std::is_arithmetic_v<T>, T> 
	{
		return _value < T( 0 ) ? _value * -1.f : _value;
	}

	template<typename T>
	constexpr auto floor( const T _value )noexcept->
		std::enable_if_t<std::is_floating_point_v<T>, T>
	{
		const auto isneg = signbit( _value );
		const auto absvalue = abs( _value ) + T( isneg ? 1 : 0 );
		const auto value = static_cast< T >( static_cast< int >( absvalue ) );
		return isneg ? -value : value;
	}
	template<typename T>
	constexpr auto ceil( const T _value )noexcept->
		std::enable_if_t<std::is_floating_point_v<T>, T>
	{
		const auto returnIdentity =
			( isinfinite( _value ) || isnan( _value ) || ( _value == 0.f ) );

		return returnIdentity ? _value : floor( _value ) + T( 1 );
	}
	template<typename T>
	constexpr auto isinfinite( const T _value )noexcept->
		std::enable_if_t<std::is_floating_point_v<T>, bool>
	{
		return std::numeric_limits<T>::infinity() == _value;
	}

	inline float sin( float _radian )noexcept
	{
		constexpr auto numElements = int( array_size( details::sinangles ) );
		auto[ t, _index ] = details::_calculate_t_and_index( details::sinangles, _radian );
		const bool is_neg = _radian < 0.f;

		const details::Dir dir1 = 
			_index < numElements ? details::Dir::down : details::Dir::up;
		const details::Dir dir2 = 
			_index + 1 < numElements ? details::Dir::down : details::Dir::up;
		
		details::_InterpolateParams<numElements> params
		{
			details::sinangles,
			_index % numElements,
			( _index + 1 ) % numElements,
			dir1,
			dir2,
			t,
			is_neg
		};

		return details::_interpolate_value( params );
	}
	inline float cos( float _radian )noexcept
	{
		constexpr auto numElements = int( array_size( details::cosangles ) );
		auto[ t, _index ] = details::_calculate_t_and_index( details::cosangles, _radian );
		const bool is_neg = _radian < 0.f;

		const details::Dir dir1 = 
			_index < numElements ? details::Dir::down : details::Dir::up;

		const details::Dir dir2 = 
			_index + 1 < numElements ? details::Dir::down :
			( _index + 1 ) / numElements == 1 ? details::Dir::up : details::Dir::down;

		details::_InterpolateParams<numElements> params
		{
			details::cosangles,
			_index % numElements,
			( _index + 1 ) % numElements,
			dir1,
			dir2,
			t,
			is_neg
		};

		return details::_interpolate_value( params );
	}
}