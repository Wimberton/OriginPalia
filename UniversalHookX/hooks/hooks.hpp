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
#ifdef FORCE_BACKEND
	inline bool bFoundRenderer = true;
#else
	inline bool bFoundRenderer = false;
#endif
}

namespace H = Hooks;
