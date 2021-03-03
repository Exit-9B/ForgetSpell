#include "MagicMenuManager.h"
#include <xbyak/xbyak.h>

constexpr REL::ID MagicMenu_ProcessInput{ 51148 };

RE::MessageBoxData* MakeMessageBox(const char* a_message)
{
	auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
	auto uiStrHolder = RE::InterfaceStrings::GetSingleton();

	if (factoryManager && uiStrHolder)
	{
		auto factory = factoryManager->GetCreator<RE::MessageBoxData>(uiStrHolder->messageBoxData);
		auto messageBox = factory ? factory->Create() : nullptr;
		if (messageBox)
		{
			messageBox->unk4C = 4;
			messageBox->unk38 = 10;
			messageBox->bodyText = a_message;

			return messageBox;
		}
	}

	return nullptr;
}

void QueueMessage(RE::MessageBoxData* a_messageBox)
{
	using func_t = decltype(&QueueMessage);
	REL::Relocation<func_t> func{ REL::ID{ 51422 } };
	return func(a_messageBox);
}

void UpdateList(RE::MagicMenu* a_menu)
{
	using func_t = decltype(&UpdateList);
	REL::Relocation<func_t> func{ REL::ID{ 51163 } };
	return func(a_menu);
}

MagicMenuManager::ForgetSpellConfirmCallback::ForgetSpellConfirmCallback(
	RE::SpellItem* a_spell) :
	spell(a_spell)
{
}

void MagicMenuManager::ForgetSpellConfirmCallback::Run(Message a_msg)
{
	int response = static_cast<int>(a_msg) - 4;
	if (response == 0)
	{
		ForgetSpell(spell);
	}
}

void MagicMenuManager::InstallHooks()
{
	REL::Relocation<std::uintptr_t> hook{ MagicMenu_ProcessInput, 0x2E3 };

	struct Patch : Xbyak::CodeGenerator
	{
		Patch(std::uintptr_t a_hookAddr, std::uintptr_t a_funcAddr)
		{
			Xbyak::Label funcLbl;
			Xbyak::Label retnLbl;

			mov(rcx, r15);
			call(ptr[rip + funcLbl]);

			cmp(byte[r15 + 0x1A], 0x77); // original instruction
			jmp(ptr[rip + retnLbl]);

			L(funcLbl);
			dq(a_funcAddr);

			L(retnLbl);
			dq(a_hookAddr + 5);
		}
	};

	auto funcAddr = SKSE::unrestricted_cast<std::uintptr_t>(TryForgetSpell);
	Patch patch{ hook.address(), funcAddr };

	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_branch<5>(hook.address(), trampoline.allocate(patch));

	logger::info("Installed hook for forget spell button"sv);
}

void MagicMenuManager::TryForgetSpell(RE::TESForm* a_item)
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
		RE::PlaySound("UIJournalClose");

		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::MagicMenu>() : nullptr;

		if (menu)
		{
			UpdateList(menu.get());
		}
	}
}

void MagicMenuManager::ShowConfirmationDialog(RE::SpellItem* a_spell)
{
	char message[200];
	const char* format = "Do you want to forget %s?";
	const char* name = a_spell->GetFullName();
	snprintf(message, 200, format, name);

	if (strlen(message) == 0)
	{
		ForgetSpell(a_spell);
		return;
	}

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

				QueueMessage(messageBox);
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
				QueueMessage(messageBox);
			}
		}
	}
}