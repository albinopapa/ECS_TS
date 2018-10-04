#pragma once

#include "ECS_Receiver.h"
#include "ECS_Sender.h"
#include "ECS_Utilities.h"

namespace screws
{
	template<typename MessageVariant, typename MsgFilter>
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

		template<typename MessageType>
		void receive_message( MessageType _message )
		{
			if constexpr( has_required_v<MessageType, MsgFilter> )
			{
				receiver->receive( std::move( _message ) );
			}
		}

		void add_receiver( shared_resource<receiver_type> _receiver )
		{
			sender->add_receiver( _receiver );
		}
		shared_resource<receiver_type> get_receiver()const { return receiver; }
	protected:
		template<typename MessageType, typename...MsgArgs>
		void send_message( MsgArgs&&... _args )
		{
			sender->send<MessageType>( std::forward<MsgArgs>( _args )... );
		}

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
