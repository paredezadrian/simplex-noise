#!/usr/bin/env python3
"""
Pure C Simplex Noise Library - Python Wrapper

A high-performance Python wrapper for the Pure C Simplex Noise library.
Provides easy access to simplex noise generation with NumPy integration.

Author: Adrian Paredez
Date: 9/5/2025
License: MIT
"""

import ctypes
import os
import numpy as np
from typing import Union, Optional

# Try to import PIL for image generation
try:
    from PIL import Image

    PIL_AVAILABLE = True
except ImportError:
    PIL_AVAILABLE = False


class SimplexNoise:
    """
    Python wrapper for the Pure C Simplex Noise library.

    Provides high-performance noise generation with NumPy integration
    and easy-to-use Python API.
    """

    def __init__(self, seed: int = 0, library_path: Optional[str] = None):
        """
        Initialize the Simplex Noise generator.

        Args:
            seed: Random seed for noise generation
            library_path: Path to the compiled C library (auto-detected if None)
        """
        self._lib = self._load_library(library_path)
        self._setup_function_signatures()
        self._seed = seed
        self._initialized = False

        # Initialize with seed
        self.init(seed)

    def _load_library(self, library_path: Optional[str]) -> ctypes.CDLL:
        """Load the compiled C library."""
        if library_path is None:
            # Try to find the library in common locations
            possible_paths = [
                # Relative to this file (development)
                os.path.join(
                    os.path.dirname(__file__), "..", "build", "libsimplex_noise.so"
                ),
                os.path.join(
                    os.path.dirname(__file__), "..", "build", "libsimplex_noise.dylib"
                ),
                os.path.join(
                    os.path.dirname(__file__), "..", "build", "libsimplex_noise.dll"
                ),
                # Package data (PyPI installation)
                os.path.join(os.path.dirname(__file__), "libsimplex_noise.so"),
                os.path.join(os.path.dirname(__file__), "libsimplex_noise.dylib"),
                os.path.join(os.path.dirname(__file__), "libsimplex_noise.dll"),
                # System paths
                "libsimplex_noise.so",
                "libsimplex_noise.dylib",
                "libsimplex_noise.dll",
            ]

            for path in possible_paths:
                if os.path.exists(path):
                    library_path = path
                    break

            if library_path is None:
                raise FileNotFoundError(
                    "Could not find compiled simplex noise library. "
                    "Please build the C library first or specify library_path."
                )

        try:
            return ctypes.CDLL(library_path)
        except OSError as e:
            raise RuntimeError(f"Failed to load library {library_path}: {e}")

    def _setup_function_signatures(self):
        """Setup function signatures for the C library."""
        # Core functions
        self._lib.simplex_noise_init.argtypes = [ctypes.c_uint]
        self._lib.simplex_noise_init.restype = None

        self._lib.simplex_noise_2d.argtypes = [ctypes.c_double, ctypes.c_double]
        self._lib.simplex_noise_2d.restype = ctypes.c_double

        self._lib.simplex_noise_3d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_noise_3d.restype = ctypes.c_double

        self._lib.simplex_noise_4d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_noise_4d.restype = ctypes.c_double

        # Fractal functions
        self._lib.simplex_fractal_2d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_int,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_fractal_2d.restype = ctypes.c_double

        self._lib.simplex_fractal_3d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_int,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_fractal_3d.restype = ctypes.c_double

        # Advanced functions
        self._lib.simplex_ridged_2d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_ridged_2d.restype = ctypes.c_double

        self._lib.simplex_billowy_2d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_billowy_2d.restype = ctypes.c_double

        self._lib.simplex_fbm_2d.argtypes = [
            ctypes.c_double,
            ctypes.c_double,
            ctypes.c_int,
            ctypes.c_double,
            ctypes.c_double,
        ]
        self._lib.simplex_fbm_2d.restype = ctypes.c_double

        # Configuration functions (these don't exist in the current C library)
        # self._lib.simplex_config_set_seed.argtypes = [ctypes.c_uint]
        # self._lib.simplex_config_set_seed.restype = None

        # self._lib.simplex_config_get_seed.argtypes = []
        # self._lib.simplex_config_get_seed.restype = ctypes.c_uint

    def init(self, seed: int) -> None:
        """Initialize the noise generator with a new seed."""
        self._lib.simplex_noise_init(ctypes.c_uint(seed))
        self._seed = seed
        self._initialized = True

    @property
    def seed(self) -> int:
        """Get the current seed."""
        return self._seed

    @seed.setter
    def seed(self, value: int) -> None:
        """Set a new seed and reinitialize."""
        self.init(value)

    def noise_2d(
        self, x: Union[float, np.ndarray], y: Union[float, np.ndarray]
    ) -> Union[float, np.ndarray]:
        """
        Generate 2D simplex noise.

        Args:
            x: X coordinate(s)
            y: Y coordinate(s)

        Returns:
            Noise value(s) in range [-1, 1]
        """
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        # Handle NumPy arrays
        if isinstance(x, np.ndarray) or isinstance(y, np.ndarray):
            return self._noise_2d_array(x, y)

        # Single values
        return self._lib.simplex_noise_2d(ctypes.c_double(x), ctypes.c_double(y))

    def noise_3d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        z: Union[float, np.ndarray],
    ) -> Union[float, np.ndarray]:
        """Generate 3D simplex noise."""
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if (
            isinstance(x, np.ndarray)
            or isinstance(y, np.ndarray)
            or isinstance(z, np.ndarray)
        ):
            return self._noise_3d_array(x, y, z)

        return self._lib.simplex_noise_3d(
            ctypes.c_double(x), ctypes.c_double(y), ctypes.c_double(z)
        )

    def noise_4d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        z: Union[float, np.ndarray],
        w: Union[float, np.ndarray],
    ) -> Union[float, np.ndarray]:
        """Generate 4D simplex noise."""
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if (
            isinstance(x, np.ndarray)
            or isinstance(y, np.ndarray)
            or isinstance(z, np.ndarray)
            or isinstance(w, np.ndarray)
        ):
            return self._noise_4d_array(x, y, z, w)

        return self._lib.simplex_noise_4d(
            ctypes.c_double(x),
            ctypes.c_double(y),
            ctypes.c_double(z),
            ctypes.c_double(w),
        )

    def fractal_2d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        octaves: int = 4,
        persistence: float = 0.5,
        lacunarity: float = 2.0,
    ) -> Union[float, np.ndarray]:
        """
        Generate 2D fractal noise.

        Args:
            x: X coordinate(s)
            y: Y coordinate(s)
            octaves: Number of octaves
            persistence: Amplitude persistence (0.0-1.0)
            lacunarity: Frequency lacunarity (>1.0)

        Returns:
            Fractal noise value(s)
        """
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if isinstance(x, np.ndarray) or isinstance(y, np.ndarray):
            return self._fractal_2d_array(x, y, octaves, persistence, lacunarity)

        return self._lib.simplex_fractal_2d(
            ctypes.c_double(x),
            ctypes.c_double(y),
            ctypes.c_int(octaves),
            ctypes.c_double(persistence),
            ctypes.c_double(lacunarity),
        )

    def fractal_3d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        z: Union[float, np.ndarray],
        octaves: int = 4,
        persistence: float = 0.5,
        lacunarity: float = 2.0,
    ) -> Union[float, np.ndarray]:
        """Generate 3D fractal noise."""
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if (
            isinstance(x, np.ndarray)
            or isinstance(y, np.ndarray)
            or isinstance(z, np.ndarray)
        ):
            return self._fractal_3d_array(x, y, z, octaves, persistence, lacunarity)

        return self._lib.simplex_fractal_3d(
            ctypes.c_double(x),
            ctypes.c_double(y),
            ctypes.c_double(z),
            ctypes.c_int(octaves),
            ctypes.c_double(persistence),
            ctypes.c_double(lacunarity),
        )

    def ridged_2d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        offset: float = 1.0,
    ) -> Union[float, np.ndarray]:
        """Generate 2D ridged noise."""
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if isinstance(x, np.ndarray) or isinstance(y, np.ndarray):
            return self._ridged_2d_array(x, y, offset)

        return self._lib.simplex_ridged_2d(
            ctypes.c_double(x), ctypes.c_double(y), ctypes.c_double(offset)
        )

    def billowy_2d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        offset: float = 1.0,
    ) -> Union[float, np.ndarray]:
        """Generate 2D billowy noise."""
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if isinstance(x, np.ndarray) or isinstance(y, np.ndarray):
            return self._billowy_2d_array(x, y, offset)

        return self._lib.simplex_billowy_2d(
            ctypes.c_double(x), ctypes.c_double(y), ctypes.c_double(offset)
        )

    def fbm_2d(
        self,
        x: Union[float, np.ndarray],
        y: Union[float, np.ndarray],
        octaves: int = 4,
        persistence: float = 0.5,
        lacunarity: float = 2.0,
    ) -> Union[float, np.ndarray]:
        """Generate 2D Fractional Brownian Motion noise."""
        if not self._initialized:
            raise RuntimeError("Noise generator not initialized. Call init() first.")

        if isinstance(x, np.ndarray) or isinstance(y, np.ndarray):
            return self._fbm_2d_array(x, y, octaves, persistence, lacunarity)

        return self._lib.simplex_fbm_2d(
            ctypes.c_double(x),
            ctypes.c_double(y),
            ctypes.c_int(octaves),
            ctypes.c_double(persistence),
            ctypes.c_double(lacunarity),
        )

    # Array processing methods
    def _noise_2d_array(self, x: np.ndarray, y: np.ndarray) -> np.ndarray:
        """Process 2D noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)

        if x.shape != y.shape:
            raise ValueError("x and y arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_noise_2d(
                ctypes.c_double(x.flat[i]), ctypes.c_double(y.flat[i])
            )

        return result

    def _noise_3d_array(
        self, x: np.ndarray, y: np.ndarray, z: np.ndarray
    ) -> np.ndarray:
        """Process 3D noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)
        z = np.asarray(z, dtype=np.float64)

        if not (x.shape == y.shape == z.shape):
            raise ValueError("x, y, and z arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_noise_3d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_double(z.flat[i]),
            )

        return result

    def _noise_4d_array(
        self, x: np.ndarray, y: np.ndarray, z: np.ndarray, w: np.ndarray
    ) -> np.ndarray:
        """Process 4D noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)
        z = np.asarray(z, dtype=np.float64)
        w = np.asarray(w, dtype=np.float64)

        if not (x.shape == y.shape == z.shape == w.shape):
            raise ValueError("x, y, z, and w arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_noise_4d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_double(z.flat[i]),
                ctypes.c_double(w.flat[i]),
            )

        return result

    def _fractal_2d_array(
        self,
        x: np.ndarray,
        y: np.ndarray,
        octaves: int,
        persistence: float,
        lacunarity: float,
    ) -> np.ndarray:
        """Process 2D fractal noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)

        if x.shape != y.shape:
            raise ValueError("x and y arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_fractal_2d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_int(octaves),
                ctypes.c_double(persistence),
                ctypes.c_double(lacunarity),
            )

        return result

    def _fractal_3d_array(
        self,
        x: np.ndarray,
        y: np.ndarray,
        z: np.ndarray,
        octaves: int,
        persistence: float,
        lacunarity: float,
    ) -> np.ndarray:
        """Process 3D fractal noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)
        z = np.asarray(z, dtype=np.float64)

        if not (x.shape == y.shape == z.shape):
            raise ValueError("x, y, and z arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_fractal_3d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_double(z.flat[i]),
                ctypes.c_int(octaves),
                ctypes.c_double(persistence),
                ctypes.c_double(lacunarity),
            )

        return result

    def _ridged_2d_array(
        self, x: np.ndarray, y: np.ndarray, offset: float
    ) -> np.ndarray:
        """Process 2D ridged noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)

        if x.shape != y.shape:
            raise ValueError("x and y arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_ridged_2d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_double(offset),
            )

        return result

    def _billowy_2d_array(
        self, x: np.ndarray, y: np.ndarray, offset: float
    ) -> np.ndarray:
        """Process 2D billowy noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)

        if x.shape != y.shape:
            raise ValueError("x and y arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_billowy_2d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_double(offset),
            )

        return result

    def _fbm_2d_array(
        self,
        x: np.ndarray,
        y: np.ndarray,
        octaves: int,
        persistence: float,
        lacunarity: float,
    ) -> np.ndarray:
        """Process 2D FBM noise for arrays."""
        x = np.asarray(x, dtype=np.float64)
        y = np.asarray(y, dtype=np.float64)

        if x.shape != y.shape:
            raise ValueError("x and y arrays must have the same shape")

        result = np.zeros_like(x)
        for i in range(x.size):
            result.flat[i] = self._lib.simplex_fbm_2d(
                ctypes.c_double(x.flat[i]),
                ctypes.c_double(y.flat[i]),
                ctypes.c_int(octaves),
                ctypes.c_double(persistence),
                ctypes.c_double(lacunarity),
            )

        return result

    # Image generation
    def generate_image(
        self,
        filename: str,
        width: int = 512,
        height: int = 512,
        color_mode: str = "grayscale",
        octaves: int = 4,
        persistence: float = 0.5,
        lacunarity: float = 2.0,
    ) -> None:
        """
        Generate a noise image and save it to file.

        Args:
            filename: Output filename
            width: Image width
            height: Image height
            color_mode: "grayscale", "rgb", or "heightmap"
            octaves: Number of octaves for fractal noise
            persistence: Amplitude persistence
            lacunarity: Frequency lacunarity
        """
        if not PIL_AVAILABLE:
            raise ImportError(
                "PIL (Pillow) is required for image generation. Install with: pip install Pillow"
            )

        # Generate noise data
        x = np.linspace(0, 10, width)
        y = np.linspace(0, 10, height)
        X, Y = np.meshgrid(x, y)

        if octaves > 1:
            noise_data = self.fractal_2d(X, Y, octaves, persistence, lacunarity)
        else:
            noise_data = self.noise_2d(X, Y)

        # Normalize to [0, 1]
        noise_data = (noise_data + 1.0) / 2.0
        noise_data = np.clip(noise_data, 0.0, 1.0)

        # Convert to image
        if color_mode == "grayscale":
            image_data = (noise_data * 255).astype(np.uint8)
            image = Image.fromarray(image_data, mode="L")
        elif color_mode == "rgb":
            # Create RGB image with noise as intensity
            image_data = (noise_data * 255).astype(np.uint8)
            image = Image.fromarray(np.stack([image_data] * 3, axis=-1), mode="RGB")
        elif color_mode == "heightmap":
            # Create heightmap with terrain-like colors
            image_data = (noise_data * 255).astype(np.uint8)
            # Simple terrain coloring
            terrain_colors = np.zeros((height, width, 3), dtype=np.uint8)
            for i in range(height):
                for j in range(width):
                    height_val = image_data[i, j]
                    if height_val < 85:  # Water
                        terrain_colors[i, j] = [0, 0, 128 + height_val]
                    elif height_val < 128:  # Sand
                        terrain_colors[i, j] = [194, 178, 128]
                    elif height_val < 170:  # Grass
                        terrain_colors[i, j] = [0, 128 + height_val // 2, 0]
                    else:  # Mountain
                        terrain_colors[i, j] = [height_val, height_val, height_val]
            image = Image.fromarray(terrain_colors, mode="RGB")
        else:
            raise ValueError(f"Unknown color_mode: {color_mode}")

        # Save image
        image.save(filename)
        print(f"Image saved to {filename}")


# Convenience functions
def noise_2d(
    x: Union[float, np.ndarray], y: Union[float, np.ndarray], seed: int = 0
) -> Union[float, np.ndarray]:
    """Generate 2D simplex noise with a given seed."""
    noise = SimplexNoise(seed)
    return noise.noise_2d(x, y)


def fractal_2d(
    x: Union[float, np.ndarray],
    y: Union[float, np.ndarray],
    octaves: int = 4,
    persistence: float = 0.5,
    lacunarity: float = 2.0,
    seed: int = 0,
) -> Union[float, np.ndarray]:
    """Generate 2D fractal noise with a given seed."""
    noise = SimplexNoise(seed)
    return noise.fractal_2d(x, y, octaves, persistence, lacunarity)


def generate_terrain(
    width: int = 512,
    height: int = 512,
    seed: int = 0,
    octaves: int = 6,
    persistence: float = 0.5,
    lacunarity: float = 2.0,
) -> np.ndarray:
    """Generate a terrain heightmap."""
    noise = SimplexNoise(seed)
    x = np.linspace(0, 10, width)
    y = np.linspace(0, 10, height)
    X, Y = np.meshgrid(x, y)
    return noise.fractal_2d(X, Y, octaves, persistence, lacunarity)


# Version info
__version__ = "1.0.1"
__author__ = "Adrian Paredez"
__license__ = "MIT"
