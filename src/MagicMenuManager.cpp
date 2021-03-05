#include "MagicMenuManager.h"
#include "Misc.h"
#include "Offsets.h"
#include <xbyak/xbyak.h>

MagicMenuManager::ForgetSpellConfirmCallback::ForgetSpellConfirmCallback(
	RE::SpellItem* a_spell) :
	spell(a_spell)
{
}

void MagicMenuManager::ForgetSpellConfirmCallback::Run(Message a_msg)
{
	std::int32_t response = static_cast<std::int32_t>(a_msg) - 4;
	if (response == 0)
	{
		ForgetSpell(spell);
	}
}

void MagicMenuManager::InstallHooks()
{
	REL::Relocation<std::uintptr_t> hook{ Offset::MagicMenu_ProcessInput, 0x2E3 };

	struct Patch : Xbyak::CodeGenerator
	{
		Patch(std::uintptr_t a_hookAddr, std::uintptr_t a_funcAddr)
		{
			Xbyak::Label funcLbl;
			Xbyak::Label retnLbl;

			mov(rcx, r15);
			call(ptr[rip + funcLbl]);

			cmp(byte[r15 + 0x1A], 0x77); //< original instruction
			jmp(ptr[rip + retnLbl]);

			L(funcLbl);
			dq(a_funcAddr);

			L(retnLbl);
			dq(a_hookAddr + 0x5);
		}
	};

	auto funcAddr = SKSE::unrestricted_cast<std::uintptr_t>(StartForgetSpell);
	Patch patch{ hook.address(), funcAddr };

	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_branch<5>(hook.address(), trampoline.allocate(patch));

	logger::info("Installed hook for forget spell button"sv);
}

void MagicMenuManager::StartForgetSpell(RE::TESForm* a_item)
{
	if (a_item && a_item->formType == RE::FormType::Spell)
	{
		auto spell = skyrim_cast<RE::SpellItem*>(a_item);
		if (spell && spell->GetSpellType() == RE::MagicSystem::SpellType::kSpell)
		{
			auto playerRef = RE::PlayerCharacter::GetSingleton();
			auto player = playerRef ? playerRef->GetActorBase() : nullptr;
			auto spellData = player ? player->actorEffects : nullptr;
			std::uint32_t numSpells = spellData ? spellData->numSpells : 0;
			for (std::uint32_t i = 0; i < numSpells; i++)
			{
				auto actorSpell = spellData->spells[i];
				if (actorSpell == spell)
				{
					ShowErrorDialog(spell);
					return;
				}
			}

			ShowConfirmationDialog(spell);
		}
	}
}

void MagicMenuManager::ForgetSpell(RE::SpellItem* a_spell)
{
	auto playerRef = RE::PlayerCharacter::GetSingleton();

	if (playerRef)
	{
		playerRef->RemoveSpell(a_spell);
		for (auto effect : a_spell->effects)
		{
			auto equipAbility = effect->baseEffect->data.equipAbility;
			if (equipAbility)
			{
				playerRef->RemoveSpell(equipAbility);
			}
		}
		RE::PlaySound("UIJournalClose");

		UpdateMagicMenu();
	}
}

void MagicMenuManager::ShowConfirmationDialog(RE::SpellItem* a_spell)
{
	char message[200];
	const char* format = "Do you want to forget %s?";
	const char* name = a_spell->GetFullName();
	snprintf(message, 200, format, name);

	auto messageBox = MakeMessageBox(message);
	if (messageBox)
	{
		auto gameSettings = RE::GameSettingCollection::GetSingleton();
		if (gameSettings)
		{
			auto sYesText = gameSettings->GetSetting("sYesText");
			auto sNoText = gameSettings->GetSetting("sNoText");
			if (sYesText && sNoText)
			{
				messageBox->buttonText.push_back(sYesText->GetString());
				messageBox->buttonText.push_back(sNoText->GetString());

				messageBox->callback = RE::BSTSmartPointer<RE::IMessageBoxCallback>{
					new ForgetSpellConfirmCallback{ a_spell }
				};

				MessageBoxData_QueueMessage(messageBox);
			}
		}
	}
}

void MagicMenuManager::ShowErrorDialog(RE::SpellItem* a_spell)
{
	char message[200];
	const char* format = "You cannot forget %s.";
	const char* name = a_spell->GetFullName();
	snprintf(message, 200, format, name);

	auto messageBox = MakeMessageBox(message);
	if (messageBox)
	{
		auto gameSettings = RE::GameSettingCollection::GetSingleton();
		if (gameSettings)
		{
			auto sOKText = gameSettings->GetSetting("sOKText");
			if (sOKText)
			{
				messageBox->buttonText.push_back(sOKText->GetString());
				MessageBoxData_QueueMessage(messageBox);
			}
		}
	}
}

void MagicMenuManager::UpdateMagicMenu()
{
	auto ui = RE::UI::GetSingleton();
	auto menu = ui ? ui->GetMenu<RE::MagicMenu>() : nullptr;

	if (menu)
	{
		MagicMenu_UpdateList(menu.get());
	}
}