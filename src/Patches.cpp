#include "Patches.h"
#include "Offsets.h"

#pragma warning(disable : 4702)
#include <xbyak/xbyak.h>

#ifndef SKYRIMVR
bool Patch::WriteForgetSpellPatch(ForgetSpellCallback* a_callback)
{
	REL::Relocation<std::uintptr_t> hook{ Offset::MagicMenu::ProcessInput, 0x34F };

	struct Patch : Xbyak::CodeGenerator
	{
		Patch(std::uintptr_t a_hookAddr, std::uintptr_t a_funcAddr)
		{
			Xbyak::Label funcLbl;
			Xbyak::Label retnLbl;

			mov(rcx, r15);
			call(ptr[rip + funcLbl]);

			cmp(byte[r15 + 0x1A], 0x77);  // original instruction
			jmp(ptr[rip + retnLbl]);

			L(funcLbl);
			dq(a_funcAddr);

			L(retnLbl);
			dq(a_hookAddr + 0x5);
		}
	};

	auto funcAddr = reinterpret_cast<std::uintptr_t>(a_callback);
	Patch patch{ hook.address(), funcAddr };

	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_branch<5>(hook.address(), trampoline.allocate(patch));

	logger::info("Installed hook for forget spell button"sv);
	return true;
}

#else

bool Patch::WriteForgetSpellPatch(ForgetSpellCallback* a_callback)
{
	std::uintptr_t hookAddr{ Offset::MagicMenu::HandleUnlockInput.address() + 0x41 };

	struct Patch : Xbyak::CodeGenerator
	{
		Patch(std::uintptr_t a_hookAddr, std::uintptr_t a_funcAddr)
		{
			Xbyak::Label funcLbl;
			Xbyak::Label retnLbl;
			Xbyak::Label notShoutLbl;

			jz(notShoutLbl);              // original instruction

			mov(rcx, rsi);
			call(ptr[rip + funcLbl]);

			cmp(byte[rsi + 0x1A], 0x77);  // original instruction
			jmp(ptr[rip + retnLbl]);

			L(funcLbl);
			dq(a_funcAddr);

			L(notShoutLbl);
			dq(a_hookAddr + 0x29D);

			L(retnLbl);
			dq(a_hookAddr + 0xA);
		}
	};

	auto funcAddr = reinterpret_cast<std::uintptr_t>(a_callback);
	Patch patch{ hookAddr, funcAddr };

	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_branch<5>(hookAddr, trampoline.allocate(patch));

	logger::info("Installed hook for forget spell button"sv);
	return true;
}
#endif
