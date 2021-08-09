#include "Translation.h"
#include "Encoding.h"
#include "Offsets.h"

void Translation::GetCachedString(wchar_t** a_pOut, wchar_t* a_bufIn, std::uint32_t a_unused)
{
	using func_t = decltype(&Translation::GetCachedString);
	REL::Relocation<func_t> func{ Offset::BSScaleformTranslator_GetCachedString };
	return func(a_pOut, a_bufIn, a_unused);
}

auto Translation::Read(RE::BSResourceNiBinaryStream* a_this, void* a_dst, std::uint32_t a_len)
	-> std::uint32_t
{
	using func_t = decltype(&Translation::Read);
	REL::Relocation<func_t> func{ Offset::BSResourceNiBinaryStream_Read };
	return func(a_this, a_dst, a_len);
}

auto Translation::ReadLine_w(
	RE::BSResourceNiBinaryStream* a_this,
	wchar_t* a_dst,
	std::uint32_t a_dstLen,
	std::uint32_t a_terminator) -> std::uint32_t
{
	wchar_t* iter = a_dst;

	if (a_dstLen == 0)
		return 0;

	for (std::uint32_t i = 0; i < a_dstLen - 1; i++) {
		wchar_t data;

		if (Read(a_this, &data, sizeof(data)) != sizeof(data))
			break;

		if (data == a_terminator)
			break;

		*iter++ = data;
	}

	// null terminate
	*iter = 0;

	return static_cast<std::uint32_t>(iter - a_dst);
}

void Translation::ParseTranslation(const std::string& a_name)
{
	const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	const auto loader = scaleformManager ? scaleformManager->loader : nullptr;
	const auto translator = loader
		? loader->GetStateAddRef<RE::GFxTranslator>(RE::GFxState::StateType::kTranslator)
		: nullptr;

	const auto scaleformTranslator = skyrim_cast<RE::BSScaleformTranslator*>(translator);

	if (!scaleformTranslator) {
		logger::warn("Failed to import translation for {}"sv, a_name);
		return;
	}

	const auto iniSettingCollection = RE::INISettingCollection::GetSingleton();
	auto setting =
		iniSettingCollection
		? iniSettingCollection->GetSetting("sLanguage:General")
		: nullptr;

	// Construct translation filename
	std::string language = (setting && setting->GetType() == RE::Setting::Type::kString)
		? setting->data.s
		: "ENGLISH"s;

	std::string path = fmt::format("Interface\\Translations\\{}_{}.txt"sv, a_name, language);

	RE::BSResourceNiBinaryStream fileStream{ path };
	if (!fileStream.good()) {
		return;
	}
	else {
		logger::info("Reading translations from {}..."sv, path);
	}

	// Check if file is empty, if not check if the BOM is UTF-16
	std::uint16_t bom = 0;
	std::uint32_t ret = Read(&fileStream, &bom, sizeof(std::uint16_t));
	if (ret == 0) {
		logger::warn("Empty translation file."sv);
		return;
	}
	if (bom != 0xFEFF) {
		logger::error("BOM Error, file must be encoded in UCS-2 LE."sv);
		return;
	}

	while (true) {
		wchar_t buf[512];

		std::uint32_t len = ReadLine_w(&fileStream, buf, sizeof(buf) / sizeof(buf[0]), '\n');
		if (len == 0)  // End of file
			return;

		// at least $ + wchar_t + \t + wchar_t
		if (len < 4 || buf[0] != '$')
			continue;

		wchar_t last = buf[len - 1];
		if (last == '\r')
			len--;

		// null terminate
		buf[len] = 0;

		std::uint32_t delimIdx = 0;
		for (std::uint32_t i = 0; i < len; i++)
			if (buf[i] == '\t')
				delimIdx = i;

		// at least $ + wchar_t
		if (delimIdx < 2)
			continue;

		// replace \t by \0
		buf[delimIdx] = 0;

		wchar_t* key = nullptr;
		wchar_t* translation = nullptr;
		GetCachedString(&key, buf, 0);
		GetCachedString(&translation, &buf[delimIdx + 1], 0);
		scaleformTranslator->translator.translationMap.emplace(key, translation);
	}
}

auto Translation::Translate(const std::string& a_key) -> std::string
{
	if (!a_key.starts_with('$')) {
		return a_key;
	}

	const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	const auto loader = scaleformManager ? scaleformManager->loader : nullptr;
	const auto translator = loader
		? loader->GetStateAddRef<RE::BSScaleformTranslator>(RE::GFxState::StateType::kTranslator)
		: nullptr;

	if (!translator) {
		logger::warn("Failed to get Scaleform translator"sv);
		return a_key;
	}

	// Count braces to find what to replace
	std::string key = a_key;
	std::vector<std::string> nested;

	auto nestOpen = a_key.find_first_of('{');
	if (nestOpen != std::string::npos) {
		auto nestLevel = 1;
		auto tokenStart = nestOpen + 1;
		for (auto i = tokenStart; i < key.length(); i++) {
			if (key[i] == '{') {
				nestLevel++;
				if (nestLevel == 1) {
					tokenStart = i + 1;
				}
			}
			else if (key[i] == '}') {
				nestLevel--;
				if (nestLevel == 0) {
					// Recursion
					auto nestedKey = key.substr(tokenStart, i - tokenStart);
					nested.push_back(Translate(nestedKey));
					key = key.substr(0, tokenStart) + key.substr(i, key.length() - i);
					i = tokenStart + 1;
				}
			}
		}
	}

	// Lookup translation
	auto key_utf16 = Encoding::Utf8ToUtf16(key);
	RE::GFxWStringBuffer result;

	RE::GFxTranslator::TranslateInfo translateInfo;
	translateInfo.key = key_utf16.c_str();
	translateInfo.result = std::addressof(result);

	translator->Translate(std::addressof(translateInfo));

	auto result_utf8 = Encoding::Utf16ToUtf8(result.c_str());

	// Replace tokens with nested translations from right to left
	auto pos = result_utf8.rfind("{}"s);
	while (pos != std::string::npos && !nested.empty()) {
		result_utf8 = result_utf8.replace(pos, 2, nested.back());
		nested.pop_back();
		pos = result_utf8.rfind("{}"s, pos);
	}

	return result_utf8;
}
