#include "hooks.h"

#include "../Utils/skins.h"
#include "../interfaces.h"

#include "../Hacks/disablepostprocessing.h"

typedef void (*BeginFrameFn) (void*, float);

void Hooks::BeginFrame(void* thisptr, float frameTime)
{
	DisablePostProcessing::BeginFrame();

	if (!engine->IsInGame())
		CreateMove::sendPacket = true;

	Skins::Localize();

	return materialVMT->GetOriginalMethod<BeginFrameFn>(42)(thisptr, frameTime);
}
