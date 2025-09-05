#!/usr/bin/env python3
"""
Basic Usage Examples for Simplex Noise Python Wrapper

This example demonstrates the basic usage of the simplex noise library.
"""

import numpy as np
from simplex_noise import SimplexNoise, noise_2d, fractal_2d, generate_terrain


def main():
    print("=== Basic Simplex Noise Usage ===\n")

    # Initialize noise generator
    noise = SimplexNoise(seed=42)
    print(f"Initialized with seed: {noise.seed}")

    # Single value generation
    print("\n--- Single Values ---")
    value = noise.noise_2d(1.0, 2.0)
    print(f"2D noise at (1.0, 2.0): {value:.6f}")

    value_3d = noise.noise_3d(1.0, 2.0, 3.0)
    print(f"3D noise at (1.0, 2.0, 3.0): {value_3d:.6f}")

    value_4d = noise.noise_4d(1.0, 2.0, 3.0, 4.0)
    print(f"4D noise at (1.0, 2.0, 3.0, 4.0): {value_4d:.6f}")

    # Fractal noise
    print("\n--- Fractal Noise ---")
    fractal = noise.fractal_2d(1.0, 2.0, octaves=4, persistence=0.5, lacunarity=2.0)
    print(f"2D fractal noise: {fractal:.6f}")

    # Array generation
    print("\n--- Array Generation ---")
    x = np.linspace(0, 5, 10)
    y = np.linspace(0, 5, 10)
    X, Y = np.meshgrid(x, y)

    noise_array = noise.noise_2d(X, Y)
    print(f"Generated {noise_array.shape} array of noise values")
    print(f"Min: {noise_array.min():.6f}, Max: {noise_array.max():.6f}")

    # Different noise types
    print("\n--- Noise Variants ---")
    ridged = noise.ridged_2d(1.0, 2.0, offset=1.0)
    billowy = noise.billowy_2d(1.0, 2.0, offset=1.0)
    fbm = noise.fbm_2d(1.0, 2.0, octaves=4, persistence=0.5, lacunarity=2.0)

    print(f"Ridged noise: {ridged:.6f}")
    print(f"Billowy noise: {billowy:.6f}")
    print(f"FBM noise: {fbm:.6f}")

    # Convenience functions
    print("\n--- Convenience Functions ---")
    quick_noise = noise_2d(1.0, 2.0, seed=123)
    quick_fractal = fractal_2d(1.0, 2.0, octaves=4, seed=123)
    terrain = generate_terrain(64, 64, seed=456, octaves=6)

    print(f"Quick 2D noise: {quick_noise:.6f}")
    print(f"Quick fractal: {quick_fractal:.6f}")
    print(f"Terrain shape: {terrain.shape}")

    print("\n=== Basic Usage Complete ===")


if __name__ == "__main__":
    main()
