#pragma once
#include <Windows.h>
#include <mutex>

#include "backend.hpp"

namespace Hooks {
	void Init( );
	void Free( );
	void FindRenderer();
	bool FoundRenderer();

	inline std::mutex FoundMutex;
	inline bool bShuttingDown;
#ifndef FORCE_BACKEND
	inline bool bFoundRenderer = false;
#else
	inline bool bFoundRenderer = true;
#endif
}

namespace H = Hooks;
