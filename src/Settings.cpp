#include "Settings.h"
#include "SimpleIni.h"

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return &singleton;
}

void Settings::LoadSettings()
{
	auto settings = GetSingleton();

	CSimpleIniA ini;
	ini.SetUnicode();

	auto iniPath = fmt::format(R"(.\Data\SKSE\Plugins\{}.ini)"sv, Plugin::NAME);
	ini.LoadFile(iniPath.data());

	ini.SetValue("ForgetSpell", nullptr, nullptr);

	settings->ForgetSpellSound = ini.GetValue("ForgetSpell", "sForgetSpellSound", "");
	settings->CanForgetStartingSpells = ini.GetBoolValue("ForgetSpell", "bCanForgetStartingSpells", true);
}
