install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/SKSE/Plugins/${PROJECT_NAME}.ini"
	DESTINATION
		"SKSE/Plugins"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/${PROJECT_NAME}_ENGLISH.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/${PROJECT_NAME}_GERMAN.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/${PROJECT_NAME}_ITALIAN.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/${PROJECT_NAME}_JAPANESE.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/${PROJECT_NAME}_RUSSIAN.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/${PROJECT_NAME}_SPANISH.txt"
	DESTINATION
		"Interface/Translations"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/skyrimui/Interface/magicmenu.swf"
	DESTINATION "Interface"
	COMPONENT SkyrimUI
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/skyui/Interface/magicmenu.swf"
	DESTINATION "Interface"
	COMPONENT SkyUI
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/skyui-vr/Interface/magicmenu.swf"
	DESTINATION "Interface"
	COMPONENT SkyUI-VR
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Fomod/info.xml"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/Fomod/ModuleConfig.xml"
	DESTINATION "fomod"
	COMPONENT Fomod
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/fomod/images/skyrim_se.jpg"
		"${PROJECT_SOURCE_DIR}/data/fomod/images/skyrim_vr.jpg"
	DESTINATION "fomod/images"
	COMPONENT Fomod
	EXCLUDE_FROM_ALL
)
