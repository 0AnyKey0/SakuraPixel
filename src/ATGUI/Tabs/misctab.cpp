#include "misctab.h"

#include <sys/stat.h>
#include <sstream>

#include "../../config.h"
#include "../../interfaces.h"

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../imgui.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static char nickname[127] = "";

void Misc::RenderTab()
{
	const char* strafeTypes[] = { "Forwards", "Backwards", "Left", "Right", "Rage" };
	const char* animationTypes[] = { "Static", "Marquee", "Words", "Letters" };
	const char* teams[] = { "Allies", "Enemies", "Both" };
	const char* grenadeTypes[] = { "FLASH", "SMOKE", "MOLOTOV", "HEGRENADE" };
	const char* throwTypes[] = { "NORMAL", "RUN", "JUMP", "WALK" };
	const char* angleTypes[] = { "Real", "Fake" };

	ImGui::Columns(2, nullptr, true);
	{
		ImGui::BeginChild(XORSTR("Child1"), ImVec2(0, 0), true);
		{
			ImGui::Text(XORSTR("Movement"));
			ImGui::Separator();

			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Edge Jump"), &Settings::EdgeJump::enabled);
			}
			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("FOV"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("FOV"), &Settings::FOVChanger::enabled);
				ImGui::Checkbox(XORSTR("Viewmodel FOV"), &Settings::FOVChanger::viewmodelEnabled);
				ImGui::Checkbox(XORSTR("Ignore Scope"), &Settings::FOVChanger::ignoreScope);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##FOVAMOUNT"), &Settings::FOVChanger::value, 0, 180);
				ImGui::SliderFloat(XORSTR("##MODELFOVAMOUNT"), &Settings::FOVChanger::viewmodelValue, 0, 360);
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Third Person"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Enabled"), &Settings::ThirdPerson::enabled);
			    ImGui::Text(XORSTR("Showed Angle"));
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##TPCAMOFFSET"), &Settings::ThirdPerson::distance, 0.f, 500.f, XORSTR("Camera Offset: %0.f"));
                ImGui::Combo(XORSTR("Showed Angle"), (int*)& Settings::ThirdPerson::type, angleTypes, IM_ARRAYSIZE(angleTypes));
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("Child2"), ImVec2(0, 0), true);
		{

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Other"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Auto Accept"), &Settings::AutoAccept::enabled);
				ImGui::Checkbox(XORSTR("Autoblock"), &Settings::Autoblock::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::PopItemWidth();
				ImGui::Checkbox(XORSTR("Screenshot Cleaner"), &Settings::ScreenshotCleaner::enabled);
				UI::KeyBindButton(&Settings::Autoblock::key);
			}
			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
}
