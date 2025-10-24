include "Dependencies.lua"

workspace "Compile Driver"
	architecture "x86_64"
	--startproject "Application"

	toolset "clang"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	--/ Platform specific filters
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS"
		}

		links
		{
			
		}

	filter "system:linux"
		systemversion "latest"
		defines
		{
			"PLATFORM_LINUX"
		}

	filter "system:macos"
		systemversion "latest"
		defines
		{
			"PLATFORM_MAC"
		}

	filter "configurations:Debug"
		defines "ML_DEBUG"
		symbols "On"

		defines {
			"ML_ENABLE_LOGGING",
			"ML_ENABLE_ASSERTS",
			"ML_ENABLE_PROFILING",
			"ML_ENABLE_TRACING",
			"ML_DEFAULT_LOG_LEVEL = ML_LOG_LEVEL_DEBUG",
		}


	filter "configurations:Release"
		defines "ML_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "ML_DIST"
		optimize "On"

outputdir = "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
	include "Compiler"
group ""