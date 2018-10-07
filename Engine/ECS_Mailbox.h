#pragma once

#include "ECS_Receiver.h"
#include "ECS_Sender.h"
#include "ECS_Utilities.h"

namespace screws
{
	struct send_recv {};
	struct sender_only {};

	template<typename MessageVariant, typename MsgFilter, typename Type = screws::send_recv>
	class ECS_Mailbox
	{
	public:
		using receiver_type = ECS_Receiver<MessageVariant>;
		using sender_type = ECS_Sender<MessageVariant>;
	public:
		ECS_Mailbox() = default;

		ECS_Mailbox(
			shared_resource<receiver_type> _receiver,
			shared_resource<sender_type> _sender )
			:
			receiver( std::move( _receiver ) ),
			sender( std::move( _sender ) )
		{
		}
		/*template<typename = std::enable_if_t<std::is_same_v<Type,screws::sender_only>,int> = 0
		ECS_Mailbox( shared_resource<sender_type> _sender )
			:
			sender( std::move( _sender ) )
		{
		}*/

		template<typename MessageType>
		void receive_message( MessageType _message )
		{
			if constexpr( std::is_same_v<MsgFilter, ECS_NullMessageFilter> ) return;
			if constexpr( !has_required_v<MessageType, MsgFilter> ) return;

			receiver->on_receive( std::move( _message ) );
		}

		template<typename MessageType, typename...Args>
		void send_message( Args&&... _args )
		{
			sender->send<MessageType>( std::forward<Args>( _args )... );
		}

		void add_receiver( shared_resource<receiver_type> _receiver )
		{
			sender->add_receiver( _receiver );
		}
		shared_resource<receiver_type> get_receiver()const { return receiver; }

	protected:
		shared_resource<receiver_type> receiver;
		shared_resource<sender_type> sender;
	};

	template<typename MessageVariant, typename MsgFilter>
	ECS_Mailbox(
		shared_resource<ECS_Receiver<MessageVariant>>,
		shared_resource<ECS_Sender<MessageVariant>> )->
		ECS_Mailbox<MessageVariant, MsgFilter>;



	template<typename T, typename...Types>
	struct has_required<T, ECS_MessageFilter<Types...>>
	{
	public:
		static constexpr bool value = has_required<T, Types...>::has();
	};
}
