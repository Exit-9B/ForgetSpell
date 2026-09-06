#pragma once

class Scaleform final
{
public:
	static void InstallHooks();

private:
	static void AddScaleformHooks(
		RE::GFxMovieView* a_view,
		RE::GFxMovieView::ScaleModeType a_scaleMode);

	inline static REL::Relocation<decltype(&AddScaleformHooks)> _SetViewScaleMode;
};
