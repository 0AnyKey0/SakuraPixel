#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "ImGUI/imgui.h"
#include "Utils/util_sdk.h"
#include "Utils/util_items.h"
#include "Utils/util.h"

#include "SDK/IClientEntity.h"
#include "SDK/IInputSystem.h"
#include "SDK/definitions.h"
#include "SDK/Materialsystem_config.h"

enum class DrawingBackend : int {
    SURFACE = 0,
    IMGUI,

    NUMBER_OF_TYPES
};

enum class SmoothType : int
{
	SLOW_END,
	CONSTANT,
	FAST_END,
};

enum class TracerType : int
{
	BOTTOM,
	CURSOR,
};

enum class ClanTagType : int
{
	STATIC,
	MARQUEE,
	WORDS,
	LETTERS,
};

enum class BoxType : int
{
	FLAT_2D,
	FRAME_2D,
	BOX_3D,
	HITBOXES,
};

enum class SpriteType : int
{
	SPRITE_TUX,
};

enum class Sound : int {

	NONE,
	SPONGEBOB,
	HALFLIFE,
	HALFLIFE2,
	HALFLIFE3,
	HALFLIFE4,
	BBGUNBELL,
	DOPAMINE,
	WUB,
	PEDOYES,
	MEME,
	ERROR,
	ORCHESTRAL,

};

enum class BarType : int
{
	VERTICAL,
	VERTICAL_RIGHT,
	HORIZONTAL,
	HORIZONTAL_UP,
	INTERWEBZ,
};

enum class BarColorType : int
{
	STATIC_COLOR,
	HEALTH_BASED,
};

enum class TeamColorType : int
{
	ABSOLUTE,
	RELATIVE,
};

enum class ArmsType : int
{
	DEFAULT,
	WIREFRAME,
	NONE,
};

enum class WeaponType : int
{
	DEFAULT,
	WIREFRAME,
	NONE,
};

enum class SmokeType : int
{
	WIREFRAME,
	NONE,
};

enum class AimTargetType : int
{
	FOV,
	DISTANCE,
	REAL_DISTANCE,
	HP,
};

enum class ShowedAngle : int
{
    REAL,
    FAKE,
};

struct AimbotWeapon_t
{
	bool enabled,
		 silent,
		 friendly,
		 closestBone,
	     engageLock,
		 engageLockTR,
		 aimkeyOnly,
		 smoothEnabled,
		 smoothSaltEnabled,
		 errorMarginEnabled,
		 autoAimEnabled,
		 aimStepEnabled,
		 rcsEnabled,
		 rcsAlwaysOn,
		 spreadLimitEnabled,
		 autoPistolEnabled,
		 autoShootEnabled,
		 autoScopeEnabled,
		 noShootEnabled,
		 ignoreJumpEnabled,
		 ignoreEnemyJumpEnabled,
		 smokeCheck,
		 flashCheck,
		 autoWallEnabled,
		 autoAimRealDistance,
		 autoSlow,
		 predEnabled,
		 scopeControlEnabled;
	int engageLockTTR = 700;
	Bone bone = BONE_HEAD;
	SmoothType smoothType = SmoothType::SLOW_END;
	ButtonCode_t aimkey = ButtonCode_t ::MOUSE_MIDDLE;
	float smoothAmount = 1.0f,
		  smoothSaltMultiplier = 0.0f,
		  errorMarginValue = 0.0f,
		  autoAimFov = 180.0f,
		  aimStepMin = 25.0f,
		  aimStepMax = 35.0f,
		  rcsAmountX = 2.0f,
		  rcsAmountY = 2.0f,
		  autoWallValue = 10.0f,
		  spreadLimit = 1.0f;
	bool desiredBones[31];

	bool operator == (const AimbotWeapon_t& another) const
	{
		for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		{
			if( this->desiredBones[bone] != another.desiredBones[bone] )
				return false;
		}

		return this->enabled == another.enabled &&
			this->silent == another.silent &&
			this->friendly == another.friendly &&
			this->closestBone == another.closestBone &&
			this->engageLock == another.engageLock &&
			this->engageLockTR == another.engageLockTR &&
			this->engageLockTTR == another.engageLockTTR &&
			this->bone == another.bone &&
			this->aimkey == another.aimkey &&
			this->aimkeyOnly == another.aimkeyOnly &&
			this->smoothEnabled == another.smoothEnabled &&
			this->smoothAmount == another.smoothAmount &&
			this->smoothType == another.smoothType &&
			this->smoothSaltEnabled == another.smoothSaltEnabled &&
			this->smoothSaltMultiplier == another.smoothSaltMultiplier &&
			this->errorMarginEnabled == another.errorMarginEnabled &&
			this->errorMarginValue == another.errorMarginValue &&
			this->autoAimEnabled == another.autoAimEnabled &&
			this->autoAimFov == another.autoAimFov &&
			this->aimStepEnabled == another.aimStepEnabled &&
			this->aimStepMin == another.aimStepMin &&
			this->aimStepMax == another.aimStepMax &&
			this->rcsEnabled == another.rcsEnabled &&
			this->rcsAlwaysOn == another.rcsAlwaysOn &&
			this->rcsAmountX == another.rcsAmountX &&
			this->rcsAmountY == another.rcsAmountY &&
			this->autoPistolEnabled == another.autoPistolEnabled &&
			this->autoShootEnabled == another.autoShootEnabled &&
			this->autoScopeEnabled == another.autoScopeEnabled &&
			this->noShootEnabled == another.noShootEnabled &&
			this->ignoreJumpEnabled == another.ignoreJumpEnabled &&
			this->ignoreEnemyJumpEnabled == another.ignoreEnemyJumpEnabled &&
			this->smokeCheck == another.smokeCheck &&
			this->flashCheck == another.flashCheck &&
			this->spreadLimitEnabled == another.spreadLimitEnabled &&
			this->spreadLimit == another.spreadLimit &&
			this->autoWallEnabled == another.autoWallEnabled &&
			this->autoWallValue == another.autoWallValue &&
			this->autoSlow == another.autoSlow &&
			this->predEnabled == another.predEnabled &&
			this->autoAimRealDistance == another.autoAimRealDistance &&
			this->scopeControlEnabled == another.scopeControlEnabled;
	}
} const defaultSettings{};

class ColorVar
{
public:
	ImColor color;
	bool rainbow;
	float rainbowSpeed;

	ColorVar() {}

	ColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
	}

	ImColor Color()
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : this->color;
		result.Value.w = this->color.Value.w;
		return result;
	}
};

class HealthColorVar : public ColorVar
{
public:
	bool hp;

	HealthColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->hp = false;
	}

	ImColor Color(C_BasePlayer* player)
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : (this->hp ? Color::ToImColor(Util::GetHealthColor(player)) : this->color);
		result.Value.w = this->color.Value.w;
		return result;
	}
};

namespace Settings
{
	namespace UI
	{
		inline ColorVar mainColor = ImColor(0, 87, 12, 255 );
		inline ColorVar bodyColor = ImColor( 0, 0, 0, 255 );
		inline ColorVar fontColor = ImColor( 255, 0, 0, 255 );
		inline ColorVar accentColor = ImColor( 255, 255, 255, 255 );

        /* Window Position/Size Defaults */
        namespace Windows
		{
			namespace Colors
			{
				inline int posX = 540;
				inline int posY = 325;
				inline int sizeX = 540;
				inline int sizeY = 325;
				inline bool open = false;
				inline bool reload = false ; // True on config load, used to change Window Properties.
			}
			namespace Config
			{
				inline int posX = 185;
				inline int posY = 250;
				inline int sizeX = 185;
				inline int sizeY = 250;
				inline bool open = false;
				inline bool reload = false;
			}
			namespace Main
			{
				inline int posX = 20;
				inline int posY = 20;
				inline int sizeX = 960;
				inline int sizeY = 645;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Skinmodel
			{
				inline int posX = 20;
				inline int posY = 20;
				inline int sizeX = 960;
				inline int sizeY = 645;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Spectators
			{
				inline int posX = 50;
				inline int posY = 100;
				inline int sizeX = 50;
				inline int sizeY = 100;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
		}
		namespace Fonts
		{
			namespace ESP
			{
				inline char* family = (char*)"Segoe UI";
				inline int size = 12;
				inline int flags = (int)FontFlags::FONTFLAG_OUTLINE;
			}
		}
	}

	namespace Aimbot
	{
		inline bool enabled = false;
        inline bool silent = false;
        inline bool friendly = false;
        inline Bone bone = BONE_HEAD;
        inline ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
        inline bool aimkeyOnly = false;

		namespace Smooth
		{
			inline bool enabled = false;
            inline float value = 0.5f;
            inline SmoothType type = SmoothType::SLOW_END;

			namespace Salting
			{
				inline bool enabled = false;
                inline float multiplier = 0.0f;
			}
		}

		namespace ErrorMargin
		{
			inline bool enabled = false;
			inline float value = 0.0f;
		}

		namespace AutoAim
		{
			inline bool enabled = false;
            inline float fov = 180.0f;
            inline bool realDistance = false;
            inline bool closestBone = false;
            inline bool desiredBones[] = {true, true, true, true, true, true, true, // center mass
                                          false, false, false, false, false, false, false, // left arm
                                          false, false, false, false, false, false, false, // right arm
                                          false, false, false, false, false, // left leg
                                          false, false, false, false, false  // right leg
            };
            inline bool engageLock = false;
            inline bool engageLockTR = false; // Target Reacquisition ( re-target after getting a kill when spraying ).
            inline int engageLockTTR = 700; // Time to Target Reacquisition in ms
		}

		namespace AutoWall
		{
			inline bool enabled = false;
			inline float value = 10.0f;
		}

		namespace AimStep
		{
			inline bool enabled = false;
			inline float min = 25.0f;
			inline float max = 35.0f;
		}

		namespace RCS
		{
			inline bool enabled = false;
			inline bool always_on = false;
			inline float valueX = 2.0f;
			inline float valueY = 2.0f;
		}

		namespace AutoPistol
		{
			inline bool enabled = false;
		}

		namespace AutoShoot
		{
			inline bool enabled = false;
			inline bool velocityCheck = false;
			inline bool autoscope = false;
		}

		namespace AutoCrouch
		{
			inline bool enabled = false;
		}

		namespace AutoSlow
		{
			inline bool enabled = false;
		}

		namespace NoShoot
		{
			inline bool enabled = false;
		}

		namespace IgnoreJump
		{
			inline bool enabled = false;
		}

		namespace IgnoreEnemyJump
		{
			inline bool enabled = false;
		}

		namespace SmokeCheck
		{
			inline bool enabled = false;
		}

		namespace FlashCheck
		{
			inline bool enabled = false;
		}

		namespace SpreadLimit
		{
			inline bool enabled = false;
			inline float value = 0.1f;
		}

		namespace Prediction
		{
			inline bool enabled = false;
		}

		namespace ScopeControl
		{
			inline bool enabled = false;
		}

		inline std::unordered_map<ItemDefinitionIndex, AimbotWeapon_t, Util::IntHash<ItemDefinitionIndex>> weapons = {
                { ItemDefinitionIndex::INVALID, defaultSettings },
        };
	}

	namespace ESP
	{
		inline bool enabled = false;
        inline DrawingBackend backend = DrawingBackend::IMGUI;
		inline ButtonCode_t key = ButtonCode_t::KEY_Z;
		inline TeamColorType teamColorType = TeamColorType::RELATIVE;
        inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
        inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
        inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
        inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
        inline HealthColorVar tColor = ImColor(255, 0, 0, 255);
        inline HealthColorVar tVisibleColor = ImColor(255, 255, 0, 255);
        inline HealthColorVar ctColor = ImColor(0, 0, 255, 255);
        inline HealthColorVar ctVisibleColor = ImColor(0, 255, 0, 255);
        inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
        inline ColorVar bombColor = ImColor(156, 39, 176, 255);
        inline ColorVar bombDefusingColor = ImColor(213, 0, 249, 255);
        inline ColorVar hostageColor = ImColor(121, 85, 72, 255);
        inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
        inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
        inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
        inline ColorVar fishColor = ImColor(255, 255, 255, 255);
        inline ColorVar smokeColor = ImColor(97, 97, 97, 255);
        inline ColorVar decoyColor = ImColor(2255, 152, 0, 255);
        inline ColorVar flashbangColor = ImColor(255, 235, 59, 255);
        inline ColorVar grenadeColor = ImColor(244, 67, 54, 255);
        inline ColorVar molotovColor = ImColor(205, 32, 31, 255);
        inline ColorVar mineColor = ImColor(205, 32, 31, 255);
        inline ColorVar chargeColor = ImColor(205, 32, 31, 255);
        inline ColorVar allyInfoColor = ImColor(255, 255, 255, 255);
        inline ColorVar enemyInfoColor = ImColor(255, 255, 255, 255);

		namespace Glow
		{
			inline bool enabled = false;
            inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
            inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
            inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
            inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
            inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
            inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
            inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
            inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
		}

		namespace Filters
		{
			inline bool legit = false;
			inline bool visibilityCheck = false;
			inline bool smokeCheck = false;
			//inline bool flashCheck = false;
			inline bool enemies = false;
			inline bool allies = false;
			inline bool bomb = false;
			inline bool hostages = false;
			inline bool defusers = false;
			inline bool weapons = false;
			inline bool chickens = false;
			inline bool fishes = false;
			inline bool throwables = false;
			inline bool localplayer = false;
		}

		namespace Info
		{
			inline bool name = false;
			inline bool clan = false;
			inline bool steamId = false;
			inline bool rank = false;
			inline bool health = false;
			inline bool armor = false;
			inline bool weapon = false;
			inline bool scoped = false;
			inline bool reloading = false;
			inline bool flashed = false;
			inline bool planting = false;
			inline bool hasDefuser = false;
			inline bool defusing = false;
			inline bool grabbingHostage = false;
			inline bool rescuing = false;
			inline bool location = false;
			inline bool money = false;
		}

		namespace Skeleton
		{
			inline bool enabled = false;
            inline ColorVar allyColor = ImColor(255, 255, 255, 255);
            inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
		}

		namespace Boxes
		{
			inline bool enabled = false;
			inline BoxType type = BoxType::FRAME_2D;
		}

		namespace Sprite
		{
			inline bool enabled = false;
			inline SpriteType type = SpriteType::SPRITE_TUX;
		}

		namespace Bars
		{
			inline bool enabled = false;
			inline BarType type = BarType::HORIZONTAL;
			inline BarColorType colorType = BarColorType::HEALTH_BASED;
		}

		namespace Tracers
		{
			inline bool enabled = false;
			inline TracerType type = TracerType::BOTTOM;
		}

		namespace BulletTracers
		{
			inline bool enabled = false;
		}

		namespace Bomb
		{
			inline bool enabled = false;
		}

		namespace FOVCrosshair
		{
			inline bool enabled = false;
			inline bool filled = false;
			inline ColorVar color = ImColor(255, 0, 0, 255);
		}

		// sound esp
		namespace Sounds
		{
			inline bool enabled = false;
			inline int time = 1000;
		}

		namespace Hitmarker
		{
			inline bool enabled = false;
			inline bool enemies = false;
			inline bool allies = false;
			inline ColorVar color = ImColor(255, 0, 0, 255);
			inline int duration = 2000;
			inline int size = 16;
			inline int innerGap = 5;

			namespace Damage
			{
				inline bool enabled = false;
			}
			namespace Sounds {
				inline bool enabled = false;
				inline Sound sound = Sound::NONE;
			}
		}

		namespace HeadDot
		{
			inline bool enabled = false;
			inline float size = 2.0f;
		}

		namespace Spread
		{
			inline bool enabled = false; // show current spread
			inline bool spreadLimit = false; // show spreadLimit value
			inline ColorVar color = ImColor(15, 200, 45, 255);
			inline ColorVar spreadLimitColor = ImColor(20, 5, 150, 255);
		}

		namespace DangerZone
		{
			inline int drawDist = 2000;
			inline bool drawDistEnabled = false;
			inline bool upgrade = false;
			inline bool lootcrate = false;
			inline bool barrel = false;
			inline bool ammobox = false;
			inline bool safe = false;
			inline bool dronegun = false;
			inline bool drone = false;
			inline bool cash = false;
			inline bool tablet = false;
			inline bool healthshot = false;
			inline bool melee = false;
            inline ColorVar upgradeColor = ImColor(255, 0, 0, 255);
            inline ColorVar lootcrateColor = ImColor(255, 0, 0, 255);
            inline ColorVar barrelColor = ImColor(255, 0, 0, 255);
            inline ColorVar ammoboxColor = ImColor(255, 0, 0, 255);
            inline ColorVar safeColor = ImColor(255, 0, 0, 255);
            inline ColorVar dronegunColor = ImColor(255, 0, 0, 255);
            inline ColorVar droneColor = ImColor(255, 0, 0, 255);
            inline ColorVar cashColor = ImColor(255, 0, 0, 255);
            inline ColorVar tabletColor = ImColor(255, 0, 0, 255);
            inline ColorVar healthshotColor = ImColor(255, 0, 0, 255);
            inline ColorVar meleeColor = ImColor(255, 0, 0, 255);
		}
	}

	namespace Skinchanger
	{
		namespace Skins
		{
			inline bool enabled = true;
			inline bool perTeam = false;
		}

		namespace Models
		{
			inline bool enabled = true;
		}

		inline std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsCT = {
                { ItemDefinitionIndex::WEAPON_AK47 /*WeaponID*/, { ItemDefinitionIndex::INVALID /*itemDefinitionIndex*/, 524 /*fallbackPaintKit*/, 0.0005f /*fallbackWear*/, -1 /*fallbackSeed*/, 1337/*fallbackStatTrak*/, -1/*fallbackEntityQuality*/, "TestTux"/*customName*/ } },
                { ItemDefinitionIndex::WEAPON_KNIFE, { ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, -1, -1, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_CT_SIDE, { ItemDefinitionIndex::GLOVE_SPECIALIST, 10006, 0.0005f, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_T_SIDE, { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, 10006, 0.0005f, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, { ItemDefinitionIndex::INVALID, 10006, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPORTY, { ItemDefinitionIndex::INVALID, 10018, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SLICK, { ItemDefinitionIndex::INVALID, 10013, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_MOTORCYCLE, { ItemDefinitionIndex::INVALID, 10024, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_LEATHER_WRAP, { ItemDefinitionIndex::INVALID, 10009, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPECIALIST, { ItemDefinitionIndex::INVALID, 10033, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, 1337, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_FLIP, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GUT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_PUSH, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_URSUS, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_STILETTO, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_CSS, { ItemDefinitionIndex::INVALID, -1, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GHOST, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                //{ ItemDefinitionIndex::WEAPON_KNIFEGG, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_USP_SILENCER, { ItemDefinitionIndex::INVALID, 2, 0.0005f, -1, -1, -1, ""} },
        };
		inline std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsT = {
                { ItemDefinitionIndex::WEAPON_AK47 /*WeaponID*/, { ItemDefinitionIndex::INVALID /*itemDefinitionIndex*/, 524 /*fallbackPaintKit*/, 0.0005f /*fallbackWear*/, -1 /*fallbackSeed*/, 1337/*fallbackStatTrak*/, -1/*fallbackEntityQuality*/, "TestTux"/*customName*/ } },
                { ItemDefinitionIndex::WEAPON_KNIFE_T, { ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, -1, -1, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_T_SIDE, { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, 10006, 0.0005f, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, { ItemDefinitionIndex::INVALID, 10006, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPORTY, { ItemDefinitionIndex::INVALID, 10018, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SLICK, { ItemDefinitionIndex::INVALID, 10013, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_MOTORCYCLE, { ItemDefinitionIndex::INVALID, 10024, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_LEATHER_WRAP, { ItemDefinitionIndex::INVALID, 10009, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPECIALIST, { ItemDefinitionIndex::INVALID, 10033, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, 1337, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_FLIP, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GUT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_PUSH, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_URSUS, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_STILETTO, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_CSS, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GHOST, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                //{ ItemDefinitionIndex::WEAPON_KNIFEGG, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
        };
	}

	namespace NoSky
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor(0, 0, 0, 255);
	}

	namespace NoScopeBorder
	{
		inline bool enabled = false;
	}

	namespace ScreenshotCleaner
	{
		inline bool enabled = false;
	}

	namespace ThirdPerson
	{
		inline bool enabled = false;
		inline float distance = 30.0f;
        inline ShowedAngle type = ShowedAngle::REAL;
	}

    namespace Debug
    {
        namespace AutoWall
        {
            inline bool debugView = false;
        }
		namespace AutoAim
		{
			inline bool drawTarget = false;
			inline Vector target = {0,0,0};
		}
		namespace BoneMap
		{
			inline bool draw = false;
			inline bool justDrawDots = false;
			inline int modelID = 1253; // in econItemMap, not itemdefindex
		}
		namespace AnimLayers
		{
			inline bool draw = false;
		}
    }

	void LoadDefaultsOrSave(std::string path);
	void LoadConfig(std::string path);
	void LoadSettings();
	void DeleteConfig(std::string path);
	void SaveGrenadeInfo(std::string path);
	void LoadGrenadeInfo(std::string path);
}
