import os
import shutil
import re
import subprocess
from pathlib import Path
import sys

SDK_PATH = os.getenv("VULKAN_SDK")
VK_NAMESPACE = "vk"
NEWLINES = '\n\n'

DEFINES = [
    "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1" ,
    "VULKAN_HPP_NO_STRUCT_CONSTRUCTORS"    ,
    "VULKAN_HPP_NO_SPACESHIP_OPERATOR"     ,
    f"VULKAN_HPP_NAMESPACE={VK_NAMESPACE}" ,
]


def all_defines_windows():
    return DEFINES + [
        "NOMINMAX"                  ,
        "WIN32_LEAN_AND_MEAN"       ,
        "VK_USE_PLATFORM_WIN32_KHR" ,
    ]


def get_command_windows(in_file, out_file):

    defines = [ f"/D{x}" for x in all_defines_windows() ]
    cmd = "C:/Program Files (x86)/Microsoft/Visual Studio/2019/Preview/VC/Tools/MSVC/14.29.30132/bin/HostX64/x64/cl.exe"
    cmd_line = [
        cmd                     ,
        "/nologo"               ,
        "/E"                    ,
        "/C"                    ,
        "/P"                    ,
        "/EP"                   ,
        "/TP"                   ,
        # "/d1PP"                 ,
        "/std:c++latest"        ,
        "DEFINES"               ,
        "/I"                    ,
        f"{SDK_PATH}/Include"   ,
        f"/Fi:{out_file}"       ,
        f"{in_file}"            ,
    ]

    loc = cmd_line.index("DEFINES")
    return cmd_line[:loc] + defines + cmd_line[loc+1:]


def check_sdk():
    if env_exists := os.getenv("VULKAN_SDK"):
        pass
    else:
        raise Exception("Vulkan SDK not found")


def copy_vulkan_hpp(source, dest):
    """
    Copies header from VULKAN_SDK to `dest`
    """
    return Path(shutil.copy(source, f"{dest}/vulkan.hpp"))


def create_header():
    sdk_path = os.getenv("VULKAN_SDK")
    return copy_vulkan_hpp(f"{sdk_path}/Include/vulkan/vulkan.hpp", os.getcwd())


def delete_files(files):
    for file in files:
        os.remove(file)


def replace_includes(includes):
    all_includes = ""

    for i in includes:
        if i[1][1] == "c" or "vulkan" in i[1]:
            # eh.. good enough metric to retain c-headers
            all_includes += f"{i[0]}{i[1]}\n"

    return all_includes


def add_conveniences():
    namespace_start = f"\n\nnamespace {VK_NAMESPACE} {{ \n"

    make_version = \
    "    constexpr uint32_t makeApiVersion(uint32_t major, uint32_t minor) { \n" \
    "        return VK_MAKE_API_VERSION(0, major, minor, 0);\n" + \
    "    }\n\n"

    dispatcher = "VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE"

    default_dispatcher = \
        "    auto& Dispatcher { VULKAN_HPP_DEFAULT_DISPATCHER };\n"

    # With this exported, should be able to init dynamic loader properly
    procaddr = "    PFN_vkGetInstanceProcAddr;\n"

    with open(SDK_PATH + "/Include/vulkan/vulkan_core.h", "r") as f:
        htext = f.read()
        # grab all extension names
        extensions = re.findall(r"(VK.*EXTENSION_NAME)", htext, re.M)

        ext_text = ""
        for i in extensions:
            if i != "VK_MAX_EXTENSION_NAME":
                ext_text += f"        static constexpr auto {i[3:-15]} {{ {i} }};\n"

        ext_text = f"    struct extname {{ \n{ext_text}\n    }};\n"

    return namespace_start + \
        make_version + \
        NEWLINES + \
        procaddr + \
        NEWLINES + \
        ext_text + \
        NEWLINES + \
        default_dispatcher + \
        "}"+ \
        NEWLINES + \
        dispatcher


def remove_dispatch_static(module_body):
    ds = re.search("class DispatchLoaderStatic", module_body, re.M)

    # loop over the string and find the closing brace of the class
    inx = ds.end()
    while ( module_body[inx] != "{" ):
        inx += 1

    i = 1
    inx += 1

    while (i != 0):
        if module_body[inx] == "{":
            i += 1
        if module_body[inx] == "}":
            i -= 1
        inx += 1

    return module_body[:ds.start()-1] + module_body[inx:]


def get_module_content(path_to_hpp):
    namespace_text_replaced = f"namespace {VK_NAMESPACE}"

    # Add some convenience stuff
    with open(path_to_hpp, "a") as hpp:
        hpp.write(add_conveniences())

    # Run the preprocessor on the file, with the defines we need
    cmd = get_command_windows(path_to_hpp, "vkm.pp")
    print(f"Running cmd:")
    print(" ".join(cmd))
    print(subprocess.check_output(cmd).decode("utf8"))

    with open("vkm.pp", "r") as hpp:
        hpp_text = hpp.read()
        match = re.search(namespace_text_replaced, hpp_text)
        module_body = hpp_text[match.start():]
        # replace namespace with export namespace
        module_body = re.sub("namespace", "export namespace", module_body)
        # static functions can't be exported
        module_body = re.sub("static void throwResultException", "void throwResultException", module_body)

    delete_files(["vkm.pp"])
    return remove_dispatch_static(module_body)


def get_module_global_fragment(path_to_hpp, get_defines):
    """
    Comment out includes and then run preprocessor. From the output, extract all the
    includes and shove them back in. Replace some includes with imports
    """
    with open(path_to_hpp, "r") as hpp:
        hpp_text = hpp.read()
        sub_text = re.sub(r"^(#\s*include\s*)(<.*>)", r"//-\g<1>\g<2>", hpp_text, flags=re.M)
        with open("sub_text", "w") as f:
            f.write(sub_text)

    cmd = get_command_windows("sub_text", "vk_gf.pp")
    print(subprocess.check_output(cmd).decode("utf8"))

    defines_text = "\n".join([ f"#define {' '.join(d.split('='))}" for d in get_defines() ])

    all_includes = defines_text + "\n\n"

    all_non_pp = ""
    with open("vk_gf.pp", "r") as ppout:

        # Generate all the includes
        text = ppout.read()
        includes = re.findall(r"^//-(#\s*include\s*)(<.*>)", text, re.M)

        all_includes += replace_includes(includes)

        # Get all the text before 'namespace vk'
        before_part = text[:re.search("namespace vk", text, re.M).start()]
        # In this, get all that is not a preprocessor stuff
        non_pp = re.findall(r"^\S.*;$", before_part, re.M)
        for i in non_pp:
            all_non_pp += f"{i}\n"

    delete_files(["vk_gf.pp", "sub_text"])

    return all_includes, all_non_pp


def generate_header(path_to_hpp):
    """
    Has 2 steps.
    1. Grab all the text before the first occurance of
       `namespace VULKAN_HPP_NAMESPACE`
    2. Run the preprocessor on the file and save it to an intermediate file
    3. Get the first occurance of the `namespace VULKAN_HPP_NAMESPACE` in the
       intermediate file and replace all the contents above it with text
       saved from step 1
    4. Write out the hpp
    """

    # We create the mpp file. Add the defines just before the top part
    # to ensure proper stuff happens
    includes, stmts = get_module_global_fragment(path_to_hpp, all_defines_windows)
    module_body = get_module_content(path_to_hpp)

    imports = "import std.core;\n" \
        "import std.memory;\n"

    with open("vulkan.mpp", "w") as mpp:
        content = \
            "module;" + \
            NEWLINES + \
            includes + \
            NEWLINES + \
            "export module vulkan;" + \
            NEWLINES + \
            imports + \
            NEWLINES + \
            stmts + \
            NEWLINES + \
            module_body
        # replace redundant newlines
        content = re.sub(r"^\n{3,}", "\n", content, flags=re.M)
        mpp.write(content)

    delete_files(["./vulkan.hpp"])

check_sdk()
path = create_header()
generate_header(path)
