include "Dependencies.lua"

project "CompilerCore"
	kind "StaticLib"	

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{SourceDir.Compiler}" .. "/**.c",

		"%{SourceDir.Compiler}" .. "/**.h",

		"%{IncludeDir.Compiler}" .. "/**.h",
	}

	includedirs {
		"%{IncludeDir.Compiler}",
	}
