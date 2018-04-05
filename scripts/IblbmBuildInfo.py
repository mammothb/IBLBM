#!/usr/bin/env python

from __future__ import print_function
import os
from string import Template

def main():
    """Simple script to update the src/io/IblbmBuildInfo.info file with the
    correct path information to Iblbm_SOURCE_ROOT and Iblbm_BUILD_ROOT. This
    file assumes the following project directory layout:
    <Iblbm_SOURCE_ROOT>
    |
    +-<Iblbm_BUILD_ROOT>
    |
    +-scripts
    | |
    | +-IblbmBuildInfo.py (__file__)
    |
    +...
    """
    src_path = os.path.realpath(
        os.path.join(__file__, *(os.path.pardir,) * 2))
    build_path = os.path.join(src_path, "build")
    build_info_input = os.path.join(src_path, "src", "io",
                                    "IblbmBuildInfo.input")
    build_info_output = os.path.join(src_path, "src", "io",
                                     "IblbmBuildRoot.cpp")

    template_dict = {
        "Iblbm_BUILD_DIR": build_path,
        "Iblbm_SOURCE_DIR": src_path
    }

    with open(build_info_input) as template_file:
        template_src = Template(template_file.read())
        with open(build_info_output, "w") as output_file:
            output_file.write(template_src.substitute(template_dict))

    print("========== Updated IBLBM Build Root Info ==========")

if __name__ == "__main__":
    main()
