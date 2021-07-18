workspacedir = "premake-build"
bindir = "%{workspacedir}/build/%{cfg.buildcfg}-%{cfg.system}"

workspace "potato"
    configurations { "Debug", "Release" }
    architecture "x64"
    targetdir "build"
    cppdialect "C++20"
    location (workspacedir)

    startupproject "potato"

    filter { "files:**.mpp" }
        buildaction "ClCompile"
        compileas "Module"

    filter { "files:**.ifc" }
        buildaction "ClCompile"
        compileas "HeaderUnit"

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

    -- filter { "files:**.cpp or **.hpp" }
    --     compileas "C++"

    -- filter { "files:**.glsl" }
    --     flags "ExcludeFromBuild"
    -- buildcommands '"../libs/vulkan/glslangValidator.exe" -V -o "%{file.directory}/bin/%{file.name}.spv" "%{file.relpath}"'
    -- buildoutputs "%{file.directory}/bin/%{file.name}.spv"


project "potato"
    kind "ConsoleApp"
    language "C++"
    links { "glfwcpp" }

    targetdir(bindir)
    objdir(bindir)

    files {
        "sources/potato/**.cpp",
        "sources/potato/**.mpp",
    }

project "glfwcpp"
    kind "StaticLib"
    language "C++"

    targetdir(bindir)
    objdir(bindir)

    files {
        "sources/glfwcpp/**.cpp",
        "sources/glfwcpp/**.mpp",
    }
