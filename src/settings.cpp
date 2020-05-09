#include "settings.h"

#include <dirent.h>
#include <fstream>
#include <unistd.h>

#include "json/json.h"
#include "fonts.h"
#include "Utils/draw.h"
#include "Hacks/skinchanger.h"
#include "Utils/util.h"
#include "Utils/util_items.h"
#include "Utils/util_sdk.h"
#include "Utils/xorstring.h"
#include "config.h"
#include "ATGUI/atgui.h"
#include "Hacks/esp.h"
#include "interfaces.h"


void GetVal(Json::Value &config, int* setting)
{
	if (config.isNull())
		return;

	*setting = config.asInt();
}

void GetVal(Json::Value &config, bool* setting)
{
	if (config.isNull())
		return;

	*setting = config.asBool();
}

void GetVal(Json::Value &config, float* setting)
{
	if (config.isNull())
		return;

	*setting = config.asFloat();
}

void GetVal(Json::Value &config, ImColor* setting)
{
	if (config.isNull())
		return;

	GetVal(config[XORSTR("r")], &setting->Value.x);
	GetVal(config[XORSTR("g")], &setting->Value.y);
	GetVal(config[XORSTR("b")], &setting->Value.z);
	GetVal(config[XORSTR("a")], &setting->Value.w);
}

void GetVal(Json::Value &config, char** setting)
{
	if (config.isNull())
		return;

	*setting = strdup(config.asCString());
}

void GetVal(Json::Value &config, char* setting)
{
	if (config.isNull())
		return;

	strcpy(setting, config.asCString());
}

void GetVal(Json::Value &config, ColorVar* setting)
{
	if (config.isNull())
		return;

	GetVal(config[XORSTR("r")], &setting->color.Value.x);
	GetVal(config[XORSTR("g")], &setting->color.Value.y);
	GetVal(config[XORSTR("b")], &setting->color.Value.z);
	GetVal(config[XORSTR("a")], &setting->color.Value.w);
	GetVal(config[XORSTR("rainbow")], &setting->rainbow);
	GetVal(config[XORSTR("rainbowSpeed")], &setting->rainbowSpeed);
}

void GetVal(Json::Value &config, HealthColorVar* setting)
{
	if (config.isNull())
		return;

	GetVal(config[XORSTR("r")], &setting->color.Value.x);
	GetVal(config[XORSTR("g")], &setting->color.Value.y);
	GetVal(config[XORSTR("b")], &setting->color.Value.z);
	GetVal(config[XORSTR("a")], &setting->color.Value.w);
	GetVal(config[XORSTR("rainbow")], &setting->rainbow);
	GetVal(config[XORSTR("rainbowSpeed")], &setting->rainbowSpeed);
	GetVal(config[XORSTR("hp")], &setting->hp);
}

template <typename Ord, Ord (*lookupFunction)(std::string)>
void GetOrdinal(Json::Value& config, Ord* setting)
{
	if (config.isNull())
		return;

	Ord value;
	if (config.isString())
		value = lookupFunction(config.asString());
	else
		value = (Ord) config.asInt();

	*setting = value;
}

void GetButtonCode(Json::Value &config, enum ButtonCode_t* setting)
{
	GetOrdinal<enum ButtonCode_t, Util::GetButtonCode>(config, setting);
}

void LoadColor(Json::Value &config, ImColor color)
{
	config[XORSTR("r")] = color.Value.x;
	config[XORSTR("g")] = color.Value.y;
	config[XORSTR("b")] = color.Value.z;
	config[XORSTR("a")] = color.Value.w;
}

void LoadColor(Json::Value &config, ColorVar color)
{
	config[XORSTR("r")] = color.color.Value.x;
	config[XORSTR("g")] = color.color.Value.y;
	config[XORSTR("b")] = color.color.Value.z;
	config[XORSTR("a")] = color.color.Value.w;
	config[XORSTR("rainbow")] = color.rainbow;
	config[XORSTR("rainbowSpeed")] = color.rainbowSpeed;
}

void LoadColor(Json::Value &config, HealthColorVar color)
{
	config[XORSTR("r")] = color.color.Value.x;
	config[XORSTR("g")] = color.color.Value.y;
	config[XORSTR("b")] = color.color.Value.z;
	config[XORSTR("a")] = color.color.Value.w;
	config[XORSTR("rainbow")] = color.rainbow;
	config[XORSTR("rainbowSpeed")] = color.rainbowSpeed;
	config[XORSTR("hp")] = color.hp;
}

void Settings::LoadDefaultsOrSave(std::string path)
{
	Json::Value settings;
	Json::StyledWriter styledWriter;

	LoadColor(settings[XORSTR("UI")][XORSTR("mainColor")], Settings::UI::mainColor);
	LoadColor(settings[XORSTR("UI")][XORSTR("bodyColor")], Settings::UI::bodyColor);
	LoadColor(settings[XORSTR("UI")][XORSTR("fontColor")], Settings::UI::fontColor);
	LoadColor(settings[XORSTR("UI")][XORSTR("accentColor")], Settings::UI::accentColor);
	settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("family")] = Settings::UI::Fonts::ESP::family;
	settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("size")] = Settings::UI::Fonts::ESP::size;
	settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("flags")] = Settings::UI::Fonts::ESP::flags;

	for (auto i : Settings::Aimbot::weapons)
	{
		// TODO this is kind of a hack and i'm too tired to find a better way to do this
		// yes i tried defining a variable, skinSetting, and giving it the same value but woooooo operator overloading
		// in C++ and weird shit
		#define weaponSetting settings[XORSTR("Aimbot")][XORSTR("weapons")][Util::Items::GetItemName((enum ItemDefinitionIndex) i.first)]
		weaponSetting[XORSTR("Enabled")] = i.second.enabled;
		weaponSetting[XORSTR("Silent")] = i.second.silent;
		weaponSetting[XORSTR("Friendly")] = i.second.friendly;
		weaponSetting[XORSTR("ClosestBone")] = i.second.closestBone;
		weaponSetting[XORSTR("engageLock")] = i.second.engageLock;
		weaponSetting[XORSTR("engageLockTR")] = i.second.engageLockTR;
		weaponSetting[XORSTR("engageLockTTR")] = i.second.engageLockTTR;
		weaponSetting[XORSTR("TargetBone")] = (int) i.second.bone;
		weaponSetting[XORSTR("AimKey")] = Util::GetButtonName(i.second.aimkey);
		weaponSetting[XORSTR("AimKeyOnly")] = i.second.aimkeyOnly;
		weaponSetting[XORSTR("Smooth")][XORSTR("Enabled")] = i.second.smoothEnabled;
		weaponSetting[XORSTR("Smooth")][XORSTR("Amount")] = i.second.smoothAmount;
		weaponSetting[XORSTR("Smooth")][XORSTR("Type")] = (int) i.second.smoothType;
		weaponSetting[XORSTR("Smooth")][XORSTR("Salting")][XORSTR("Enabled")] = i.second.smoothSaltEnabled;
		weaponSetting[XORSTR("Smooth")][XORSTR("Salting")][XORSTR("Multiplier")] = i.second.smoothSaltMultiplier;
		weaponSetting[XORSTR("ErrorMargin")][XORSTR("Enabled")] = i.second.errorMarginEnabled;
		weaponSetting[XORSTR("ErrorMargin")][XORSTR("Value")] = i.second.errorMarginValue;
		weaponSetting[XORSTR("AutoAim")][XORSTR("Enabled")] = i.second.autoAimEnabled;
		weaponSetting[XORSTR("AutoAim")][XORSTR("FOV")] = i.second.autoAimFov;
		weaponSetting[XORSTR("AimStep")][XORSTR("Enabled")] = i.second.aimStepEnabled;
		weaponSetting[XORSTR("AimStep")][XORSTR("min")] = i.second.aimStepMin;
		weaponSetting[XORSTR("AimStep")][XORSTR("max")] = i.second.aimStepMax;
		weaponSetting[XORSTR("RCS")][XORSTR("Enabled")] = i.second.rcsEnabled;
		weaponSetting[XORSTR("RCS")][XORSTR("AlwaysOn")] = i.second.rcsAlwaysOn;
		weaponSetting[XORSTR("RCS")][XORSTR("AmountX")] = i.second.rcsAmountX;
		weaponSetting[XORSTR("RCS")][XORSTR("AmountY")] = i.second.rcsAmountY;
		weaponSetting[XORSTR("AutoPistol")][XORSTR("Enabled")] = i.second.autoPistolEnabled;
		weaponSetting[XORSTR("AutoShoot")][XORSTR("Enabled")] = i.second.autoShootEnabled;
		weaponSetting[XORSTR("AutoScope")][XORSTR("Enabled")] = i.second.autoScopeEnabled;
		weaponSetting[XORSTR("NoShoot")][XORSTR("Enabled")] = i.second.noShootEnabled;
		weaponSetting[XORSTR("IgnoreJump")][XORSTR("Enabled")] = i.second.ignoreJumpEnabled;
		weaponSetting[XORSTR("IgnoreEnemyJump")][XORSTR("Enabled")] = i.second.ignoreEnemyJumpEnabled;
		weaponSetting[XORSTR("SmokeCheck")][XORSTR("Enabled")] = i.second.smokeCheck;
		weaponSetting[XORSTR("FlashCheck")][XORSTR("Enabled")] = i.second.flashCheck;
		weaponSetting[XORSTR("SpreadLimit")][XORSTR("Enabled")] = i.second.spreadLimitEnabled;
		weaponSetting[XORSTR("SpreadLimit")][XORSTR("Value")] = i.second.spreadLimit;
		weaponSetting[XORSTR("AutoWall")][XORSTR("Enabled")] = i.second.autoWallEnabled;
		weaponSetting[XORSTR("AutoWall")][XORSTR("Value")] = i.second.autoWallValue;
		weaponSetting[XORSTR("AutoSlow")][XORSTR("enabled")] = i.second.autoSlow;
		weaponSetting[XORSTR("Prediction")][XORSTR("enabled")] = i.second.predEnabled;
		weaponSetting[XORSTR("ScopeControl")][XORSTR("Enabled")] = i.second.scopeControlEnabled;

		for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
			weaponSetting[XORSTR("DesiredBones")][XORSTR("Bones")][bone] = i.second.desiredBones[bone];

		weaponSetting[XORSTR("AutoAim")][XORSTR("RealDistance")] = i.second.autoAimRealDistance;

		#undef weaponSetting
	}

	settings[XORSTR("Aimbot")][XORSTR("AutoCrouch")][XORSTR("enabled")] = Settings::Aimbot::AutoCrouch::enabled;
	settings[XORSTR("Aimbot")][XORSTR("AutoShoot")][XORSTR("velocityCheck")] = Settings::Aimbot::AutoShoot::velocityCheck;

	settings[XORSTR("ESP")][XORSTR("enabled")] = Settings::ESP::enabled;
	settings[XORSTR("ESP")][XORSTR("backend")] = (int)Settings::ESP::backend;
	settings[XORSTR("ESP")][XORSTR("key")] = Util::GetButtonName(Settings::ESP::key);
	LoadColor(settings[XORSTR("ESP")][XORSTR("enemy_color")], Settings::ESP::enemyColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("enemy_visible_color")], Settings::ESP::enemyVisibleColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("ally_color")], Settings::ESP::allyColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("ally_visible_color")], Settings::ESP::allyVisibleColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("t_color")], Settings::ESP::tColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("t_visible_color")], Settings::ESP::tVisibleColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("ct_color")], Settings::ESP::ctColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("ct_visible_color")], Settings::ESP::ctVisibleColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("localplayer_color")], Settings::ESP::localplayerColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("bomb_color")], Settings::ESP::bombColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("bomb_defusing_color")], Settings::ESP::bombDefusingColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("hostage_color")], Settings::ESP::hostageColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("defuser_color")], Settings::ESP::defuserColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("weapon_color")], Settings::ESP::weaponColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("chicken_color")], Settings::ESP::chickenColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("fish_color")], Settings::ESP::fishColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("smoke_color")], Settings::ESP::smokeColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("decoy_color")], Settings::ESP::decoyColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("flashbang_color")], Settings::ESP::flashbangColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("grenade_color")], Settings::ESP::grenadeColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("molotov_color")], Settings::ESP::molotovColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("mine_color")], Settings::ESP::mineColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("charge_color")], Settings::ESP::chargeColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("ally_info_color")], Settings::ESP::allyInfoColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("enemy_info_color")], Settings::ESP::enemyInfoColor);
	settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enabled")] = Settings::ESP::Glow::enabled;
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("ally_color")], Settings::ESP::Glow::allyColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_color")], Settings::ESP::Glow::enemyColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_visible_color")], Settings::ESP::Glow::enemyVisibleColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("localplayer_color")], Settings::ESP::Glow::localplayerColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("weapon_color")], Settings::ESP::Glow::weaponColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("grenade_color")], Settings::ESP::Glow::grenadeColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("defuser_color")], Settings::ESP::Glow::defuserColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("chicken_color")], Settings::ESP::Glow::chickenColor);
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("legit")] = Settings::ESP::Filters::legit;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("visibility_check")] = Settings::ESP::Filters::visibilityCheck;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("smoke_check")] = Settings::ESP::Filters::smokeCheck;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("enemies")] = Settings::ESP::Filters::enemies;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("allies")] = Settings::ESP::Filters::allies;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("bomb")] = Settings::ESP::Filters::bomb;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("hostages")] = Settings::ESP::Filters::hostages;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("defusers")] = Settings::ESP::Filters::defusers;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("weapons")] = Settings::ESP::Filters::weapons;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("chickens")] = Settings::ESP::Filters::chickens;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("fishes")] = Settings::ESP::Filters::fishes;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("throwables")] = Settings::ESP::Filters::throwables;
	settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("localplayer")] = Settings::ESP::Filters::localplayer;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("name")] = Settings::ESP::Info::name;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("clan")] = Settings::ESP::Info::clan;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("steam_id")] = Settings::ESP::Info::steamId;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rank")] = Settings::ESP::Info::rank;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("health")] = Settings::ESP::Info::health;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("armor")] = Settings::ESP::Info::armor;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("weapon")] = Settings::ESP::Info::weapon;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("scoped")] = Settings::ESP::Info::scoped;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("reloading")] = Settings::ESP::Info::reloading;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("flashed")] = Settings::ESP::Info::flashed;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("planting")] = Settings::ESP::Info::planting;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("has_defuser")] = Settings::ESP::Info::hasDefuser;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("defusing")] = Settings::ESP::Info::defusing;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("grabbing_hostage")] = Settings::ESP::Info::grabbingHostage;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rescuing")] = Settings::ESP::Info::rescuing;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("location")] = Settings::ESP::Info::location;
	settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("money")] = Settings::ESP::Info::money;
	settings[XORSTR("ESP")][XORSTR("Boxes")][XORSTR("enabled")] = Settings::ESP::Boxes::enabled;
	settings[XORSTR("ESP")][XORSTR("Boxes")][XORSTR("type")] = (int) Settings::ESP::Boxes::type;
	settings[XORSTR("ESP")][XORSTR("Sprite")][XORSTR("enabled")] = Settings::ESP::Sprite::enabled;
	settings[XORSTR("ESP")][XORSTR("Sprite")][XORSTR("type")] = (int) Settings::ESP::Sprite::type;
	settings[XORSTR("ESP")][XORSTR("Skeleton")][XORSTR("enabled")] = Settings::ESP::Skeleton::enabled;
	LoadColor(settings[XORSTR("ESP")][XORSTR("Skeleton")][XORSTR("ally_color")], Settings::ESP::Skeleton::allyColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Skeleton")][XORSTR("enemy_color")], Settings::ESP::Skeleton::enemyColor);
	settings[XORSTR("ESP")][XORSTR("Bars")][XORSTR("enabled")] = Settings::ESP::Bars::enabled;
	settings[XORSTR("ESP")][XORSTR("Bars")][XORSTR("color_type")] = (int) Settings::ESP::Bars::colorType;
	settings[XORSTR("ESP")][XORSTR("Bars")][XORSTR("type")] = (int) Settings::ESP::Bars::type;
	settings[XORSTR("ESP")][XORSTR("Tracers")][XORSTR("enabled")] = Settings::ESP::Tracers::enabled;
	settings[XORSTR("ESP")][XORSTR("Tracers")][XORSTR("type")] = (int) Settings::ESP::Tracers::type;
	settings[XORSTR("ESP")][XORSTR("BulletTracers")][XORSTR("enabled")] = Settings::ESP::BulletTracers::enabled;
	settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("enabled")] = Settings::ESP::FOVCrosshair::enabled;
	settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("filled")] = Settings::ESP::FOVCrosshair::filled;
	LoadColor(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("color")], Settings::ESP::FOVCrosshair::color);
	settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("enabled")] = Settings::ESP::Sounds::enabled;
	settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("time")] = Settings::ESP::Sounds::time;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enabled")] = Settings::ESP::Hitmarker::enabled;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enemies")] = Settings::ESP::Hitmarker::enemies;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("allies")] = Settings::ESP::Hitmarker::allies;
	LoadColor(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("color")], Settings::ESP::Hitmarker::color);
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("duration")] = Settings::ESP::Hitmarker::duration;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("size")] = Settings::ESP::Hitmarker::size;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("inner_gap")] = Settings::ESP::Hitmarker::innerGap;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Damage")][XORSTR("enabled")] = Settings::ESP::Hitmarker::Damage::enabled;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("enabled")] = Settings::ESP::Hitmarker::Sounds::enabled;
	settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("sound")] = (int)Settings::ESP::Hitmarker::Sounds::sound;
	settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("enabled")] = Settings::ESP::HeadDot::enabled;
	settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("size")] = Settings::ESP::HeadDot::size;
	settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("enabled")] = Settings::ESP::Spread::enabled;
	settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("spreadLimit")] = Settings::ESP::Spread::spreadLimit;
	LoadColor(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("color")], Settings::ESP::Spread::color);
	LoadColor(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("spreadLimitColor")], Settings::ESP::Spread::spreadLimitColor);

	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDist")] = Settings::ESP::DangerZone::drawDist;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDistEnabled")] = Settings::ESP::DangerZone::drawDistEnabled;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade")] = Settings::ESP::DangerZone::upgrade;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate")] = Settings::ESP::DangerZone::lootcrate;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel")] = Settings::ESP::DangerZone::barrel;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox")] = Settings::ESP::DangerZone::ammobox;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe")] = Settings::ESP::DangerZone::safe;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun")] = Settings::ESP::DangerZone::dronegun;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone")] = Settings::ESP::DangerZone::drone;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash")] = Settings::ESP::DangerZone::cash;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet")] = Settings::ESP::DangerZone::tablet;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot")] = Settings::ESP::DangerZone::healthshot;
	settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee")] = Settings::ESP::DangerZone::melee;
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade_color")], Settings::ESP::DangerZone::upgradeColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate_color")], Settings::ESP::DangerZone::lootcrateColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel_color")], Settings::ESP::DangerZone::barrelColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox_color")], Settings::ESP::DangerZone::ammoboxColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe_color")], Settings::ESP::DangerZone::safeColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun_color")], Settings::ESP::DangerZone::dronegunColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone_color")], Settings::ESP::DangerZone::droneColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash_color")], Settings::ESP::DangerZone::cashColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet_color")], Settings::ESP::DangerZone::tabletColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot_color")], Settings::ESP::DangerZone::healthshotColor);
	LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee_color")], Settings::ESP::DangerZone::meleeColor);

	settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("enabled")] = Settings::Skinchanger::Skins::enabled;
	settings[XORSTR("SkinChanger")][XORSTR("Models")][XORSTR("enabled")] = Settings::Skinchanger::Models::enabled;
	settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("perTeam")] = Settings::Skinchanger::Skins::perTeam;

	for (const auto& item: Settings::Skinchanger::skinsCT)
	{
		const AttribItem_t& skin = item.second;

		#define skinSetting settings[XORSTR("SkinChanger")][XORSTR("skinsCT")][Util::Items::GetItemConfigEntityName(item.first)]
		skinSetting[XORSTR("ItemDefinitionIndex")] = Util::Items::GetItemConfigEntityName(skin.itemDefinitionIndex);
		skinSetting[XORSTR("PaintKit")] = skin.fallbackPaintKit;
		skinSetting[XORSTR("Wear")] = skin.fallbackWear;
		skinSetting[XORSTR("Seed")] = skin.fallbackSeed;
		skinSetting[XORSTR("StatTrak")] = skin.fallbackStatTrak;
		skinSetting[XORSTR("CustomName")] = skin.customName;
		#undef skinSetting
	}

	for (const auto& item: Settings::Skinchanger::skinsT)
	{
		const AttribItem_t& skin = item.second;

		#define skinSetting settings[XORSTR("SkinChanger")][XORSTR("skinsT")][Util::Items::GetItemConfigEntityName(item.first)]
		skinSetting[XORSTR("ItemDefinitionIndex")] = Util::Items::GetItemConfigEntityName(skin.itemDefinitionIndex);
		skinSetting[XORSTR("PaintKit")] = skin.fallbackPaintKit;
		skinSetting[XORSTR("Wear")] = skin.fallbackWear;
		skinSetting[XORSTR("Seed")] = skin.fallbackSeed;
		skinSetting[XORSTR("StatTrak")] = skin.fallbackStatTrak;
		skinSetting[XORSTR("CustomName")] = skin.customName;
		#undef skinSetting
	}

	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posX")] = Settings::UI::Windows::Colors::posX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posY")] = Settings::UI::Windows::Colors::posY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeX")] = Settings::UI::Windows::Colors::sizeX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeY")] = Settings::UI::Windows::Colors::sizeY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("open")] = Settings::UI::Windows::Colors::open;

	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posX")] = Settings::UI::Windows::Config::posX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posY")] = Settings::UI::Windows::Config::posY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeX")] = Settings::UI::Windows::Config::sizeX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeY")] = Settings::UI::Windows::Config::sizeY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("open")] = Settings::UI::Windows::Config::open;

	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posX")] = Settings::UI::Windows::Main::posX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posY")] = Settings::UI::Windows::Main::posY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeX")] = Settings::UI::Windows::Main::sizeX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeY")] = Settings::UI::Windows::Main::sizeY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("open")] = Settings::UI::Windows::Main::open;

	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posX")] = Settings::UI::Windows::Skinmodel::posX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posY")] = Settings::UI::Windows::Skinmodel::posY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeX")] = Settings::UI::Windows::Skinmodel::sizeX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeY")] = Settings::UI::Windows::Skinmodel::sizeY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("open")] = Settings::UI::Windows::Skinmodel::open;

	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posX")] = Settings::UI::Windows::Spectators::posX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posY")] = Settings::UI::Windows::Spectators::posY;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeX")] = Settings::UI::Windows::Spectators::sizeX;
	settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeY")] = Settings::UI::Windows::Spectators::sizeY;

	settings[XORSTR("NoSky")][XORSTR("enabled")] = Settings::NoSky::enabled;
	LoadColor(settings[XORSTR("NoSky")][XORSTR("color")], Settings::NoSky::color);

	settings[XORSTR("NoScopeBorder")][XORSTR("enabled")] = Settings::NoScopeBorder::enabled;

	settings[XORSTR("ScreenshotCleaner")][XORSTR("enabled")] = Settings::ScreenshotCleaner::enabled;

	settings[XORSTR("ThirdPerson")][XORSTR("enabled")] = Settings::ThirdPerson::enabled;
	settings[XORSTR("ThirdPerson")][XORSTR("distance")] = Settings::ThirdPerson::distance;
	settings[XORSTR("ThirdPerson")][XORSTR("type")] = (int) Settings::ThirdPerson::type;

	std::ofstream(path) << styledWriter.write(settings);
}

void Settings::LoadConfig(std::string path)
{
	if (!std::ifstream(path).good())
	{
		Settings::LoadDefaultsOrSave(path);
		return;
	}

	Json::Value settings;
	std::ifstream configDoc(path, std::ifstream::binary);
	configDoc >> settings;

	GetVal(settings[XORSTR("UI")][XORSTR("mainColor")], &Settings::UI::mainColor);
	GetVal(settings[XORSTR("UI")][XORSTR("bodyColor")], &Settings::UI::bodyColor);
	GetVal(settings[XORSTR("UI")][XORSTR("fontColor")], &Settings::UI::fontColor);
	GetVal(settings[XORSTR("UI")][XORSTR("accentColor")], &Settings::UI::accentColor);
	GetVal(settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("family")], &Settings::UI::Fonts::ESP::family);
	GetVal(settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("size")], &Settings::UI::Fonts::ESP::size);
	GetVal(settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("flags")], &Settings::UI::Fonts::ESP::flags);

	Fonts::SetupFonts();

	Settings::Aimbot::weapons = {
			{ ItemDefinitionIndex::INVALID, defaultSettings },
	};

	for (Json::ValueIterator itr = settings[XORSTR("Aimbot")][XORSTR("weapons")].begin(); itr != settings[XORSTR("Aimbot")][XORSTR("weapons")].end(); itr++)
	{
		std::string weaponDataKey = itr.key().asString();
		auto weaponSetting = settings[XORSTR("Aimbot")][XORSTR("weapons")][weaponDataKey];

		// XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
		// XXX We can't use GetOrdinal() since the key type is a string...
		ItemDefinitionIndex weaponID;
		try
		{
			weaponID = (ItemDefinitionIndex) std::stoi(weaponDataKey);
		}
		catch (std::invalid_argument&) // Not a number
		{
			weaponID = Util::Items::GetItemIndex(weaponDataKey);
		}

		if (Settings::Aimbot::weapons.find(weaponID) == Settings::Aimbot::weapons.end())
			Settings::Aimbot::weapons[weaponID] = AimbotWeapon_t();

		AimbotWeapon_t weapon = {
				.enabled = weaponSetting[XORSTR( "Enabled" )].asBool(),
				.silent = weaponSetting[XORSTR( "Silent" )].asBool(),
				.friendly = weaponSetting[XORSTR( "Friendly" )].asBool(),
				.closestBone = weaponSetting[XORSTR( "ClosestBone" )].asBool(),
				.engageLock = weaponSetting[XORSTR( "engageLock" )].asBool(),
				.engageLockTR = weaponSetting[XORSTR( "engageLockTR" )].asBool(),
				.aimkeyOnly = weaponSetting[XORSTR( "AimKeyOnly" )].asBool(),
				.smoothEnabled = weaponSetting[XORSTR( "Smooth" )][XORSTR( "Enabled" )].asBool(),
				.smoothSaltEnabled = weaponSetting[XORSTR( "Smooth" )][XORSTR( "Salting" )][XORSTR( "Enabled" )].asBool(),
				.errorMarginEnabled = weaponSetting[XORSTR( "ErrorMargin" )][XORSTR( "Enabled" )].asBool(),
				.autoAimEnabled = weaponSetting[XORSTR( "AutoAim" )][XORSTR( "Enabled" )].asBool(),
				.aimStepEnabled = weaponSetting[XORSTR( "AimStep" )][XORSTR( "Enabled" )].asBool(),
				.rcsEnabled = weaponSetting[XORSTR( "RCS" )][XORSTR( "Enabled" )].asBool(),
				.rcsAlwaysOn = weaponSetting[XORSTR( "RCS" )][XORSTR( "AlwaysOn" )].asBool(),
				.spreadLimitEnabled = weaponSetting[XORSTR( "SpreadLimit" )][XORSTR( "Enabled" )].asBool(),
				.autoPistolEnabled = weaponSetting[XORSTR( "AutoPistol" )][XORSTR( "Enabled" )].asBool(),
				.autoShootEnabled = weaponSetting[XORSTR( "AutoShoot" )][XORSTR( "Enabled" )].asBool(),
				.autoScopeEnabled = weaponSetting[XORSTR( "AutoScope" )][XORSTR( "Enabled" )].asBool(),
				.noShootEnabled = weaponSetting[XORSTR( "NoShoot" )][XORSTR( "Enabled" )].asBool(),
				.ignoreJumpEnabled = weaponSetting[XORSTR( "IgnoreJump" )][XORSTR( "Enabled" )].asBool(),
				.ignoreEnemyJumpEnabled = weaponSetting[XORSTR( "IgnoreEnemyJump" )][XORSTR( "Enabled" )].asBool(),
				.smokeCheck = weaponSetting[XORSTR( "SmokeCheck" )][XORSTR( "Enabled" )].asBool(),
				.flashCheck = weaponSetting[XORSTR( "FlashCheck" )][XORSTR( "Enabled" )].asBool(),
				.autoWallEnabled = weaponSetting[XORSTR( "AutoWall" )][XORSTR( "Enabled" )].asBool(),
				.autoAimRealDistance = weaponSetting[XORSTR( "AutoAim" )][XORSTR( "RealDistance" )].asBool(),
				.autoSlow = weaponSetting[XORSTR( "AutoSlow" )][XORSTR( "enabled" )].asBool(),
				.predEnabled = weaponSetting[XORSTR( "Prediction" )][XORSTR( "enabled" )].asBool(),
				.scopeControlEnabled = weaponSetting[XORSTR( "ScopeControl" )][XORSTR( "Enabled" )].asBool(),

				.engageLockTTR = weaponSetting[XORSTR( "engageLockTTR" )].asInt(),
				.bone = weaponSetting[XORSTR( "TargetBone" )].asInt(),
				.smoothType = (SmoothType) weaponSetting[XORSTR( "Smooth" )][XORSTR( "Type" )].asInt(),
				.aimkey = Util::GetButtonCode(weaponSetting[XORSTR( "AimKey" )].asCString()),
				.smoothAmount = weaponSetting[XORSTR( "Smooth" )][XORSTR( "Amount" )].asFloat(),
				.smoothSaltMultiplier = weaponSetting[XORSTR( "Smooth" )][XORSTR( "Salting" )][XORSTR( "Multiplier" )].asFloat(),
				.errorMarginValue = weaponSetting[XORSTR( "ErrorMargin" )][XORSTR( "Value" )].asFloat(),
				.autoAimFov = weaponSetting[XORSTR( "AutoAim" )][XORSTR( "FOV" )].asFloat(),
				.aimStepMin = weaponSetting[XORSTR( "AimStep" )][XORSTR( "min" )].asFloat(),
				.aimStepMax = weaponSetting[XORSTR( "AimStep" )][XORSTR( "max" )].asFloat(),
				.rcsAmountX = weaponSetting[XORSTR( "RCS" )][XORSTR( "AmountX" )].asFloat(),
				.rcsAmountY = weaponSetting[XORSTR( "RCS" )][XORSTR( "AmountY" )].asFloat(),
				.autoWallValue = weaponSetting[XORSTR( "AutoWall" )][XORSTR( "Value" )].asFloat(),
				.spreadLimit = weaponSetting[XORSTR( "SpreadLimit" )][XORSTR( "Value" )].asFloat(),
		};

		for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
			weapon.desiredBones[bone] = weaponSetting[XORSTR("DesiredBones")][XORSTR("Bones")][bone].asBool();
		Settings::Aimbot::weapons.at(weaponID) = weapon;
	}

	GetVal(settings[XORSTR("Aimbot")][XORSTR("AutoCrouch")][XORSTR("enabled")], &Settings::Aimbot::AutoCrouch::enabled);
	GetVal(settings[XORSTR("Aimbot")][XORSTR("AutoShoot")][XORSTR("velocityCheck")], &Settings::Aimbot::AutoShoot::velocityCheck);

	GetVal(settings[XORSTR("ESP")][XORSTR("enabled")], &Settings::ESP::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("backend")], (int*)&Settings::ESP::backend);
	GetButtonCode(settings[XORSTR("ESP")][XORSTR("key")], &Settings::ESP::key);
	GetVal(settings[XORSTR("ESP")][XORSTR("enemy_color")], &Settings::ESP::enemyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("enemy_visible_color")], &Settings::ESP::enemyVisibleColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("ally_color")], &Settings::ESP::allyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("ally_visible_color")], &Settings::ESP::allyVisibleColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("t_color")], &Settings::ESP::tColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("t_visible_color")], &Settings::ESP::tVisibleColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("ct_color")], &Settings::ESP::ctColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("ct_visible_color")], &Settings::ESP::ctVisibleColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("localplayer_color")], &Settings::ESP::localplayerColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("bomb_color")], &Settings::ESP::bombColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("bomb_defusing_color")], &Settings::ESP::bombDefusingColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("hostage_color")], &Settings::ESP::hostageColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("defuser_color")], &Settings::ESP::defuserColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("weapon_color")], &Settings::ESP::weaponColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("chicken_color")], &Settings::ESP::chickenColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("fish_color")], &Settings::ESP::fishColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("smoke_color")], &Settings::ESP::smokeColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("decoy_color")], &Settings::ESP::decoyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("flashbang_color")], &Settings::ESP::flashbangColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("grenade_color")], &Settings::ESP::grenadeColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("molotov_color")], &Settings::ESP::molotovColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("mine_color")], &Settings::ESP::mineColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("charge_color")], &Settings::ESP::chargeColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("ally_info_color")], &Settings::ESP::allyInfoColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("enemy_info_color")], &Settings::ESP::enemyInfoColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enabled")], &Settings::ESP::Glow::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("ally_color")], &Settings::ESP::Glow::allyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_color")], &Settings::ESP::Glow::enemyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_visible_color")], &Settings::ESP::Glow::enemyVisibleColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("localplayer_color")], &Settings::ESP::Glow::localplayerColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("weapon_color")], &Settings::ESP::Glow::weaponColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("grenade_color")], &Settings::ESP::Glow::grenadeColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("defuser_color")], &Settings::ESP::Glow::defuserColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("chicken_color")], &Settings::ESP::Glow::chickenColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("legit")], &Settings::ESP::Filters::legit);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("visibility_check")], &Settings::ESP::Filters::visibilityCheck);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("smoke_check")], &Settings::ESP::Filters::smokeCheck);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("enemies")], &Settings::ESP::Filters::enemies);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("allies")], &Settings::ESP::Filters::allies);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("bomb")], &Settings::ESP::Filters::bomb);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("hostages")], &Settings::ESP::Filters::hostages);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("defusers")], &Settings::ESP::Filters::defusers);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("weapons")], &Settings::ESP::Filters::weapons);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("chickens")], &Settings::ESP::Filters::chickens);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("fishes")], &Settings::ESP::Filters::fishes);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("throwables")], &Settings::ESP::Filters::throwables);
	GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("localplayer")], &Settings::ESP::Filters::localplayer);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("name")], &Settings::ESP::Info::name);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("clan")], &Settings::ESP::Info::clan);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("steam_id")], &Settings::ESP::Info::steamId);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rank")], &Settings::ESP::Info::rank);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("health")], &Settings::ESP::Info::health);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("armor")], &Settings::ESP::Info::armor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("weapon")], &Settings::ESP::Info::weapon);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("scoped")], &Settings::ESP::Info::scoped);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("reloading")], &Settings::ESP::Info::reloading);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("flashed")], &Settings::ESP::Info::flashed);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("planting")], &Settings::ESP::Info::planting);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("has_defuser")], &Settings::ESP::Info::hasDefuser);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("defusing")], &Settings::ESP::Info::defusing);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("grabbing_hostage")], &Settings::ESP::Info::grabbingHostage);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rescuing")], &Settings::ESP::Info::rescuing);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("location")], &Settings::ESP::Info::location);
	GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("money")], &Settings::ESP::Info::money);
	GetVal(settings[XORSTR("ESP")][XORSTR("Boxes")][XORSTR("enabled")], &Settings::ESP::Boxes::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Boxes")][XORSTR("type")], (int*)& Settings::ESP::Boxes::type);
	GetVal(settings[XORSTR("ESP")][XORSTR("Sprite")][XORSTR("enabled")], &Settings::ESP::Sprite::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Sprite")][XORSTR("type")], (int*)&Settings::ESP::Sprite::type);
	GetVal(settings[XORSTR("ESP")][XORSTR("Skeleton")][XORSTR("enabled")], &Settings::ESP::Skeleton::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Skeleton")][XORSTR("ally_color")], &Settings::ESP::Skeleton::allyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Skeleton")][XORSTR("enemy_color")], &Settings::ESP::Skeleton::enemyColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("Bars")][XORSTR("enabled")], &Settings::ESP::Bars::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Bars")][XORSTR("color_type")], (int*)& Settings::ESP::Bars::colorType);
	GetVal(settings[XORSTR("ESP")][XORSTR("Bars")][XORSTR("type")], (int*)& Settings::ESP::Bars::type);
	GetVal(settings[XORSTR("ESP")][XORSTR("Tracers")][XORSTR("enabled")], &Settings::ESP::Tracers::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Tracers")][XORSTR("type")], (int*)& Settings::ESP::Tracers::type);
	GetVal(settings[XORSTR("ESP")][XORSTR("BulletTracers")][XORSTR("enabled")], &Settings::ESP::BulletTracers::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("enabled")], &Settings::ESP::FOVCrosshair::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("filled")], &Settings::ESP::FOVCrosshair::filled);
	GetVal(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("color")], &Settings::ESP::FOVCrosshair::color);
	GetVal(settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("enabled")], &Settings::ESP::Sounds::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("time")], &Settings::ESP::Sounds::time);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enabled")], &Settings::ESP::Hitmarker::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enemies")], &Settings::ESP::Hitmarker::enemies);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("allies")], &Settings::ESP::Hitmarker::allies);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("color")], &Settings::ESP::Hitmarker::color);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("duration")], &Settings::ESP::Hitmarker::duration);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("size")], &Settings::ESP::Hitmarker::size);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("inner_gap")], &Settings::ESP::Hitmarker::innerGap);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Damage")][XORSTR("enabled")], &Settings::ESP::Hitmarker::Damage::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("enabled")], &Settings::ESP::Hitmarker::Sounds::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("sound")], (int*)&Settings::ESP::Hitmarker::Sounds::sound);
	GetVal(settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("enabled")], &Settings::ESP::HeadDot::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("size")], &Settings::ESP::HeadDot::size);
	GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("enabled")], &Settings::ESP::Spread::enabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("spreadLimit")], &Settings::ESP::Spread::spreadLimit);
	GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("color")], &Settings::ESP::Spread::color);
	GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("spreadLimitColor")], &Settings::ESP::Spread::spreadLimitColor);

	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDist")], &Settings::ESP::DangerZone::drawDist);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDistEnabled")], &Settings::ESP::DangerZone::drawDistEnabled);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade")], &Settings::ESP::DangerZone::upgrade);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate")], &Settings::ESP::DangerZone::lootcrate);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel")], &Settings::ESP::DangerZone::barrel);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox")], &Settings::ESP::DangerZone::ammobox);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe")], &Settings::ESP::DangerZone::safe);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun")], &Settings::ESP::DangerZone::dronegun);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone")], &Settings::ESP::DangerZone::drone);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash")], &Settings::ESP::DangerZone::cash);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet")], &Settings::ESP::DangerZone::tablet);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot")], &Settings::ESP::DangerZone::healthshot);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee")], &Settings::ESP::DangerZone::melee);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade_color")], &Settings::ESP::DangerZone::upgradeColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate_color")], &Settings::ESP::DangerZone::lootcrateColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel_color")], &Settings::ESP::DangerZone::barrelColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox_color")], &Settings::ESP::DangerZone::ammoboxColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe_color")], &Settings::ESP::DangerZone::safeColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun_color")], &Settings::ESP::DangerZone::dronegunColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone_color")], &Settings::ESP::DangerZone::droneColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash_color")], &Settings::ESP::DangerZone::cashColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet_color")], &Settings::ESP::DangerZone::tabletColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot_color")], &Settings::ESP::DangerZone::healthshotColor);
	GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee_color")], &Settings::ESP::DangerZone::meleeColor);

	Settings::Skinchanger::Skins::enabled = false;
	Settings::Skinchanger::skinsCT.clear();
	Settings::Skinchanger::skinsT.clear();

	for (Json::ValueIterator itr = settings[XORSTR("SkinChanger")][XORSTR("skinsCT")].begin(); itr != settings[XORSTR("SkinChanger")][XORSTR("skinsCT")].end(); itr++)
	{
		std::string skinDataKey = itr.key().asString();
		auto skinSetting = settings[XORSTR("SkinChanger")][XORSTR("skinsCT")][skinDataKey];

		// XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
		// XXX We can't use GetOrdinal() since the key type is a string...
		unsigned int weaponID;

		try
		{
			weaponID = std::stoi(skinDataKey);
		}
		catch(std::invalid_argument&)
		{
			weaponID = (int) Util::Items::GetItemIndex(skinDataKey);
		}

		ItemDefinitionIndex defIndex = ItemDefinitionIndex::INVALID;
		GetOrdinal<ItemDefinitionIndex, Util::Items::GetItemIndex>(skinSetting[XORSTR("ItemDefinitionIndex")], &defIndex);

		if (Settings::Skinchanger::skinsCT.find((ItemDefinitionIndex) weaponID) == Settings::Skinchanger::skinsCT.end())
			Settings::Skinchanger::skinsCT[(ItemDefinitionIndex) weaponID] = AttribItem_t();

		AttribItem_t skin = {
				defIndex,
				skinSetting[XORSTR("PaintKit")].asInt(),
				skinSetting[XORSTR("Wear")].asFloat(),
				skinSetting[XORSTR("Seed")].asInt(),
				skinSetting[XORSTR("StatTrak")].asInt(),
				-1,
				skinSetting[XORSTR("CustomName")].asString(),
		};

		Settings::Skinchanger::skinsCT.at((ItemDefinitionIndex) weaponID) = skin;
	}

	for (Json::ValueIterator itr = settings[XORSTR("SkinChanger")][XORSTR("skinsT")].begin(); itr != settings[XORSTR("SkinChanger")][XORSTR("skinsT")].end(); itr++)
	{
		std::string skinDataKey = itr.key().asString();
		auto skinSetting = settings[XORSTR("SkinChanger")][XORSTR("skinsT")][skinDataKey];

		// XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
		// XXX We can't use GetOrdinal() since the key type is a string...
		unsigned int weaponID;

		try
		{
			weaponID = std::stoi(skinDataKey);
		}
		catch(std::invalid_argument&)
		{
			weaponID = (int) Util::Items::GetItemIndex(skinDataKey);
		}

		ItemDefinitionIndex defIndex = ItemDefinitionIndex::INVALID;
		GetOrdinal<ItemDefinitionIndex, Util::Items::GetItemIndex>(skinSetting[XORSTR("ItemDefinitionIndex")], &defIndex);

		if (Settings::Skinchanger::skinsT.find((ItemDefinitionIndex) weaponID) == Settings::Skinchanger::skinsT.end())
			Settings::Skinchanger::skinsT[(ItemDefinitionIndex) weaponID] = AttribItem_t();

		AttribItem_t skin = {
				defIndex,
				skinSetting[XORSTR("PaintKit")].asInt(),
				skinSetting[XORSTR("Wear")].asFloat(),
				skinSetting[XORSTR("Seed")].asInt(),
				skinSetting[XORSTR("StatTrak")].asInt(),
				-1,
				skinSetting[XORSTR("CustomName")].asString(),
		};

		Settings::Skinchanger::skinsT.at((ItemDefinitionIndex) weaponID) = skin;
	}

	SkinChanger::forceFullUpdate = true;

	GetVal(settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("enabled")], &Settings::Skinchanger::Skins::enabled);
	GetVal(settings[XORSTR("SkinChanger")][XORSTR("Models")][XORSTR("enabled")], &Settings::Skinchanger::Models::enabled);
	GetVal(settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("perTeam")], &Settings::Skinchanger::Skins::perTeam);

	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posX")], &Settings::UI::Windows::Colors::posX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posY")], &Settings::UI::Windows::Colors::posY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeX")], &Settings::UI::Windows::Colors::sizeX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeY")], &Settings::UI::Windows::Colors::sizeY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("open")], &Settings::UI::Windows::Colors::open);

	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posX")], &Settings::UI::Windows::Config::posX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posY")], &Settings::UI::Windows::Config::posY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeX")], &Settings::UI::Windows::Config::sizeX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeY")], &Settings::UI::Windows::Config::sizeY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("open")], &Settings::UI::Windows::Config::open);

	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posX")], &Settings::UI::Windows::Main::posX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posY")], &Settings::UI::Windows::Main::posY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeX")], &Settings::UI::Windows::Main::sizeX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeY")], &Settings::UI::Windows::Main::sizeY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("open")], &Settings::UI::Windows::Main::open);

	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posX")], &Settings::UI::Windows::Skinmodel::posX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posY")], &Settings::UI::Windows::Skinmodel::posY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeX")], &Settings::UI::Windows::Skinmodel::sizeX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeY")], &Settings::UI::Windows::Skinmodel::sizeY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("open")], &Settings::UI::Windows::Skinmodel::open);

	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posX")], &Settings::UI::Windows::Spectators::posX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posY")], &Settings::UI::Windows::Spectators::posY);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeX")], &Settings::UI::Windows::Spectators::sizeX);
	GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeY")], &Settings::UI::Windows::Spectators::sizeY);

	Settings::UI::Windows::Main::reload = true;
	Settings::UI::Windows::Colors::reload = true;
	Settings::UI::Windows::Config::reload = true;
	Settings::UI::Windows::Skinmodel::reload = true;
	Settings::UI::Windows::Spectators::reload = true;

	GetVal(settings[XORSTR("NoSky")][XORSTR("enabled")], &Settings::NoSky::enabled);
	GetVal(settings[XORSTR("NoSky")][XORSTR("color")], &Settings::NoSky::color);

	GetVal(settings[XORSTR("NoScopeBorder")][XORSTR("enabled")], &Settings::NoScopeBorder::enabled);

	GetVal(settings[XORSTR("ScreenshotCleaner")][XORSTR("enabled")], &Settings::ScreenshotCleaner::enabled);

	GetVal(settings[XORSTR("ThirdPerson")][XORSTR("enabled")], &Settings::ThirdPerson::enabled);
	GetVal(settings[XORSTR("ThirdPerson")][XORSTR("distance")], &Settings::ThirdPerson::distance);
	GetVal(settings[XORSTR("ThirdPerson")][XORSTR("type")], (int*)&Settings::ThirdPerson::type);
}

void remove_directory(const char* path)
{
    DIR* dir = opendir(path);
    dirent* pdir;

    const size_t path_len = strlen(path);

    while ((pdir = readdir(dir)))
    {
		if (strcmp(pdir->d_name, ".") == 0 || strcmp(pdir->d_name, "..") == 0)
			continue;

		std::string _path;
		_path.reserve(path_len + Util::StrLen("/") + strlen(pdir->d_name));
		_path.append(path);
		_path += '/';
		_path.append(pdir->d_name);

		if (pdir->d_type == DT_DIR)
		{
			remove_directory(_path.c_str());
		}
		else if (pdir->d_type == DT_REG)
		{
			unlink(_path.c_str());
		}
	}

	rmdir(path);
}

void Settings::DeleteConfig(std::string path)
{
	remove_directory(path.c_str());
}