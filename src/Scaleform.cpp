#include "Scaleform.h"

void Scaleform::InstallHooks()
{
#ifndef SKYRIMVR
		auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::BSScaleformManager::LoadMovie,
			0x1DD);
#else
		auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::BSScaleformManager::LoadMovie.address() + 0x1D9);
#endif

	if (!REL::make_pattern<"FF 15">().match(hook.address())) {
		logger::critical("Failed to install Scaleform hook");
		return;
	}

	auto& trampoline = SKSE::GetTrampoline();
	auto ptr = trampoline.write_call<6>(hook.address(), &AddScaleformHooks);
	_SetViewScaleMode = *reinterpret_cast<std::uintptr_t*>(ptr);
}

class UpdateBottomBar : public RE::GFxFunctionHandler
{
	static bool Get(RE::GFxValue* a_result, const RE::GFxValue& a_obj, const char* a_name)
	{
		if (a_obj.IsObject()) {
			a_obj.GetMember(a_name, a_result);
			return true;
		}
		else {
			return false;
		}
	}

	static bool Get(RE::GFxValue* a_result, const RE::GFxValue& a_arr, std::integral auto a_idx)
	{
		const std::uint32_t idx = static_cast<std::uint32_t>(a_idx);
		if (a_arr.IsArray() && idx < a_arr.GetArraySize()) {
			a_arr.GetElement(idx, a_result);
			return true;
		}
		else {
			return false;
		}
	}

	template <typename Arg, typename... Rest>
	static bool Get(RE::GFxValue* a_result, const RE::GFxValue& a_val, Arg a_arg, Rest... a_rest)
	{
		if (RE::GFxValue next; Get(&next, a_val, a_arg)) {
			return Get(a_result, next, a_rest...);
		}
		else {
			return false;
		}
	}

	static RE::GFxValue CreateButton(
		RE::GFxMovie* a_movie,
		const wchar_t* a_text,
		const RE::GFxValue& a_controls)
	{
		RE::GFxValue button;
		a_movie->CreateObject(&button);
		button.SetMember("text", a_text);
		button.SetMember("controls", a_controls);
		return button;
	}

	void Call(Params& a_params) override
	{
		RE::GFxValue a_bSelected;
		if (a_params.argCount >= 1) {
			a_bSelected = a_params.args[0];
		}
		const bool bSelected = a_bSelected.IsBool() && a_bSelected.GetBool();

		RE::GFxValue navPanel;
		a_params.thisPtr->GetMember("navPanel", &navPanel);
		if (!navPanel.IsObject()) {
			return;
		}

		navPanel.Invoke("clearButtons");

		RE::GFxValue inventoryLists;
		Get(&inventoryLists, *a_params.thisPtr, "inventoryLists");

		std::uint32_t uiFilterFlag = 0;
		if (RE::GFxValue filterFlag;
			Get(&filterFlag, inventoryLists, "itemList", "selectedEntry", "filterFlag")) {
			uiFilterFlag = static_cast<std::uint32_t>(filterFlag.GetNumber());
		}

		static constexpr std::uint32_t FILTERFLAG_MAGIC_ACTIVEEFFECTS = 0x00000100;

		if (bSelected && (uiFilterFlag & FILTERFLAG_MAGIC_ACTIVEEFFECTS) == 0) {
			RE::GFxValue Input;
			a_params.movie->GetVariable(&Input, "skyui.defines.Input");
			if (!Input.IsObject()) {
				return;
			}

			RE::GFxValue Equip, YButton, XButton;
			Input.GetMember("Equip", &Equip);
			Input.GetMember("YButton", &YButton);
			Input.GetMember("XButton", &XButton);

			RE::GFxValue equipButton = CreateButton(a_params.movie, L"$Equip", Equip);
			navPanel.Invoke("addButton", std::array{ equipButton });

			std::uint32_t uiFavoritesFlag = 0;
			if (RE::GFxValue flag;
				Get(&flag, inventoryLists, "categoryList", "entryList", 0, "flag")) {
				uiFavoritesFlag = static_cast<std::uint32_t>(flag.GetNumber());
			}

			if (uiFilterFlag & uiFavoritesFlag) {
				RE::GFxValue unfavoriteButton = CreateButton(
					a_params.movie,
					L"$Unfavorite",
					YButton);
				navPanel.Invoke("addButton", std::array{ unfavoriteButton });
			}
			else {
				RE::GFxValue favoriteButton = CreateButton(a_params.movie, L"$Favorite", YButton);
				navPanel.Invoke("addButton", std::array{ favoriteButton });
			}

			RE::GFxValue itemCard;
			a_params.thisPtr->GetMember("itemCard", &itemCard);
			if (RE::GFxValue showUnlocked;
				Get(&showUnlocked, itemCard, "itemInfo", "showUnlocked") &&
				showUnlocked.IsBool() && showUnlocked.GetBool()) {
				RE::GFxValue unlockButton = CreateButton(a_params.movie, L"$Unlock", XButton);
			}
			else if (RE::GFxValue type; Get(&type, itemCard, "itemInfo", "type")) {
				static constexpr double ICT_SPELL = 7;
				if (type.IsNumber() && type.GetNumber() == ICT_SPELL) {
					RE::GFxValue forgetButton = CreateButton(a_params.movie, L"$Forget", XButton);
					navPanel.Invoke("addButton", std::array{ forgetButton });
				}
			}
		}
		else {
			RE::GFxValue _cancelControls;
			a_params.thisPtr->GetMember("_cancelControls", &_cancelControls);
			RE::GFxValue exitButton = CreateButton(a_params.movie, L"$Exit", _cancelControls);
			navPanel.Invoke("addButton", std::array{ exitButton });

			RE::GFxValue _searchControls;
			a_params.thisPtr->GetMember("_searchControls", &_searchControls);
			RE::GFxValue searchButton = CreateButton(a_params.movie, L"$Search", _searchControls);
			navPanel.Invoke("addButton", std::array{ searchButton });

			if (RE::GFxValue _platform; Get(&_platform, *a_params.thisPtr, "_platform") &&
				_platform.IsNumber() && _platform.GetNumber() != 0) {
				RE::GFxValue _sortColumnControls;
				a_params.thisPtr->GetMember("_sortColumnControls", &_sortColumnControls);
				RE::GFxValue columnButton = CreateButton(
					a_params.movie,
					L"$Column",
					_sortColumnControls);

				RE::GFxValue _sortOrderControls;
				a_params.thisPtr->GetMember("_sortOrderControls", &_sortOrderControls);
				RE::GFxValue orderButton = CreateButton(
					a_params.movie,
					L"$Order",
					_sortOrderControls);
			}

			RE::GFxValue _switchControls;
			a_params.thisPtr->GetMember("_switchControls", &_switchControls);
			RE::GFxValue inventoryButton = CreateButton(
				a_params.movie,
				L"$Inventory",
				_switchControls);
		}

		navPanel.Invoke("updateButtons", std::to_array<RE::GFxValue>({ true }));
	}
};

void Scaleform::AddScaleformHooks(
	RE::GFxMovieView* a_view,
	RE::GFxMovieView::ScaleModeType a_scaleMode)
{
	_SetViewScaleMode(a_view, a_scaleMode);

	RE::GFxValue obj;
	a_view->GetVariable(&obj, "_global.MagicMenu.prototype");
	if (!obj.IsObject()) {
		return;
	}

	RE::GFxValue func;
	auto impl = RE::make_gptr<UpdateBottomBar>();
	a_view->CreateFunction(&func, impl.get());
	obj.SetMember("updateBottomBar", func);
}
