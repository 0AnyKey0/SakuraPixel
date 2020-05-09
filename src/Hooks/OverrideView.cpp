#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Hacks/thirdperson.h"

float OverrideView::currentFOV = 90.0f;

typedef void (*OverrideViewFn) (void*, CViewSetup*);

void Hooks::OverrideView(void* thisptr, CViewSetup* pSetup)
{
	if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot())
	{
		ThirdPerson::OverrideView(pSetup);
	}

	OverrideView::currentFOV = pSetup->fov;

	clientModeVMT->GetOriginalMethod<OverrideViewFn>(19)(thisptr, pSetup);
}
