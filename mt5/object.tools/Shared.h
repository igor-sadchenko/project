#pragma once

namespace manager {
	class Shared final
	{
	public:
		Shared() {}
		~Shared() {}
	public:
		sk::container::queue<shared::RequestField> m_RequestQ;
		sk::container::map<UINT, shared::RequestField> m_RequestCacheQ;

		sk::container::map<UINT64, shared::UserField> m_MTUsersQ;
	};
}///namespace manager
