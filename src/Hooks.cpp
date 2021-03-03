#include "Hooks.h"
#include "MagicMenuManager.h"

void Hooks::Install()
{
	MagicMenuManager::InstallHooks();
	logger::info("Installed all hooks"sv);
}