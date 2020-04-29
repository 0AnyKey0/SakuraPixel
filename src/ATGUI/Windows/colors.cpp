#include "colors.h"

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

bool Colors::showWindow = false;

void Colors::RenderWindow()
{
	if( Settings::UI::Windows::Colors::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Colors::posX, Settings::UI::Windows::Colors::posY), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Colors::sizeX, Settings::UI::Windows::Colors::sizeY), ImGuiSetCond_Always);
		Settings::UI::Windows::Colors::reload = false;
		Colors::showWindow = Settings::UI::Windows::Colors::open;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Colors::posX, Settings::UI::Windows::Colors::posY), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Colors::sizeX, Settings::UI::Windows::Colors::sizeY), ImGuiSetCond_FirstUseEver);
	}
	if (!Colors::showWindow)
	{
		Settings::UI::Windows::Colors::open = false;
		return;
	}

	struct ColorListVar
	{
		const char* name;
		union
		{
			ColorVar* colorVarPtr;
			HealthColorVar* healthColorVarPtr;
		};
		enum
		{
			COLORVAR_TYPE,
			HEALTHCOLORVAR_TYPE
		} type;

		ColorListVar(const char* name, ColorVar* ptr)
		{
			this->name = name;
			this->colorVarPtr = ptr;
			this->type = COLORVAR_TYPE;
		}

		ColorListVar(const char* name, HealthColorVar* ptr)
		{
			this->name = name;
			this->healthColorVarPtr = ptr;
			this->type = HEALTHCOLORVAR_TYPE;
		}
	};

	ColorListVar colors[] = {
			{ "UI Main", &Settings::UI::mainColor },
			{ "UI Body", &Settings::UI::bodyColor },
			{ "UI Font", &Settings::UI::fontColor },
			{ "UI Accent", &Settings::UI::accentColor },
	};

	const char* colorNames[IM_ARRAYSIZE(colors)];
	for (int i = 0; i < IM_ARRAYSIZE(colors); i++)
		colorNames[i] = colors[i].name;

	static int colorSelected = 0;

	if (ImGui::Begin(XORSTR("Colors"), &Colors::showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize))
	{
		Settings::UI::Windows::Colors::open = true;
		ImVec2 temp = ImGui::GetWindowSize();
		Settings::UI::Windows::Colors::sizeX = (int)temp.x;
		Settings::UI::Windows::Colors::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Colors::posX = (int)temp.x;
		Settings::UI::Windows::Colors::posY = (int)temp.y;
		ImGui::Columns(2, nullptr, true);
		{
			ImGui::PushItemWidth(-1);
			ImGui::ListBox(XORSTR("##COLORSELECTION"), &colorSelected, colorNames, IM_ARRAYSIZE(colorNames), 12);
			ImGui::PopItemWidth();
		}
		ImGui::NextColumn();
		{
			if (colors[colorSelected].type == ColorListVar::HEALTHCOLORVAR_TYPE)
			{
				UI::ColorPicker4((float*)colors[colorSelected].healthColorVarPtr);
				ImGui::Checkbox(XORSTR("Rainbow"), &colors[colorSelected].healthColorVarPtr->rainbow);
				ImGui::SameLine();
				ImGui::Checkbox(XORSTR("Health-Based"), &colors[colorSelected].healthColorVarPtr->hp);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &colors[colorSelected].healthColorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
				ImGui::PopItemWidth();
			}
			else
			{
				UI::ColorPicker4((float*)colors[colorSelected].colorVarPtr);
				ImGui::Checkbox(XORSTR("Rainbow"), &colors[colorSelected].colorVarPtr->rainbow);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &colors[colorSelected].colorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
				ImGui::PopItemWidth();
			}
		}
		ImGui::End();
	}
}
