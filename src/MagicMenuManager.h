#pragma once

class MagicMenuManager
{
public:
	class ForgetSpellConfirmCallback : public RE::IMessageBoxCallback
	{
	public:
		ForgetSpellConfirmCallback(RE::SpellItem* a_spell);

		virtual ~ForgetSpellConfirmCallback() = default;

		virtual void Run(Message a_msg) override;

		RE::SpellItem* spell;
	};

	static void InstallHooks();

private:
	static void TryForgetSpell(RE::TESForm* a_item);
	static void ForgetSpell(RE::SpellItem* a_spell);
	static void ShowConfirmationDialog(RE::SpellItem* a_spell);
	static void ShowErrorDialog(RE::SpellItem* a_spell);
};