#include "Misc.h"
#include "Offsets.h"

RE::MessageBoxData* MakeMessageBox(const std::string& a_message)
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

void MessageBoxData_QueueMessage(RE::MessageBoxData* a_messageBox)
{
	using func_t = decltype(&MessageBoxData_QueueMessage);
	REL::Relocation<func_t> func{ Offset::MessageBoxData::QueueMessage };
	return func(a_messageBox);
}

void MagicMenu_UpdateList(RE::MagicMenu* a_menu)
{
	using func_t = decltype(&MagicMenu_UpdateList);
	REL::Relocation<func_t> func{ Offset::MagicMenu::UpdateList };
	return func(a_menu);
}
