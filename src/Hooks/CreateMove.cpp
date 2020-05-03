#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Hacks/grenadehelper.h"
#include "../Hacks/edgejump.h"
#include "../Hacks/autoblock.h"
#include "../Hacks/predictionsystem.h"
#include "../Hacks/aimbot.h"
#include "../Hacks/triggerbot.h"
#include "../Hacks/antiaim.h"
#include "../Hacks/esp.h"

bool CreateMove::sendPacket = true;
QAngle CreateMove::lastTickViewAngles = QAngle(0, 0, 0);

typedef bool (*CreateMoveFn) (void*, float, CUserCmd*);

bool Hooks::CreateMove(void* thisptr, float flInputSampleTime, CUserCmd* cmd)
{
	clientModeVMT->GetOriginalMethod<CreateMoveFn>(25)(thisptr, flInputSampleTime, cmd);

	if (cmd && cmd->command_number)
	{
        uintptr_t rbp;
        asm volatile("mov %%rbp, %0" : "=r" (rbp));
        bool *sendPacket = ((*(bool **)rbp) - 0x18);
        CreateMove::sendPacket = true;

		GrenadeHelper::CreateMove(cmd);
        EdgeJump::PrePredictionCreateMove(cmd);
		Autoblock::CreateMove(cmd);

		PredictionSystem::StartPrediction(cmd);
			Aimbot::CreateMove(cmd);
			Triggerbot::CreateMove(cmd);
            AntiAim::CreateMove(cmd);
			ESP::CreateMove(cmd);
		PredictionSystem::EndPrediction();

		EdgeJump::PostPredictionCreateMove(cmd);

        *sendPacket = CreateMove::sendPacket;

        if (CreateMove::sendPacket) {
            CreateMove::lastTickViewAngles = cmd->viewangles;
        }
	}

	return false;
}
