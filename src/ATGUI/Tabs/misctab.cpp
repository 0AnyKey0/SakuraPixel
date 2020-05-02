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

#include "../../Hacks/grenadehelper.h"

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
			ImGui::Text(XORSTR("Grenade Helper"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Enabled ###ghenabled"), &Settings::GrenadeHelper::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Only matching ###match"), &Settings::GrenadeHelper::onlyMatchingInfos);
			}

			ImGui::Columns(2);
			{
				if (ImGui::Button(XORSTR("Aimassist"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("optionAimAssist"));
				ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiSetCond_Always);
				if (ImGui::BeginPopup(XORSTR("optionAimAssist")))
				{
					ImGui::PushItemWidth(-1);
					ImGui::Checkbox(XORSTR("Enabled"), &Settings::GrenadeHelper::aimAssist);
					ImGui::SliderFloat(XORSTR("###aimstep"), &Settings::GrenadeHelper::aimStep, 0, 10, "Speed: %0.3f");
					ImGui::SliderFloat(XORSTR("###aimfov"), &Settings::GrenadeHelper::aimFov, 0, 180, "Fov: %0.2f");
					ImGui::SliderFloat(XORSTR("###aimdistance"), &Settings::GrenadeHelper::aimDistance, 0, 100, "Distance: %0.2f");
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
			}
			ImGui::NextColumn();
			{
				if (ImGui::Button(XORSTR("Add Info"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("addinfo_throw"));

				ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
				if (ImGui::BeginPopup(XORSTR("addinfo_throw")))
				{
					static int throwMessageCurrent = -1;
					static char inputName[40];
					static int tType = (int)ThrowType::NORMAL;
					static int gType = (int)GrenadeType::SMOKE;

					ImGui::Columns(1);
					ImGui::PushItemWidth(500);
					ImGui::InputText("", inputName, sizeof(inputName));
					ImGui::PopItemWidth();
					ImGui::SameLine();
					if (ImGui::Button(XORSTR("Add")) && engine->IsInGame() && Settings::GrenadeHelper::actMapName.length() > 0)
					{
						C_BasePlayer* localPlayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
						if (strlen(inputName) > 0)
						{
							GrenadeInfo n = GrenadeInfo((GrenadeType)gType, localPlayer->GetEyePosition(), *localPlayer->GetVAngles(), (ThrowType)tType, inputName);
							Settings::GrenadeHelper::grenadeInfos.push_back(n);
							std::ostringstream path;
							path << GetGhConfigDirectory() << Settings::GrenadeHelper::actMapName;
							if (!DoesFileExist(path.str().c_str()))
								mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
							path << XORSTR("/config.json");
							Settings::SaveGrenadeInfo(path.str());
						}
						strcpy(inputName, "");
					}
					ImGui::Columns(2);
					ImGui::Combo(XORSTR("###Throwtype"), &tType, throwTypes, IM_ARRAYSIZE(throwTypes));
					ImGui::NextColumn();
					ImGui::Combo(XORSTR("###Grenadetype"), &gType, grenadeTypes, IM_ARRAYSIZE(grenadeTypes));
					ImGui::Columns(1);
					ImGui::Separator();
					ImGui::PushItemWidth(550);
					auto lambda =[](void* data, int idx, const char** out_text)
					{
						*out_text = Settings::GrenadeHelper::grenadeInfos.at(idx).name.c_str();
						return *out_text != nullptr;
					};
					ImGui::ListBox("", &throwMessageCurrent, lambda, nullptr, Settings::GrenadeHelper::grenadeInfos.size(), 7);
					ImGui::PopItemWidth();
					ImGui::Columns(1);
					if (ImGui::Button(XORSTR("Remove"),  ImVec2(ImGui::GetWindowWidth(), 30)))
						if (throwMessageCurrent > -1 && (int) Settings::GrenadeHelper::grenadeInfos.size() > throwMessageCurrent)
						{
							Settings::GrenadeHelper::grenadeInfos.erase(Settings::GrenadeHelper::grenadeInfos.begin() + throwMessageCurrent);
							std::ostringstream path;
							path << GetGhConfigDirectory() << Settings::GrenadeHelper::actMapName;
							if (!DoesFileExist(path.str().c_str()))
								mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
							path << XORSTR("/config.json");
							Settings::SaveGrenadeInfo(path.str());
						}
					ImGui::EndPopup();
				}
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
				ImGui::Checkbox(XORSTR("Auto Defuse"), &Settings::AutoDefuse::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::PopItemWidth();
				ImGui::Checkbox(XORSTR("Screenshot Cleaner"), &Settings::ScreenshotCleaner::enabled);
				UI::KeyBindButton(&Settings::Autoblock::key);
				ImGui::Checkbox(XORSTR("Silent Defuse"), &Settings::AutoDefuse::silent);
			}
			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
}
