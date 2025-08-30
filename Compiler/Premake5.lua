include "Dependencies.lua"

project "CompilerCore"
	kind "StaticLib"	

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{SourceDir.Core}" .. "/**.c",
		"%{SourceDir.Common}" .. "/**.c",
		"%{SourceDir.Utils}" .. "/**.c",

		"%{SourceDir.Core}" .. "/**.h",
		"%{SourceDir.Common}" .. "/**.h",
		"%{SourceDir.Utils}" .. "/**.h",

		"%{IncludeDir.Core}" .. "/**.h",
		"%{IncludeDir.Common}" .. "/**.h",
		"%{IncludeDir.Utils}" .. "/**.h"
	}

	includedirs {
		"%{IncludeDir.Core}",
		"%{IncludeDir.Common}",
		"%{IncludeDir.Utils}"
	}
