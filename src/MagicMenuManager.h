#pragma once

class MagicMenuManager
{
public:
	static void InstallHooks();

private:
	static void StartForgetSpell(RE::TESForm* a_item);
	static void ForgetSpell(RE::SpellItem* a_spell);
	static bool IsStartingSpell(RE::Actor* a_actor, RE::SpellItem* a_spell);
	static void RemoveStartingSpell(RE::Actor* a_actor, RE::SpellItem* a_spell);
	static void ShowConfirmationDialog(RE::SpellItem* a_spell);
	static void ShowErrorDialog(RE::SpellItem* a_spell);
};
