#include "SystemControllable.h"
#include "Component.h"
#include "Message.h"
#include "ChiliWin.h"

ControllableExecuteDispatch::ControllableExecuteDispatch( Controllable& _parent, Keyboard& _keyboard )
	:
	parent( _parent ),
	kbd( _keyboard )
{
}

//void ControllableExecuteDispatch::operator()( Player& _entity )
//{
//	while( !kbd.KeyIsEmpty() )
//	{
//		const auto key_event = kbd.ReadKey();
//		if( key_event.IsPress() )
//		{
//			auto vel_option = _entity.get_component<Velocity>();
//			if(vel_option.has_value())
//			{
//				vel_option.value().get() = Velocity(
//					key_event.GetCode() == VK_LEFT ? -100.f : 100.f,
//					key_event.GetCode() == VK_UP ? -100.f : 100.f );
//			}
//		}
//	}
//}

ControllableMessageDispatch::ControllableMessageDispatch( Controllable& _parent )
	:
	SystemMessageDispatchBase<Controllable, Velocity>( _parent )
{
}
/*void ControllableMessageDispatch::operator()( const ComponentAdded& _message )
{

}
void ControllableMessageDispatch::operator()( const ComponentRemoved& _message )
{

}
void ControllableMessageDispatch::operator()( const EntityAdded& _message )
{

}
void ControllableMessageDispatch::operator()( const EntityRemoved& _message )
{

}*/
void ControllableMessageDispatch::operator()( KeyPress& _message )
{
	std::visit( [ & ]( auto& _entity )
		{
			auto vel_option = _entity.get_component<Velocity>();
			if( vel_option.has_value() )
			{
				Velocity vel = vel_option.value().get();
				vel_option.value().get() = Velocity(
					_message.key_event.GetCode() == VK_LEFT ? -100.f :
					_message.key_event.GetCode() == VK_RIGHT ? 100.f : vel.value.x,
					_message.key_event.GetCode() == VK_UP ? -100.f :
					_message.key_event.GetCode() == VK_DOWN ? 100.f : vel.value.y );
			}
		}, *_message.entity );
}
void ControllableMessageDispatch::operator()( KeyRelease& _message )
{
	std::visit( [ & ]( auto& _entity )
		{
			auto vel_option = _entity.get_component<Velocity>();
			if( vel_option.has_value() )
			{
				Velocity vel = vel_option.value().get();
				vel_option.value().get() = Velocity(
					_message.key_event.GetCode() == VK_LEFT ||
					_message.key_event.GetCode() == VK_RIGHT ? 0.f : vel.value.x,
					_message.key_event.GetCode() == VK_UP ||
					_message.key_event.GetCode() == VK_DOWN ? 0.f : vel.value.y
				);
			}
		}, *_message.entity );
}
