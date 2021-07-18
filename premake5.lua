workspacedir = "premake-build"
bindir = "%{workspacedir}/build/%{cfg.buildcfg}-%{cfg.system}"

workspace "potato"
    configurations { "Debug", "Release" }
    architecture "x64"
    targetdir "build"
    cppdialect "C++20"
    location (workspacedir)

    filter { "files:**.ixx" }
        buildaction "ClCompile"
        compileas "Module"

    filter { "files:**.ifc" }
        buildaction "ClCompile"
        compileas "HeaderUnit"

    -- filter { "files:**.cpp or **.hpp" }
    --     compileas "C++"

    -- filter { "files:**.glsl" }
    --     flags "ExcludeFromBuild"
    -- buildcommands '"../libs/vulkan/glslangValidator.exe" -V -o "%{file.directory}/bin/%{file.name}.spv" "%{file.relpath}"'
    -- buildoutputs "%{file.directory}/bin/%{file.name}.spv"


project "potato"
    kind "ConsoleApp"
    language "C++"

    targetdir(bindir)
    objdir(bindir)

    files {
        "sources/potato/**.hpp",
        "sources/potato/**.cpp",
        "sources/potato/**.ixx",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "On"
        optimize "On"
        runtime "Release"
