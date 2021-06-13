#pragma once

class Settings
{
public:
	static Settings* GetSingleton();

	static void LoadSettings();

	bool CanForgetStartingSpells;
	std::string ForgetSpellSound;
};