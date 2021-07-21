import os
import shutil
import re
import subprocess
from pathlib import Path

SDK_PATH = os.getenv("VULKAN_SDK")
VK_NAMESPACE = "vk"

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

def get_module_content(path_to_hpp):
    namespace_text_replaced = f"namespace {VK_NAMESPACE}"

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

    delete_files(["vkm.pp"])

    return module_body

def get_module_global_fragment(path_to_hpp, get_defines):
    """
    Comment out includes and then run preprocessor. From the output, extract all the
    includes and shove them back in
    """
    with open(path_to_hpp, "r") as hpp:
        hpp_text = hpp.read()
        sub_text = re.sub(r"^(#\s*include\s*)(<.*>)", r"//-\g<1>\g<2>", hpp_text, flags=re.M)
        with open("sub_text", "w") as f:
            f.write(sub_text)

    cmd = get_command_windows("sub_text", "vk_gf.pp")
    print(subprocess.check_output(cmd).decode("utf8"))

    defines_text = "\n".join([ f"#define {d}" for d in get_defines() ])

    all_includes = defines_text + "\n\n"

    all_non_pp = ""
    with open("vk_gf.pp", "r") as ppout:

        # Generate all the includes
        text = ppout.read()
        includes = re.findall(r"^//-(#\s*include\s*)(<.*>)", text, re.M)

        for i in includes:
            all_includes += f"{i[0]}{i[1]}\n"

        # Get all the text before 'namespace vk'
        before_part = text[:re.search("namespace vk", text, re.M).start()]
        # In this, get all that is not a preprocessor stuff
        non_pp = re.findall(r"^\S.*;$", before_part, re.M)
        for i in non_pp:
            all_non_pp += f"{i}\n"

    delete_files(["vk_gf.pp", "sub_text"])

    return all_includes, all_non_pp

    # module_body = re.sub("namespace", "export namespace", module_body)
    # module_body = hpp_text[match.start():]

    # # replace namespace with export namespace
    # text = re.sub("namespace", "export namespace", text, re.M)

    # return before + defines_text + after


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

    with open("vulkan.mpp", "w") as mpp:
        content = \
            "module;\n" + \
            includes + \
            "\n\n" + \
            "export module vulkan;" + \
            "\n\n" + \
            stmts + \
            "\n\n" + \
            module_body
        mpp.write(content)

    delete_files(["./vulkan.hpp"])

check_sdk()
path = create_header()
generate_header(path)
