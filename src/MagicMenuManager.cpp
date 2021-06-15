#include "MagicMenuManager.h"
#include "Misc.h"
#include "Offsets.h"
#include "Settings.h"
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

			bool canForgetStartingSpells = Settings::GetSingleton()->CanForgetStartingSpells;

			if (canForgetStartingSpells || !IsStartingSpell(playerRef, spell))
			{
				ShowConfirmationDialog(spell);
			}
			else
			{
				ShowErrorDialog(spell);
			}
		}
	}
}

void MagicMenuManager::ForgetSpell(RE::SpellItem* a_spell)
{
	auto playerRef = RE::PlayerCharacter::GetSingleton();

	if (playerRef)
	{
		if (IsStartingSpell(playerRef, a_spell))
		{
			RemoveStartingSpell(playerRef, a_spell);
		}
		else
		{
			playerRef->RemoveSpell(a_spell);
		}

		for (auto effect : a_spell->effects)
		{
			auto equipAbility = effect->baseEffect->data.equipAbility;
			if (equipAbility)
			{
				playerRef->RemoveSpell(equipAbility);
			}
		}

		auto& sound = Settings::GetSingleton()->ForgetSpellSound;
		if (!sound.empty())
		{
			RE::PlaySound(sound.c_str());
		}

		UpdateMagicMenu();
	}
}

bool MagicMenuManager::IsStartingSpell(RE::Actor* a_actor, RE::SpellItem* a_spell)
{
	if (!a_actor || !a_spell)
		return false;

	auto npc = a_actor->GetActorBase();
	auto spellData = npc ? npc->actorEffects : nullptr;
	if (!spellData)
		return false;

	std::uint32_t numSpells = spellData->numSpells;
	auto begin = spellData->spells;
	auto end = spellData->spells + numSpells;
	return std::find(begin, end, a_spell) != end;
}

void MagicMenuManager::RemoveStartingSpell(RE::Actor* a_actor, RE::SpellItem* a_spell)
{
	auto npc = a_actor->GetActorBase();
	auto spellData = npc ? npc->actorEffects : nullptr;
	if (!spellData)
		return;

	std::vector<RE::SpellItem*> spellsToCopy;
	spellsToCopy.reserve(static_cast<std::size_t>(spellData->numSpells));

	for (std::uint32_t i = 0; i < spellData->numSpells; i++)
	{
		auto spell = spellData->spells[i];
		if (spell != a_spell)
		{
			spellsToCopy.push_back(spell);
		}
	}

	auto newNumSpells = static_cast<std::uint32_t>(spellsToCopy.size());
	auto newSpells = RE::calloc<RE::SpellItem*>(newNumSpells);
	std::copy(spellsToCopy.cbegin(), spellsToCopy.cend(), newSpells);

	auto oldSpells = spellData->spells;

	spellData->numSpells = newNumSpells;
	spellData->spells = newSpells;

	RE::free(oldSpells);

	npc->AddChange(RE::TESNPC::ChangeFlags::kSpellList);
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