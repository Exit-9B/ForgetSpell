install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION
		"SKSE/Plugins"
	COMPONENT SKSEPlugin
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/SKSE/Plugins/${PROJECT_NAME}.ini"
	DESTINATION
		"SKSE/Plugins"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/${PROJECT_NAME}_ENGLISH.txt"
	DESTINATION
		"Interface/Translations"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/skyrimui/Interface/magicmenu.swf"
	DESTINATION "Interface"
	COMPONENT SkyrimUI
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/skyui/Interface/magicmenu.swf"
	DESTINATION "Interface"
	COMPONENT SkyUI
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/skyui-vr/Interface/magicmenu.swf"
	DESTINATION "Interface"
	COMPONENT SkyUI-VR
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Fomod/info.xml"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Fomod/ModuleConfig.xml"
	DESTINATION "fomod"
	COMPONENT Fomod
	EXCLUDE_FROM_ALL
)
