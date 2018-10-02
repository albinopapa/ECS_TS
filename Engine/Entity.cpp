#include "Entity.h"

Player::Player( Factory & _factory )
	:
	ECS_Entity<component_t>( Position( 400.f, 300.f ), Velocity( 0.f, 0.f ), Health( 100.f ), Shape( Circle{ Vec2{ 0.f,0.f }, 20.f }, Colors::Blue ) )
{
	
}

Enemy::Enemy( Factory & _factory )
	:
	ECS_Entity<component_t>( Position( 600.f, 300.f ), Velocity( 0.f, 0.f ), Health( 100.f ), Shape( Circle{ Vec2{ 0.f,0.f }, 20.f }, Colors::Red ) )
{
}
