#pragma once

RE::MessageBoxData* MakeMessageBox(const std::string& a_message);

void MessageBoxData_QueueMessage(RE::MessageBoxData* a_messageBox);

void MagicMenu_UpdateList(RE::MagicMenu* a_menu);
