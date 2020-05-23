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

	Fonts::SetupFonts();

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
