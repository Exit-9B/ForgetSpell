#pragma once

namespace Translation
{
	void GetCachedString(wchar_t** a_pOut, wchar_t* a_bufIn, std::uint32_t a_unused);

	auto Read(RE::BSResourceNiBinaryStream* a_this, void* a_dst, std::uint32_t a_len)
		-> std::uint32_t;

	auto ReadLine_w(
		RE::BSResourceNiBinaryStream* a_this,
		wchar_t* a_dst,
		std::uint32_t a_dstLen,
		std::uint32_t a_terminator) -> std::uint32_t;

	void ParseTranslation(const std::string& a_name);

	auto Translate(const std::string& a_key) -> std::string;
}
