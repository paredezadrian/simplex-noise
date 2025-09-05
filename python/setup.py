#!/usr/bin/env python3
"""
Setup script for Pure C Simplex Noise Python Wrapper

This script allows easy installation of the Python wrapper for the simplex noise library.
"""

from setuptools import setup, find_packages
import os
import subprocess


# Read the README file
def read_readme():
    readme_path = os.path.join(os.path.dirname(__file__), "..", "README.md")
    if os.path.exists(readme_path):
        with open(readme_path, "r", encoding="utf-8") as f:
            return f.read()
    return "Pure C Simplex Noise Library - Python Wrapper"


# Read requirements
def read_requirements():
    requirements_path = os.path.join(os.path.dirname(__file__), "requirements.txt")
    if os.path.exists(requirements_path):
        with open(requirements_path, "r", encoding="utf-8") as f:
            return [
                line.strip() for line in f if line.strip() and not line.startswith("#")
            ]
    return []


# Check if C library is built
def check_c_library():
    """Check if the C library is built and available."""
    # Look for the library in common locations
    possible_paths = [
        os.path.join(os.path.dirname(__file__), "..", "build", "libsimplex_noise.so"),
        os.path.join(
            os.path.dirname(__file__), "..", "build", "libsimplex_noise.dylib"
        ),
        os.path.join(os.path.dirname(__file__), "..", "build", "libsimplex_noise.dll"),
    ]

    for path in possible_paths:
        if os.path.exists(path):
            return True

    return False


# Build C library if needed
def build_c_library():
    """Build the C library if it's not already built."""
    if check_c_library():
        return True

    print("C library not found. Building...")

    # Change to the project root directory
    project_root = os.path.dirname(os.path.dirname(__file__))
    original_cwd = os.getcwd()

    try:
        os.chdir(project_root)

        # Try to build with CMake
        if os.path.exists("CMakeLists.txt"):
            print("Building with CMake...")
            try:
                # Create build directory
                os.makedirs("build", exist_ok=True)

                # Configure
                subprocess.run(["cmake", "-B", "build", "-S", "."], check=True)

                # Build
                subprocess.run(["cmake", "--build", "build"], check=True)

                print("C library built successfully!")
                return True

            except subprocess.CalledProcessError as e:
                print(f"CMake build failed: {e}")
                return False
        else:
            print("No CMakeLists.txt found. Please build the C library manually.")
            return False

    finally:
        os.chdir(original_cwd)


# Note: C library should be built separately before packaging


# Get version
def get_version():
    """Get version from the main module."""
    try:
        with open(
            os.path.join(os.path.dirname(__file__), "simplex_noise.py"), "r"
        ) as f:
            for line in f:
                if line.startswith("__version__"):
                    return line.split("=")[1].strip().strip("\"'")
    except (FileNotFoundError, ValueError, IndexError):
        pass
    return "1.0.0"


# Setup configuration
setup(
    name="simplex-noise",
    version=get_version(),
    author="Adrian Paredez",
    author_email="adrian@example.com",  # Replace with actual email
    description="Python wrapper for Pure C Simplex Noise library",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    url="https://github.com/paredezadrian/simplex-noise",
    project_urls={
        "Bug Tracker": "https://github.com/paredezadrian/simplex-noise/issues",
        "Documentation": "https://paredezadrian.github.io/simplex-noise/",
        "Source Code": "https://github.com/paredezadrian/simplex-noise",
    },
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: Multimedia :: Graphics",
        "Topic :: Scientific/Engineering :: Mathematics",
        "Topic :: Games/Entertainment",
    ],
    keywords="noise, simplex, procedural, generation, graphics, terrain, texture",
    packages=find_packages(),
    py_modules=["simplex_noise"],
    python_requires=">=3.7",
    install_requires=[
        "numpy",
        "Pillow",
    ],
    extras_require={
        "dev": [
            "pytest",
            "pytest-cov",
            "matplotlib",
            "black",
            "flake8",
        ],
        "docs": [
            "sphinx",
            "sphinx-rtd-theme",
        ],
    },
    entry_points={
        "console_scripts": [
            "simplex-noise=simplex_noise:main",
        ],
    },
    include_package_data=True,
    package_data={
        "": ["*.so", "*.dylib", "*.dll"],  # Include compiled libraries
    },
    zip_safe=False,
    platforms=["any"],
    license="MIT",
    download_url="https://github.com/paredezadrian/simplex-noise/archive/v{}.tar.gz".format(
        get_version()
    ),
)

if __name__ == "__main__":
    setup()
