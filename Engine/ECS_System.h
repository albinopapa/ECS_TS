#pragma once

#include "ECS_Algorithms.h"
#include "ECS_Mailbox.h"
#include "ECS_Utilities.h"


namespace screws
{
	template<
		typename ExecuteDispatch,
		typename MessageDispatch,
		typename EntityVariant, 
		typename MessageVariant, 
		typename MessageFilter>
	class ECS_System :
		public ECS_Mailbox<MessageVariant, MessageFilter>
	{
	public:
		using entity_resource = shared_resource<EntityVariant>;

		using iterator = typename std::vector<entity_resource>::iterator;
		using const_iterator = typename std::vector<entity_resource>::const_iterator;

	public:
		void add_entity( entity_resource _entity )
		{
			entities.push_back( std::move( _entity ) );
		}

		void remove_entity( const entity_resource& _entity )
		{
			if( auto it = find_if( entities, is_same_resource( _entity ) ); it != entities.end() )
			{
				entities.erase( it );
			}
		}

		void execute( MessageDispatch _msg_visitor, ExecuteDispatch _execute_visitor )
		{
			process_messages( _msg_visitor );
			for( auto& ventity : entities )
			{
				std::visit( _execute_visitor, *ventity );
			}
		}

		bool has_entity( const entity_resource& _entity )const
		{
			return ( find_entity( _entity ) != entities.end() );
		}

		iterator find_entity( const entity_resource& _entity )
		{
			return find_if( entities, is_same_resource<entity_resource>( _entity ) );
		}

		const_iterator find_entity( const entity_resource& _entity )const
		{
			return find_if( entities, is_same_resource<entity_resource>( _entity ) );
		}

		void set_mailbox( shared_resource<ECS_Receiver<MessageVariant>> _receiver,
			shared_resource<ECS_Sender<MessageVariant>> _sender )
		{
			this->receiver = _receiver;
			this->sender = _sender;
		}
	private:
		void process_messages( MessageDispatch& _visitor )
		{
			for( std::optional<MessageVariant> result = receiver->get_next_message();
				result.has_value();
				result = receiver->get_next_message() )
			{
				std::visit( _visitor, result.value() );
			}
		}
		friend ExecuteDispatch;
		friend MessageDispatch;
	protected:
		std::vector<entity_resource> entities;
	};
}
