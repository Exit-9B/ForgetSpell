#pragma once

namespace Offset
{
#ifndef SKYRIMVR
	namespace BSResourceNiBinaryStream
	{
		// SkyrimSE 1.6.318.0: 0xC9EE90
		inline constexpr REL::ID Read(71027);
	}

	namespace BSScaleformTranslator
	{
		// SkyrimSE 1.6.318.0: 0xC4F6B0
		inline constexpr REL::ID GetCachedString(69188);
	}

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

	namespace BSResourceNiBinaryStream
	{
		inline constexpr REL::Offset Read(0xCBCBE0);
	}

	namespace BSScaleformTranslator
	{
		inline constexpr REL::Offset GetCachedString(0xC6E830);
	}

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
