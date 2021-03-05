#include "Misc.h"

void MessageBoxData_QueueMessage(RE::MessageBoxData* a_messageBox)
{
	using func_t = decltype(&MessageBoxData_QueueMessage);
	REL::Relocation<func_t> func{ REL::ID{ 51422 } };
	return func(a_messageBox);
}

void MagicMenu_UpdateList(RE::MagicMenu* a_menu)
{
	using func_t = decltype(&MagicMenu_UpdateList);
	REL::Relocation<func_t> func{ REL::ID{ 51163 } };
	return func(a_menu);
}
