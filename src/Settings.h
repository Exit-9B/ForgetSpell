#pragma once

class Settings
{
public:
	static Settings* GetSingleton();

	static void LoadSettings();

	bool CanForgetStartingSpells;
	bool EnableButtonPrompt;
	std::string ForgetSpellSound;
};
