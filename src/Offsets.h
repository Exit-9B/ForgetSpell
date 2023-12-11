#pragma once

namespace Offset
{
#ifndef SKYRIMVR
	namespace MagicMenu
	{
		// SkyrimSE 1.6.318.0: 0x8CB690
		inline constexpr REL::ID ProcessInput(52028);
		// SkyrimSE 1.6.318.0: 0x8CD900
		inline constexpr REL::ID UpdateList(52043);
	}

	namespace MessageBoxData
	{
		// SkyrimSE 1.6.318.0: 0x8DAC20
		inline constexpr REL::ID QueueMessage(52271);
	}

#else

	namespace MagicMenu
	{
		inline constexpr REL::Offset HandleUnlockInput(0x8CBC10);
		inline constexpr REL::Offset UpdateList(0x8CB450);
	}

	namespace MessageBoxData
	{
		inline constexpr REL::Offset QueueMessage(0x8D8420);
	}
#endif
}
