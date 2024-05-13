#pragma once
#include <mutex>

struct ImFont;

class OverlayBase
{
public:
	static OverlayBase* Instance;

	void HookReady();
	void CreateFonts();
	void OverlayProc();

	bool ShowOverlay() const { return bShowOverlay; }
	void ShowOverlay(bool bShow);

	void SetupOverlay();
	bool IsReady() const { return bIsReady; }

protected:
	// Called always - use to draw an HUD
	virtual void DrawHUD() = 0;
	// Called only when overlay is active
	virtual void DrawOverlay() = 0;


protected:
	std::recursive_mutex OverlayMutex;
	bool bSetupOverlayCalled;
	bool bIsReady;
	bool bShowOverlay;
	ImFont* FontDefault;
	ImFont* FontHUD;
};