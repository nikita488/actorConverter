IG_ROOT = os.getenv("IG_ROOT")
FBX_SDK = "D:/Dev/FBX SDK/2020.2.1"

workspace "actorConverter"
	kind "ConsoleApp"
	language "C++"
	system "Windows"
	systemversion "latest"
	characterset "MBCS"
	location "build"
	configurations { "Debug", "Release" }
	targetdir "bin/%{cfg.buildcfg}"
	staticruntime "on"
	
	filter "configurations:Debug"
		libdirs { path.join(IG_ROOT, "DirectX9/libdbg"), path.join(FBX_SDK, "lib/vs2019/x86/debug") }
		defines { "WIN32", "_DEBUG", "_CONSOLE", "IG_ALCHEMY_DLL", "IG_TARGET_WIN32", "IG_GFX_DX9", "IG_COMPILER_MSVC", "IG_DEBUG", "IG_MEMORY_TRACKING" }
		symbols "On"
	filter {}
	
	filter "configurations:Release"
		libdirs { path.join(IG_ROOT, "DirectX9/lib"), path.join(FBX_SDK, "lib/vs2019/x86/release") }
		defines { "WIN32", "NDEBUG", "_CONSOLE", "IG_ALCHEMY_DLL", "IG_TARGET_WIN32", "IG_GFX_DX9", "IG_COMPILER_MSVC" }
		optimize "On"
	filter {}
	
	includedirs { path.join(IG_ROOT, "include"), path.join(FBX_SDK, "include") }
	
	links { "libIGCore", "libIGMath", "libIGGfx", "libIGSg", "libIGAttrs", "libIGOpt" }
	links { "libfbxsdk-mt", "libxml2-mt", "zlib-mt" }
   
	files { "src/*.*" }
   
project "actorConverter"