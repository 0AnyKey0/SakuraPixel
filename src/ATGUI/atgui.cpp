#include "atgui.h"

#include "../Utils/draw.h"
#include "../interfaces.h"
#include "../settings.h"
#include "../Utils/xorstring.h"

#include "Windows/colors.h"
#include "Windows/configs.h"
#include "Windows/skinmodelchanger.h"
bool UI::isVisible = false;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void SetupMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8 * 2.0f, 4 * 2.0f));

		ImGui::Selectable(XORSTR("Skin & Model Changer Window"), &SkinModelChanger::showWindow, 0, ImVec2(ImGui::CalcTextSize(XORSTR("Skin & Model Changer Window"), nullptr, true).x, 0.0f));
		ImGui::SameLine();

		ImGui::Selectable(XORSTR("Config Window"), &Configs::showWindow, 0, ImVec2(ImGui::CalcTextSize(XORSTR("Config Window"), nullptr, true).x, 0.0f));
		ImGui::SameLine();

		ImGui::Selectable(XORSTR("Colors Window"), &Colors::showWindow, 0, ImVec2(ImGui::CalcTextSize(XORSTR("Colors Window"), nullptr, true).x, 0.0f));
		ImGui::SameLine();

		ImGui::PopStyleVar();
		ImGui::EndMainMenuBar();
	}
}

void UI::SwapWindow()
{
	if (UI::isVisible)
		return;

	if (engine->IsInGame())
		return;

    Draw::ImText( ImVec2( 4.f, 4.f ), ImColor( 255, 255, 255, 255 ), XORSTR( "SakuraPixel" ), nullptr, 0.0f, nullptr,
                  ImFontFlags_Shadow );
}

void UI::SetVisible(bool visible)
{
	UI::isVisible = visible;
	cvar->FindVar(XORSTR("cl_mouseenable"))->SetValue(!UI::isVisible);
}

void UI::SetupWindows()
{
	if (UI::isVisible)
	{
		SetupMainMenuBar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(960, 645));
			SkinModelChanger::RenderWindow();
		ImGui::PopStyleVar();

		Configs::RenderWindow();
		Colors::RenderWindow();

	}

}
