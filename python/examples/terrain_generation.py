#!/usr/bin/env python3
"""
Terrain Generation Example

This example demonstrates how to generate realistic terrain using simplex noise.
"""

import numpy as np
import matplotlib.pyplot as plt
from simplex_noise import SimplexNoise


def generate_heightmap(width: int, height: int, seed: int = 42) -> np.ndarray:
    """Generate a terrain heightmap using multiple noise layers."""
    noise = SimplexNoise(seed)

    # Create coordinate grids
    x = np.linspace(0, 20, width)
    y = np.linspace(0, 20, height)
    X, Y = np.meshgrid(x, y)

    # Base terrain (large scale features)
    base_terrain = noise.fractal_2d(X, Y, octaves=6, persistence=0.5, lacunarity=2.0)

    # Mountain ranges (medium scale)
    mountains = noise.fractal_2d(
        X * 2, Y * 2, octaves=4, persistence=0.3, lacunarity=2.5
    )
    mountains = np.abs(mountains)  # Make them positive

    # Hills and valleys (small scale)
    hills = noise.fractal_2d(X * 4, Y * 4, octaves=3, persistence=0.2, lacunarity=2.0)

    # Combine layers
    terrain = base_terrain + 0.3 * mountains + 0.1 * hills

    # Normalize to [0, 1]
    terrain = (terrain - terrain.min()) / (terrain.max() - terrain.min())

    return terrain


def generate_terrain_colors(heightmap: np.ndarray) -> np.ndarray:
    """Generate terrain colors based on height."""
    height, width = heightmap.shape
    colors = np.zeros((height, width, 3), dtype=np.uint8)

    for i in range(height):
        for j in range(width):
            h = heightmap[i, j]

            if h < 0.2:  # Water
                colors[i, j] = [0, 0, 128 + int(h * 127)]
            elif h < 0.3:  # Sand
                colors[i, j] = [194, 178, 128]
            elif h < 0.5:  # Grass
                colors[i, j] = [0, 128 + int(h * 127), 0]
            elif h < 0.7:  # Forest
                colors[i, j] = [0, 64 + int(h * 64), 0]
            elif h < 0.9:  # Rock
                colors[i, j] = [128, 128, 128]
            else:  # Snow
                colors[i, j] = [255, 255, 255]

    return colors


def main():
    print("=== Terrain Generation Example ===\n")

    # Generate terrain
    print("Generating terrain...")
    width, height = 512, 512
    terrain = generate_heightmap(width, height, seed=42)

    print(f"Terrain shape: {terrain.shape}")
    print(f"Height range: {terrain.min():.3f} to {terrain.max():.3f}")

    # Generate colors
    print("Generating terrain colors...")
    colors = generate_terrain_colors(terrain)

    # Create visualization
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    # Heightmap
    im1 = axes[0].imshow(terrain, cmap="terrain", origin="lower")
    axes[0].set_title("Heightmap")
    axes[0].set_xlabel("X")
    axes[0].set_ylabel("Y")
    plt.colorbar(im1, ax=axes[0])

    # Terrain colors
    axes[1].imshow(colors, origin="lower")
    axes[1].set_title("Terrain Colors")
    axes[1].set_xlabel("X")
    axes[1].set_ylabel("Y")

    # 3D surface
    x = np.linspace(0, 20, width)
    y = np.linspace(0, 20, height)
    X, Y = np.meshgrid(x, y)

    # Subsample for 3D plot
    step = 8
    X_sub = X[::step, ::step]
    Y_sub = Y[::step, ::step]
    Z_sub = terrain[::step, ::step]

    ax3d = fig.add_subplot(133, projection="3d")
    ax3d.plot_surface(X_sub, Y_sub, Z_sub, cmap="terrain", alpha=0.8)
    ax3d.set_title("3D Terrain")
    ax3d.set_xlabel("X")
    ax3d.set_ylabel("Y")
    ax3d.set_zlabel("Height")

    plt.tight_layout()
    plt.savefig("terrain_example.png", dpi=150, bbox_inches="tight")
    print("Terrain visualization saved as 'terrain_example.png'")

    # Generate different terrain types
    print("\n--- Different Terrain Types ---")

    # Desert
    noise = SimplexNoise(seed=123)
    x = np.linspace(0, 10, 256)
    y = np.linspace(0, 10, 256)
    X, Y = np.meshgrid(x, y)

    desert = noise.fractal_2d(X, Y, octaves=4, persistence=0.3, lacunarity=2.5)
    desert = (desert - desert.min()) / (desert.max() - desert.min())

    # Ocean
    ocean = noise.fractal_2d(
        X * 0.5, Y * 0.5, octaves=3, persistence=0.4, lacunarity=2.0
    )
    ocean = (ocean - ocean.min()) / (ocean.max() - ocean.min())
    ocean = ocean * 0.3  # Keep it low for water

    # Save examples
    plt.figure(figsize=(12, 4))

    plt.subplot(1, 3, 1)
    plt.imshow(terrain, cmap="terrain", origin="lower")
    plt.title("Mountain Terrain")
    plt.colorbar()

    plt.subplot(1, 3, 2)
    plt.imshow(desert, cmap="YlOrBr", origin="lower")
    plt.title("Desert Terrain")
    plt.colorbar()

    plt.subplot(1, 3, 3)
    plt.imshow(ocean, cmap="Blues", origin="lower")
    plt.title("Ocean Terrain")
    plt.colorbar()

    plt.tight_layout()
    plt.savefig("terrain_types.png", dpi=150, bbox_inches="tight")
    print("Terrain types saved as 'terrain_types.png'")

    print("\n=== Terrain Generation Complete ===")


if __name__ == "__main__":
    main()
