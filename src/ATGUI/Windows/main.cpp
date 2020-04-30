#include "main.h"

#include "../../settings.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"
/*
#include "../Tabs/aimbottab.h"
#include "../Tabs/hvhtab.h"
#include "../Tabs/misctab.h"
#include "../Tabs/triggerbottab.h"
#include "../Tabs/visualstab.h"
*/

bool Main::showWindow = false;

void Main::RenderWindow()
{
	if( Settings::UI::Windows::Main::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Main::posX, Settings::UI::Windows::Main::posY), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiSetCond_Always);
		Main::showWindow = Settings::UI::Windows::Main::open;
		Settings::UI::Windows::Main::reload = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Main::posX, Settings::UI::Windows::Main::posY), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiSetCond_FirstUseEver);
	}
	if (!Main::showWindow)
	{
		Settings::UI::Windows::Main::open = false;
		return;
	}

	static int page = 0;

	if (ImGui::Begin(XORSTR("SakuraPixel"), &Main::showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders))
	{
		Settings::UI::Windows::Main::open = true;
		ImVec2 temp = ImGui::GetWindowSize();
		Settings::UI::Windows::Main::sizeX = (int)temp.x;
		Settings::UI::Windows::Main::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Main::posX = (int)temp.x;
		Settings::UI::Windows::Main::posY = (int)temp.y;
		const char* tabs[] = {
				"Aimbot",
				"Triggerbot",
				"Visuals",
				"HvH",
				"Misc",
		};

		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
		{
			int distance = i == page ? 0 : i > page ? i - page : page - i;

			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
					Settings::UI::mainColor.Color().Value.x - (distance * 0.035f),
					Settings::UI::mainColor.Color().Value.y - (distance * 0.035f),
					Settings::UI::mainColor.Color().Value.z - (distance * 0.035f),
					Settings::UI::mainColor.Color().Value.w
			);

			if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
				page = i;

			ImGui::GetStyle().Colors[ImGuiCol_Button] = Settings::UI::accentColor.Color();

			if (i < IM_ARRAYSIZE(tabs) - 1)
				ImGui::SameLine();
		}

		ImGui::End();
	}
}