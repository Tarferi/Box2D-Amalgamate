@echo off

@rem === Box2D Amalgamate script ===

@rem -- Config --
set output_file=box2d.h
set fixed_point_support=1
set fixed_point_support_external=1
set fp_file=fp.h


if "%~1" == "echo_headers" goto echo_headers
if "%~1" == "echo_sources" goto echo_sources
echo //=== Box2D Amalagamation === >%output_file%
echo //>>%output_file%
echo //>>%output_file%
echo // Usage: Include this header file and in one source file define BOX2D_IMPL for implementation>>%output_file%
echo // To adjust included libraries and headers, define BOX2D_NO_INCLUDES>>%output_file%
echo // To exclude b2_Timer class (and define your own), define BOX2D_NO_TIMER>>%output_file%
echo.>>%output_file%
echo.>>%output_file%
echo #ifndef BOX2D_NO_INCLUDES>>%output_file%
for /f %%a in ('call "%~0" echo_headers') do call :process_file 0 "%%a"
echo #ifdef BOX2D_IMPL>>%output_file%
for /f %%a in ('call "%~0" echo_sources') do call :process_file 0 "%%a"
echo #endif>>%output_file%
echo #endif>>%output_file%

if "%fixed_point_support%"=="1" (
	echo.>>%output_file%
	echo.>>%output_file%
	for /f "delims=:" %%a in ('findstr /n "INCLUDE_POST_HEADERS_BEGIN" "%fp_file%"') do set head_begin=%%a
	for /f "delims=:" %%a in ('findstr /n "INCLUDE_POST_HEADERS_END" "%fp_file%"') do set head_end=%%a

	echo #ifndef BOX2D_USE_FIXED_POINT>>%output_file%
	echo #define b2Dump_unused b2Dump>>%output_file%
	echo #endif>>%output_file%
)
if "%fixed_point_support%"=="1" echo #ifndef BOX2D_FP_SUPPORT_H>>%output_file%
if "%fixed_point_support%"=="1" echo #define BOX2D_FP_SUPPORT_H>>%output_file%
if "%fixed_point_support%"=="1" set /a head_begin=%head_begin%-1
if "%fixed_point_support%"=="1" set /a head_len=%head_end%-%head_begin%
if "%fixed_point_support%"=="1" if "%fixed_point_support_external%"=="0" powershell -Command "& { Get-Content "%fp_file%" | Select-Object -Skip %head_begin% -First %head_len% }" >> %output_file%
if "%fixed_point_support%"=="1" echo #endif >>%output_file%

echo.>>%output_file%
echo.>>%output_file%
echo #ifndef BOX2D_INCLUDES_ONLY>>%output_file%
for /f %%a in ('call "%~0" echo_headers') do call :process_file 1 "%%a"
call :process_file "test.txt"
echo #ifdef BOX2D_IMPL>>%output_file%
for /f %%a in ('call "%~0" echo_sources') do call :process_file 1 "%%a"


if "%fixed_point_support%"=="1" (
	echo.>>%output_file%
	echo.>>%output_file%
	for /f "delims=:" %%a in ('findstr /n "INCLUDE_POST_IMPL_BEGIN" "%fp_file%"') do set impl_begin=%%a
	for /f "delims=:" %%a in ('findstr /n "INCLUDE_POST_IMPL_END" "%fp_file%"') do set impl_end=%%a
)
if "%fixed_point_support%"=="1" set /a impl_begin=%impl_begin%-1
if "%fixed_point_support%"=="1" set /a impl_len=%impl_end%-%impl_begin%
if "%fixed_point_support%"=="1" if "%fixed_point_support_external%"=="0" powershell -Command "& { Get-Content "%fp_file%" | Select-Object -Skip %impl_begin% -First %impl_len% }" >> %output_file%

echo #endif>>%output_file%
echo #endif>>%output_file%

if "%fixed_point_support%"=="1" (
@rem replace floating point constants
@rem fixes
	powershell -Command "& { (Get-Content .\%output_file%) | Foreach-Object {$_ -replace '1e-3f','0.001f'} | Set-Content .\%output_file% }"
	powershell -Command "& { (Get-Content .\%output_file%) | Foreach-Object {$_ -replace 'dt == 0.0','dt == 0.0f'} | Set-Content .\%output_file% }"
	powershell -Command "& { (Get-Content .\%output_file%) | Foreach-Object {$_ -replace 'float iB = 0.;','float iB = 0.0f;'} | Set-Content .\%output_file% }"

@rem main replacement
	powershell -Command "& { (Get-Content .\%output_file%) | Foreach-Object {$_ -replace '(\d+)(\.)(0*)(\d+)(f)','F_CONST($1, $4, \"$3$4\", $1.$3$4f)'} | Set-Content .\%output_file% }"

@rem replace dump function
	powershell -Command "& { (Get-Content .\box2d.h) | Foreach-Object {$_ -replace 'void b2Dump\(const char\* string','void b2Dump_unused(const char* string'} | Set-Content .\box2d.h }"

)

goto :EOF


:process_file
set action=%1
set f=%~2
if "%f%" == "" goto :EOF
echo %f%
if "%f%" == "include\box2d\b2_timer.h" echo #ifndef BOX2D_NO_TIMER >>%output_file%
if "%f%" == "src\common\b2_timer.cpp" echo #ifndef BOX2D_NO_TIMER >>%output_file%

if %action%==0 (
	for /f "tokens=*" %%a in ('findstr /C:"#include " /B "%CD%\%f%"') do call :process_file_line "%%a" 2>NUL
) else if %action%==1 (
	echo // File: %f%>>%output_file%
	echo.>>%output_file%
	findstr /C:"#include " /B /V "%CD%\%f%">>%output_file% 2>NUL
	echo.>>%output_file%
)
if %action%==1 (
	echo.>>%output_file%
	echo.>>%output_file%
)
if "%f%" == "include\box2d\b2_timer.h" echo #endif >>%output_file%
if "%f%" == "src\common\b2_timer.cpp" echo #endif >>%output_file%

goto :EOF


:process_file_line
set line="%~1"
setlocal EnableDelayedExpansion

if "!line:~1,8!" == "#include" (
	set target=!line:~10,-1!
	set target2=!target!
	set tmp1=!target:~0,1!
	set tmp2=!target:~-1,1!
	set tmp1=!tmp1:"=<!
	set tmp2=!tmp2:"=>!
	set target=!tmp1!!target:~1,-1!!tmp2!
	if "!target:~0,1!" == "<" set target=!target:~1!
	if "!target:~-1,1!" == ">" set target=!target:~0,-1!
	if not "!target:~-0,3!" == "b2_" (
		if not "!target:~-0,9!" == "box2d/b2_" (
			if not "!target:~-0,12!" == "dynamics/b2_" (
				findstr /C:"!line:~1,-1!" /M %output_file% >NUL 2>NUL
				if !ERRORLEVEL! EQU 1 (
					if "!target!" == "windows.h" (
						echo #ifdef BOX2D_IMPL>>%output_file%
						echo #if defined^(_WIN32^)>>%output_file%
						echo #ifndef WIN32_LEAN_AND_MEAN >>%output_file%
						echo #define WIN32_LEAN_AND_MEAN >>%output_file%
						echo #endif >>%output_file%
					) else if "!target!" == "sys/time.h" (
						echo #ifdef BOX2D_IMPL>>%output_file%
						echo #if defined^(__linux__^) ^|^| defined ^(__APPLE__^)>>%output_file%
					)
					( echo !line:~1,-1! ) >> %output_file%
					if "!target!" == "windows.h" (
						echo #endif >>%output_file%
						echo #endif >>%output_file%
					) else if "!target!" == "sys/time.h" (
						echo #endif >>%output_file%
						echo #endif >>%output_file%
					)
				)
			)
		)
	)
)
endlocal
goto :EOF

:echo_headers
echo include\box2d\b2_types.h
echo include\box2d\b2_api.h
echo include\box2d\b2_settings.h
echo include\box2d\b2_common.h
echo include\box2d\b2_stack_allocator.h
echo include\box2d\b2_block_allocator.h
echo include\box2d\b2_math.h
echo include\box2d\b2_collision.h
echo include\box2d\b2_shape.h
echo include\box2d\b2_body.h
echo include\box2d\b2_growable_stack.h
echo include\box2d\b2_dynamic_tree.h
echo include\box2d\b2_broad_phase.h
echo include\box2d\b2_chain_shape.h
echo include\box2d\b2_circle_shape.h
echo include\box2d\b2_fixture.h
echo include\box2d\b2_contact.h
echo include\box2d\b2_contact_manager.h
echo include\box2d\b2_distance.h
echo include\box2d\b2_joint.h
echo include\box2d\b2_distance_joint.h
echo include\box2d\b2_draw.h
echo include\box2d\b2_edge_shape.h
echo include\box2d\b2_friction_joint.h
echo include\box2d\b2_gear_joint.h
echo include\box2d\b2_motor_joint.h
echo include\box2d\b2_mouse_joint.h
echo include\box2d\b2_polygon_shape.h
echo include\box2d\b2_prismatic_joint.h
echo include\box2d\b2_pulley_joint.h
echo include\box2d\b2_revolute_joint.h
echo include\box2d\b2_rope.h
echo include\box2d\b2_timer.h
echo include\box2d\b2_time_of_impact.h
echo include\box2d\b2_time_step.h
echo include\box2d\b2_weld_joint.h
echo include\box2d\b2_wheel_joint.h
echo include\box2d\b2_world_callbacks.h
echo include\box2d\b2_world.h
echo src\dynamics\b2_chain_circle_contact.h
echo src\dynamics\b2_chain_polygon_contact.h
echo src\dynamics\b2_circle_contact.h
echo src\dynamics\b2_contact_solver.h
echo src\dynamics\b2_edge_circle_contact.h
echo src\dynamics\b2_edge_polygon_contact.h
echo src\dynamics\b2_island.h
echo src\dynamics\b2_polygon_circle_contact.h
echo src\dynamics\b2_polygon_contact.h
goto :EOF

:echo_sources
echo src\collision\b2_broad_phase.cpp
echo src\collision\b2_chain_shape.cpp
echo src\collision\b2_circle_shape.cpp
echo src\collision\b2_collide_circle.cpp
echo src\collision\b2_collide_edge.cpp
echo src\collision\b2_collide_polygon.cpp
echo src\collision\b2_collision.cpp
echo src\collision\b2_distance.cpp
echo src\collision\b2_dynamic_tree.cpp
echo src\collision\b2_edge_shape.cpp
echo src\collision\b2_polygon_shape.cpp
echo src\collision\b2_time_of_impact.cpp
echo src\common\b2_block_allocator.cpp
echo src\common\b2_draw.cpp
echo src\common\b2_math.cpp
echo src\common\b2_settings.cpp
echo src\common\b2_stack_allocator.cpp
echo src\common\b2_timer.cpp
echo src\dynamics\b2_body.cpp
echo src\dynamics\b2_chain_circle_contact.cpp
echo src\dynamics\b2_chain_polygon_contact.cpp
echo src\dynamics\b2_circle_contact.cpp
echo src\dynamics\b2_contact.cpp
echo src\dynamics\b2_contact_manager.cpp
echo src\dynamics\b2_contact_solver.cpp
echo src\dynamics\b2_distance_joint.cpp
echo src\dynamics\b2_edge_circle_contact.cpp
echo src\dynamics\b2_edge_polygon_contact.cpp
echo src\dynamics\b2_fixture.cpp
echo src\dynamics\b2_friction_joint.cpp
echo src\dynamics\b2_gear_joint.cpp
echo src\dynamics\b2_island.cpp
echo src\dynamics\b2_joint.cpp
echo src\dynamics\b2_motor_joint.cpp
echo src\dynamics\b2_mouse_joint.cpp
echo src\dynamics\b2_polygon_circle_contact.cpp
echo src\dynamics\b2_polygon_contact.cpp
echo src\dynamics\b2_prismatic_joint.cpp
echo src\dynamics\b2_pulley_joint.cpp
echo src\dynamics\b2_revolute_joint.cpp
echo src\dynamics\b2_weld_joint.cpp
echo src\dynamics\b2_wheel_joint.cpp
echo src\dynamics\b2_world.cpp
echo src\dynamics\b2_world_callbacks.cpp
echo src\rope\b2_rope.cpp
goto :EOF