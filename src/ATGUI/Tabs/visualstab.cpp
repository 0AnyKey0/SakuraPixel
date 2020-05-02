#include "visualstab.h"

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void Visuals::RenderTab()
{
	const char* BackendTypes[] = { "Surface (Valve)", "ImGUI (Custom/Faster)" };
	const char* BoxTypes[] = { "Flat 2D", "Frame 2D", "Box 3D", "Hitboxes" };
	const char* SpriteTypes[] = { "Tux" };
	const char* TracerTypes[] = { "Bottom", "Cursor" };
	const char* BarTypes[] = { "Vertical Left", "Vertical Right", "Horizontal Below", "Horizontal Above", "Interwebz" };
	const char* BarColorTypes[] = { "Static", "Health Based" };
	const char* TeamColorTypes[] = { "Absolute", "Relative" };
	const char* ArmsTypes[] = { "Default", "Wireframe", "None" };
	const char* WeaponTypes[] = { "Default", "Wireframe", "None" };
	const char* SmokeTypes[] = { "Wireframe", "None" };
    const char* Sounds[] = { "None", "SpongeBob", "Half life", "Half life 2", "Half life 3", "Half life 4", "BB Gun Bell", "Dopamine", "Wub", "Pedo Yes!", "Meme", "Error", "Orchestral" };

	ImGui::Columns(2, nullptr, true);
	{
		ImGui::Checkbox(XORSTR("Enabled"), &Settings::ESP::enabled);
        ImGui::Combo( XORSTR( "##BACKENDTYPE" ), (int*)&Settings::ESP::backend, BackendTypes, IM_ARRAYSIZE( BackendTypes ) );

        ImGui::NextColumn();
		ImGui::Text(XORSTR("Only on Key"));
		UI::KeyBindButton(&Settings::ESP::key);
	}
	ImGui::Separator();

	ImGui::Columns(2, nullptr, true);
	{
		ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), true);
		{
			ImGui::Text(XORSTR("ESP"));
			ImGui::BeginChild(XORSTR("ESP"), ImVec2(0, 0), true);
			ImGui::Text(XORSTR("Type"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Outline Box"), &Settings::ESP::Boxes::enabled);
				ImGui::Checkbox(XORSTR("Sprite ESP"), &Settings::ESP::Sprite::enabled);
				ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::Bars::enabled);
				ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::Tracers::enabled);
				ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
				ImGui::Text(XORSTR("Bar Color"));
				ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
				ImGui::Text(XORSTR("Team Color"));
				ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
				ImGui::Checkbox(XORSTR("Bullet Tracers"), &Settings::ESP::BulletTracers::enabled);
				ImGui::Checkbox(XORSTR("Head Dot"), &Settings::ESP::HeadDot::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::Combo(XORSTR("##BOXTYPE"), (int*)& Settings::ESP::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
				ImGui::Combo(XORSTR("##SPRITETYPE"), (int*)& Settings::ESP::Sprite::type, SpriteTypes, IM_ARRAYSIZE(SpriteTypes));
				ImGui::Combo(XORSTR("##BARTYPE"), (int*)& Settings::ESP::Bars::type, BarTypes, IM_ARRAYSIZE(BarTypes));
				ImGui::Combo(XORSTR("##TRACERTYPE"), (int*)& Settings::ESP::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
				ImGui::Combo(XORSTR("##BARCOLTYPE"), (int*)& Settings::ESP::Bars::colorType, BarColorTypes, IM_ARRAYSIZE(BarColorTypes));
				ImGui::Combo(XORSTR("##TEAMCOLTYPE"), (int*)& Settings::ESP::teamColorType, TeamColorTypes, IM_ARRAYSIZE(TeamColorTypes));
				ImGui::PopItemWidth();
				ImGui::Checkbox(XORSTR("Skeleton"), &Settings::ESP::Skeleton::enabled);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##HDOTSIZE"), &Settings::ESP::HeadDot::size, 1.f, 10.f, XORSTR("Size: %0.3f"));
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Filter"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Enemies"), &Settings::ESP::Filters::enemies);
				ImGui::Checkbox(XORSTR("Chickens"), &Settings::ESP::Filters::chickens);
				ImGui::Checkbox(XORSTR("LocalPlayer"), &Settings::ESP::Filters::localplayer);
				ImGui::Checkbox(XORSTR("Legit Mode"), &Settings::ESP::Filters::legit);
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Allies"), &Settings::ESP::Filters::allies);
				ImGui::Checkbox(XORSTR("Fish"), &Settings::ESP::Filters::fishes);
				ImGui::Checkbox(XORSTR("Smoke Check"), &Settings::ESP::Filters::smokeCheck);
				ImGui::Checkbox(XORSTR("Visiblity Check"), &Settings::ESP::Filters::visibilityCheck);
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Player Information"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Clan"), &Settings::ESP::Info::clan);
				ImGui::Checkbox(XORSTR("Rank"), &Settings::ESP::Info::rank);
				ImGui::PushID(1);
				ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::Info::health);
				ImGui::PopID();
				ImGui::Checkbox(XORSTR("Armor"), &Settings::ESP::Info::armor);
				ImGui::Checkbox(XORSTR("Scoped"), &Settings::ESP::Info::scoped);
				ImGui::Checkbox(XORSTR("Flashed"), &Settings::ESP::Info::flashed);
				ImGui::Checkbox(XORSTR("Defuse Kit"), &Settings::ESP::Info::hasDefuser);
				ImGui::Checkbox(XORSTR("Grabbing Hostage"), &Settings::ESP::Info::grabbingHostage);
				ImGui::Checkbox(XORSTR("Location"), &Settings::ESP::Info::location);
				ImGui::Checkbox(XORSTR("Money"), &Settings::ESP::Info::money);
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Name"), &Settings::ESP::Info::name);
				ImGui::Checkbox(XORSTR("Steam ID"), &Settings::ESP::Info::steamId);
				ImGui::Checkbox(XORSTR("Weapon"), &Settings::ESP::Info::weapon);
				ImGui::Checkbox(XORSTR("Reloading"), &Settings::ESP::Info::reloading);
				ImGui::Checkbox(XORSTR("Planting"), &Settings::ESP::Info::planting);
				ImGui::Checkbox(XORSTR("Defusing"), &Settings::ESP::Info::defusing);
				ImGui::Checkbox(XORSTR("Rescuing Hostage"), &Settings::ESP::Info::rescuing);
                ImGui::Checkbox(XORSTR("Layers Debug"), &Settings::Debug::AnimLayers::draw);
            }

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("World"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Weapons"), &Settings::ESP::Filters::weapons);
				ImGui::Checkbox(XORSTR("Throwables"), &Settings::ESP::Filters::throwables);
				ImGui::Checkbox(XORSTR("Entity Glow"), &Settings::ESP::Glow::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Bomb"), &Settings::ESP::Filters::bomb);
				ImGui::Checkbox(XORSTR("Defuse Kits"), &Settings::ESP::Filters::defusers);
				ImGui::Checkbox(XORSTR("Hostages"), &Settings::ESP::Filters::hostages);
			}
			ImGui::Columns(1);

			ImGui::Separator();
			ImGui::Text(XORSTR("Danger Zone"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Loot Crates"), &Settings::ESP::DangerZone::lootcrate);
				ImGui::Checkbox(XORSTR("Weapon Upgrades"), &Settings::ESP::DangerZone::upgrade);
				ImGui::Checkbox(XORSTR("Ammo box"), &Settings::ESP::DangerZone::ammobox);
				ImGui::Checkbox(XORSTR("Cash"), &Settings::ESP::DangerZone::cash);
				ImGui::Checkbox(XORSTR("Drone"), &Settings::ESP::DangerZone::drone);
				ImGui::Checkbox(XORSTR("Draw Distance"), &Settings::ESP::DangerZone::drawDistEnabled);
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Safe"), &Settings::ESP::DangerZone::safe);
				ImGui::Checkbox(XORSTR("Sentry Turret"), &Settings::ESP::DangerZone::dronegun);
				ImGui::Checkbox(XORSTR("Melee"), &Settings::ESP::DangerZone::melee);
				ImGui::Checkbox(XORSTR("Tablet"), &Settings::ESP::DangerZone::tablet);
				ImGui::Checkbox(XORSTR("Healthshot"), &Settings::ESP::DangerZone::healthshot);
				ImGui::Checkbox(XORSTR("Explosive Barrel"), &Settings::ESP::DangerZone::barrel);
				if (Settings::ESP::DangerZone::drawDistEnabled)
					ImGui::SliderInt(XORSTR("##DZDRAWDIST"), &Settings::ESP::DangerZone::drawDist, 1, 10000, XORSTR("Amount: %0.f"));
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Event logger"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Show Enemies"), &Settings::Eventlog::showEnemies);
				ImGui::Checkbox(XORSTR("Show Allies"), &Settings::Eventlog::showTeammates);

			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##LOGGERDURATION"), &Settings::Eventlog::duration, 1000.f, 5000.f, XORSTR("Log duration: %0.f"));
				ImGui::SliderFloat(XORSTR("##LOGGERLINES"), &Settings::Eventlog::lines, 5, 15, XORSTR("Log lines: %0.f"));
				ImGui::Checkbox(XORSTR("Show LocalPlayer"), &Settings::Eventlog::showLocalplayer);
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);

			ImGui::EndChild();
			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}
