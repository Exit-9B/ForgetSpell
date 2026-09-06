@echo off

cmake --preset vs2022-windows --check-stamp-file "build/CMakeFiles/generate.stamp" || goto :error
cmake --build build --config Release --clean-first || goto :error
cmake --install build --component "Fomod" --prefix "package" || goto :error
cmake --install build --component "Data" --prefix "package" || goto :error
cmake --install build --component "SKSEPlugin" --prefix "package/SkyrimSE" || goto :error

cmake --preset vs2022-windows-vr --check-stamp-file "build-vr/CMakeFiles/generate.stamp" || goto :error
cmake --build build-vr --target "ForgetSpell" --config Release --clean-first || goto :error
cmake --install build-vr --component "SKSEPlugin" --prefix "package/SkyrimVR" || goto :error

pushd package || goto :error
7z a -r -t7Z "../ForgetSpell.7z" *
popd

goto :EOF

:error
exit /b %errorlevel%
