from __future__ import print_function
import os
import subprocess

def main():
    src_path = os.path.realpath(
        os.path.join(__file__, *(os.path.pardir,) * 2))
    rc_path = os.path.join(src_path, "scripts", "lcovrc")
    output_name = "coverage"
    output_file_path = os.path.join(src_path, output_name + ".info")
    clean_output_file_path = os.path.join(src_path,
                                          output_name + ".info.cleaned")
    test_bin = os.path.join(src_path, "build", "Coverage", "bin", "IBLBM")

    lcov = "/usr/bin/lcov"
    genhtml = "/usr/bin/genhtml"

    # Clean up lcov
    cleanup_cmd = [lcov, "--directory", ".", "--zerocounters"]
    subprocess.call(cleanup_cmd, cwd=src_path)

    # Run tests
    run_test_cmd = [test_bin]
    subprocess.call(run_test_cmd, cwd=src_path)

    # Capture lcov counters and generate report
    capture_cmd = [lcov, "--config-file", rc_path, "--directory", src_path,
                   "--capture", "--output-file", output_file_path]
    subprocess.call(capture_cmd, cwd=src_path)
    ignore_path = ["usr/*", "*/examples/*", "*/fortests/*", "*/test/*",
                   "*/thirdparty/*"]
    clean_capture_cmd = [lcov, "--config-file", rc_path, "--remove",
                         output_file_path]
    clean_capture_cmd += ignore_path
    clean_capture_cmd += ["--output-file", clean_output_file_path]
    subprocess.call(clean_capture_cmd, cwd=src_path)
    genhtml_cmd = [genhtml, "--title", "IBLBM Coverage Results",
                   "--config-file", rc_path, "--no-function-coverage", "-o",
                   output_name, clean_output_file_path]
    subprocess.call(genhtml_cmd, cwd=src_path)
    os.remove(output_file_path)
    os.remove(clean_output_file_path)

if __name__ == "__main__":
    main()
