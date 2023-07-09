#!/bin/bash

scriptName="$(basename ${BASH_SOURCE[0]})"
scriptDir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
projectName=$(basename $scriptDir)
projectNameUpper=$(printf '%s\n' "$projectName" | awk '{ print toupper($0) }')

print_help() {
    echo "$scriptName - Configure, build, and install $(basename $scriptDir)"
    echo "-h                : print this help and exit"
    echo "-p                : package after building"
    echo "-t build-type     : build type [Debug, Release, RelWithDebInfo] (default: Release)"
    echo "-b build-dir      : build directory"
    echo "-i install-dir    : install directory (python site package by default)"
    echo "-c compiler       : C++ compiler to use"
}

if ! command -v python3 &>/dev/null; then
    echo "Error: $scriptName requires python3, but could not find it on the system"
    exit 1
fi

# Function for getting the module paths associated with the current interpreter
get_site_packages_dir() {
    echo $(python3 -c 'import sysconfig; print(sysconfig.get_paths()["purelib"])')
}

# Default arguments
package=0
buildType="Release"
buildDir="$scriptDir/build"
installDir=$(get_site_packages_dir)
compiler=g++
generator="Unix Makefiles"
cCacheFlag=""

while getopts "hpt:b:i:c:" arg; do
    case "$arg" in
        h)  # Print help and exit without doing anything
            print_help
            exit 0
            ;;
        p)  # Package after building
            package=1
            ;;
        t)  # Set build type
            buildType="$OPTARG"
            (("${buildType}" == "Debug" || "${buildType}" == "RelWithDebInfo" || "${buildType}" == "Release")) || (print_help && echo "Invalid build type: $buildType" && exit 1)
            ;;
        b)  # Set build directory
            buildDir="$OPTARG"
            ;;
        i)  # Set install directory
            installDir="$OPTARG"
            ;;
        c)  # Set C++ compiler
            compiler="$OPTARG"
            ;;
        \?) # Unrecognized argument
            echo "Error: unrecognized argument $arg"
            exit 1;;
    esac
done

# Create or clear the build directory
if ! [ -d "$buildDir" ]; then
    mkdir -p "$buildDir"
else
    rm -rf "$buildDir/cmake_install.cmake"
    rm -rf "$buildDir/CMakeCache.txt"
    rm -rf "$buildDir/CMakeFiles"
fi

# Check optional dependency - ninja
if command -v ninja &>/dev/null; then
    generator="Ninja"
fi

# Check optional dependency - ccache
if command -v ccache &>/dev/null; then
    cCacheFlag="-DCMAKE_CXX_COMPILER_LAUNCHER:STRING=ccache"
fi

# Generate with CMake
if ! cmake                                      \
    "-H$scriptDir"                              \
    "-B$buildDir"                               \
    "-DCMAKE_INSTALL_PREFIX:STRING=$installDir" \
    "-G${generator}"                            \
    "-DCMAKE_BUILD_TYPE:STRING=$buildType"      \
    "-DCMAKE_COLOR_DIAGNOSTICS:BOOL=ON"         \
    "-D${projectNameUpper}_BUILD_SHARED_LIBRARY:BOOL=ON"        \
    "-D${projectNameUpper}_BUILD_PYTHON_MODULE:BOOL=ON"         \
    "-D${projectNameUpper}_BUILD_TESTS:BOOL=ON"                 \
    "$cCacheFlag"                               \
    ; then
    exit $?
fi

# Build and install
if ! cmake --build "$buildDir" --target install -j; then
    exit $?
fi

# Package
if [ $package -eq 1 ]; then
    cd "$buildDir"
    if [ "$generator" = "Ninja" ]; then
        echo ninja package
        ninja package
        ninja package_source
    else
        make package
        make package_source
    fi
fi

# Success
exit 0
