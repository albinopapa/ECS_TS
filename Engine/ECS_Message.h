#pragma once

#include "ECS_Utilities.h"

namespace screws
{
	template<typename...MessageTypes> struct ECS_Message {};

	template<typename...msg_filter_list>
	struct ECS_MessageFilter
	{
		using type = ECS_MessageFilter<msg_filter_list...>;
	};

	template<>struct ECS_MessageFilter<std::nullptr_t> {};

	using ECS_NullMessageFilter = ECS_MessageFilter<std::nullptr_t>;

}
