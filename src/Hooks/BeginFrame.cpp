#include "hooks.h"

#include "../Utils/skins.h"
#include "../interfaces.h"


typedef void (*BeginFrameFn) (void*, float);

void Hooks::BeginFrame(void* thisptr, float frameTime)
{

	if (!engine->IsInGame())
		CreateMove::sendPacket = true;

	Skins::Localize();

	return materialVMT->GetOriginalMethod<BeginFrameFn>(42)(thisptr, frameTime);
}
