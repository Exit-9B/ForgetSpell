#pragma once

namespace RE::Offset
{
#ifndef SKYRIMVR
	namespace MagicMenu
	{
		inline constexpr REL::ID ProcessInput(52028);
	}
#else
	namespace MagicMenu
	{
		inline constexpr REL::Offset HandleUnlockInput(0x8CBC10);
	}
#endif
}
