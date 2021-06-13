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
	ini.LoadFile(R"(.\Data\SKSE\Plugins\ForgetSpell.ini)");

	ini.SetValue("ForgetSpell", nullptr, nullptr);

	settings->ForgetSpellSound = ini.GetValue("ForgetSpell", "sForgetSpellSound", "");
	settings->CanForgetStartingSpells = ini.GetBoolValue("ForgetSpell", "bCanForgetStartingSpells", true);
}