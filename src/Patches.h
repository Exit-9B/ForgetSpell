#pragma once

namespace Patch
{
	using ForgetSpellCallback = void(RE::TESForm*);

	bool WriteForgetSpellPatch(ForgetSpellCallback* a_callback);
}
