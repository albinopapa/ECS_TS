#pragma once

#include "ECS_Utilities.h"
#include "Sender.h"
#include "Receiver.h"


class ECS_PostOffice
{
public:
	template<typename...msg_filter_list>
	struct ECS_MessageFilter 
	{
		using type = ECS_MessageFilter<msg_filter_list>;
	};

	template<typename msg_filter>
	class ECS_MailBox
	{
	public:
		ECS_MailBox(
			shared_resource<Receiver> _receiver,
			shared_resource<Sender> _sender,
			ECS_MessageFilter<msg_filter> _filter )
			:
			receiver( std::move( _receiver ) ),
			sender( std::move( _sender ) ),
			filter( _filter )
		{
		}

		template<typename MessageType>
		void receive_message( MessageType _message )
		{
			if( filter( MessageType ) )
			{
				receiver->receive( std::move( _message ) );
			}
		}

		shared_resource<Receiver> get_receiver()const { return receiver; }
	protected:
		template<typename MessageType, typename...MsgArgs>
		void send_message( MsgArgs&&... _args )
		{
			sender->send<MessageType>( std::forward<MsgArgs>( _args )... );
		}

	protected:
		shared_resource<Receiver> receiver;
		shared_resource<Sender> sender;
		ECS_MessageFilter<msg_filter> filter;
	};

public:
	template<typename msg_filter>
	static ECS_MailBox<msg_filter> mailbox_factory()
	{
		return { receiver_factory(),sender_factory() };
	}

private:
	static shared_pool<Receiver>::resource receiver_factory()
	{
		receivers.push_back( Receiver() );
		return receivers.back();
	}
	static shared_pool<Sender>::resource sender_factory()
	{
		senders.push_back( Sender() );
		return senders.back();
	}

private:
	static shared_pool<Receiver> receivers;
	static shared_pool<Sender> senders;
};

shared_pool<Receiver> ECS_PostOffice::receivers;
shared_pool<Sender> ECS_PostOffice::senders;

template<typename msg_filter> using ECS_Mailbox = ECS_PostOffice::ECS_MailBox<msg_filter>;