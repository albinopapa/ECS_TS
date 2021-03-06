/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Vec2.h																				  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include "ChiliMath.h"

template <typename T> class _Vec2;
template<typename T> T dot_product( const _Vec2<T>& _left, const _Vec2<T>& _right );

template <typename T>
class _Vec2
{
public:
	constexpr _Vec2()noexcept = default;
	constexpr _Vec2( T x,T y )noexcept
		:
		x( x ),
		y( y )
	{}
	constexpr _Vec2( const _Vec2& vect )noexcept
		:
		_Vec2( vect.x,vect.y )
	{}
	template <typename T2>
	constexpr explicit operator _Vec2<T2>() const noexcept
	{
		return{ (T2)x,(T2)y };
	}
	constexpr T		LenSq() const noexcept
	{
		return dot_product( *this, *this );
	}
	T		Len() const noexcept
	{
		return sqrt( LenSq() );
	}
	_Vec2&	Normalize()noexcept
	{
		const T length_sq = LenSq();
		if( length_sq == T( 0 ) ) return *this;

		if constexpr( std::is_floating_point_v<T> )
		{
			const T length = T( 1 ) / std::sqrt( length_sq );
			x *= length;
			y *= length;
		}
		else
		{
			const T length = std::sqrt( length_sq );
			x /= length;
			y /= length;
		}

		return *this;
	}
	_Vec2	Normalize() const noexcept
	{
		return _Vec2( *this ).Normalize();
	}
	constexpr _Vec2	operator-() const noexcept
	{
		return _Vec2( -x,-y );
	}
	_Vec2&	operator+=( const _Vec2 &rhs )noexcept
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	_Vec2&	operator-=( const _Vec2 &rhs )noexcept
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	constexpr _Vec2	operator+( const _Vec2 &rhs ) const noexcept
	{
		return _Vec2( rhs.x + x, rhs.y + y );
	}
	constexpr _Vec2	operator-( const _Vec2 &rhs ) const noexcept
	{
		return _Vec2( x - rhs.x, y - rhs.y );
	}
	_Vec2&	operator*=( const T &rhs )noexcept
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}
	constexpr _Vec2	operator*( const T &rhs ) const noexcept
	{
		return { x * rhs, y * rhs };
	}
	_Vec2&	operator/=( const T &rhs )noexcept
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}
	constexpr _Vec2	operator/( const T &rhs ) const noexcept
	{
		return { x / rhs,y / rhs };
	}
	constexpr bool	operator==( const _Vec2 &rhs ) const noexcept
	{
		return x == rhs.x && y == rhs.y;
	}
	constexpr bool	operator!=( const _Vec2 &rhs ) const noexcept
	{
		return !(*this == rhs);
	}
public:
	T x;
	T y;
};

template<typename T>
T dot_product( const _Vec2<T>& _left, const _Vec2<T>& _right )
{
	return ( _left.x * _right.x ) + ( _left.y * _right.y );
}
template<typename T>
_Vec2<T> reflect( const _Vec2<T>& _direction, const _Vec2<T>& _normal )
{
	return _direction - ( _normal * ( dot_product( _direction, _normal ) * 2.f ) );
}
typedef _Vec2<float> Vec2;
typedef _Vec2<double> Ved2;
typedef _Vec2<int> Vei2;