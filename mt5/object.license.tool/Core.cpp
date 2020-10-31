#include "stdafx.h"
#include "Core.h"

namespace local {
	Core::Core() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	Core::~Core() {
		UnInit();
	}

	int Core::Init() {

		return 0;
	}
	int Core::UnInit() {


		return 0;
	}

	int Core::Open() {
		if (m_IsOpen.load()) { return 0; }



		m_IsOpen.store(true);
		return 0;
	}
	int Core::Close() {
		if (!m_IsOpen.load()) { return 0; }



		m_IsOpen.store(false);
		return 0;
	}

}///namespace local