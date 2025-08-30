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

outputdir = "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
	include "Compiler"
group ""