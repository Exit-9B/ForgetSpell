#include "MagicMenuManager.h"
#include "Offsets.h"
#include "Patches.h"
#include "Settings.h"

void MagicMenuManager::InstallHooks()
{
	Patch::WriteForgetSpellPatch(StartForgetSpell);
}

void MagicMenuManager::StartForgetSpell(RE::TESForm* a_item)
{
	if (a_item && a_item->GetFormType() == RE::FormType::Spell) {
		auto spell = static_cast<RE::SpellItem*>(a_item);
		if (spell && spell->GetSpellType() == RE::MagicSystem::SpellType::kSpell) {
			auto playerRef = RE::PlayerCharacter::GetSingleton();

			bool canForgetStartingSpells = Settings::GetSingleton()->CanForgetStartingSpells;

			if (canForgetStartingSpells || !IsStartingSpell(playerRef, spell)) {
				ShowConfirmationDialog(spell);
			}
			else {
				ShowErrorDialog(spell);
			}
		}
	}
}

void MagicMenuManager::ForgetSpell(RE::SpellItem* a_spell)
{
	auto playerRef = RE::PlayerCharacter::GetSingleton();

	if (playerRef) {
		if (IsStartingSpell(playerRef, a_spell)) {
			RemoveStartingSpell(playerRef, a_spell);
		}
		else {
			playerRef->RemoveSpell(a_spell);
		}

		for (auto effect : a_spell->effects) {
			auto equipAbility = effect->baseEffect->data.equipAbility;
			if (equipAbility) {
				playerRef->RemoveSpell(equipAbility);
			}
		}

		auto& sound = Settings::GetSingleton()->ForgetSpellSound;
		if (!sound.empty()) {
			RE::PlaySound(sound.c_str());
		}

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::MagicMenu>() : nullptr;
		if (menu) {
			menu->UpdateList();
		}
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

	for (std::uint32_t i = 0; i < spellData->numSpells; i++) {
		auto spell = spellData->spells[i];
		if (spell != a_spell) {
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
	std::string message;
	SKSE::Translation::Translate(
		fmt::format("$FS_ConfirmForget{{{}}}"sv, a_spell->GetFullName()),
		message);

	const auto messageBox = new RE::MessageBoxData();
	if (!messageBox) {
		return;
	}

	messageBox->bodyText = message;
	messageBox->buttonText.push_back(*"sYesText"_gs);
	messageBox->buttonText.push_back(*"sNoText"_gs);

	messageBox->SetCallback(
		[a_spell](std::int8_t a_msg)
		{
			if (a_msg == 0) {
				ForgetSpell(a_spell);
			}
		});

	RE::MessageBoxMenu::Create(messageBox);
}

void MagicMenuManager::ShowErrorDialog(RE::SpellItem* a_spell)
{
	std::string message;
	SKSE::Translation::Translate(
		fmt::format("$FS_CannotForget{{{}}}"sv, a_spell->GetFullName()),
		message);

	const auto messageBox = new RE::MessageBoxData();
	if (!messageBox) {
		return;
	}

	messageBox->bodyText = message;
	if (const auto sOKText = "sOKText"_gs) {
		messageBox->buttonText.push_back(*sOKText);
		RE::MessageBoxMenu::Create(messageBox);
	}
}
