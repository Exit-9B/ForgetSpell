#pragma once

namespace Offset
{
#ifndef SKYRIMVR
	constexpr REL::ID MagicMenu_ProcessInput{ 51148 };
	constexpr REL::ID MagicMenu_UpdateList{ 51163 };
	constexpr REL::ID MessageBoxData_QueueMessage{ 51422 };
	constexpr REL::ID BSScaleformTranslator_GetCachedString{ 67844 };
	constexpr REL::ID BSResourceNiBinaryStream_Read{ 69647 };
#else
	constexpr REL::Offset MagicMenu_ProcessInput{ 0x008C8AF0 };
	constexpr REL::Offset MagicMenu_UpdateList{ 0x008CB450 };
	constexpr REL::Offset MessageBoxData_QueueMessage{ 0x008D8420 };
	constexpr REL::Offset BSScaleformTranslator_GetCachedString{ 0x00C6E830 };
	constexpr REL::Offset BSResourceNiBinaryStream_Read{ 0x00CBCBE0 };
#endif
}
