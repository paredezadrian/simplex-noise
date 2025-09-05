#!/usr/bin/env python3
"""
Image Generation Example

This example demonstrates how to generate various types of images using simplex noise.
"""

import numpy as np
import matplotlib.pyplot as plt
from simplex_noise import SimplexNoise


def generate_texture(width: int, height: int, seed: int = 42) -> np.ndarray:
    """Generate a procedural texture using simplex noise."""
    noise = SimplexNoise(seed)

    # Create coordinate grids
    x = np.linspace(0, 10, width)
    y = np.linspace(0, 10, height)
    X, Y = np.meshgrid(x, y)

    # Generate multiple noise layers
    base = noise.fractal_2d(X, Y, octaves=4, persistence=0.5, lacunarity=2.0)
    detail = noise.fractal_2d(X * 2, Y * 2, octaves=2, persistence=0.3, lacunarity=2.5)

    # Combine layers
    texture = base + 0.3 * detail

    # Normalize to [0, 1]
    texture = (texture - texture.min()) / (texture.max() - texture.min())

    return texture


def generate_clouds(width: int, height: int, seed: int = 42) -> np.ndarray:
    """Generate cloud-like patterns using simplex noise."""
    noise = SimplexNoise(seed)

    x = np.linspace(0, 5, width)
    y = np.linspace(0, 5, height)
    X, Y = np.meshgrid(x, y)

    # Generate cloud noise
    clouds = noise.fractal_2d(X, Y, octaves=6, persistence=0.4, lacunarity=2.0)

    # Apply cloud-like transformation
    clouds = np.abs(clouds)
    clouds = np.power(clouds, 0.5)  # Soften the edges

    # Normalize
    clouds = (clouds - clouds.min()) / (clouds.max() - clouds.min())

    return clouds


def generate_marble(width: int, height: int, seed: int = 42) -> np.ndarray:
    """Generate marble-like patterns using simplex noise."""
    noise = SimplexNoise(seed)

    x = np.linspace(0, 8, width)
    y = np.linspace(0, 8, height)
    X, Y = np.meshgrid(x, y)

    # Generate marble pattern
    marble = noise.fractal_2d(X, Y, octaves=4, persistence=0.5, lacunarity=2.0)

    # Apply marble transformation
    marble = np.sin(marble * 10) * 0.5 + 0.5

    return marble


def generate_wood(width: int, height: int, seed: int = 42) -> np.ndarray:
    """Generate wood-like patterns using simplex noise."""
    noise = SimplexNoise(seed)

    x = np.linspace(0, 6, width)
    y = np.linspace(0, 6, height)
    X, Y = np.meshgrid(x, y)

    # Generate wood pattern
    wood = noise.fractal_2d(X, Y, octaves=3, persistence=0.4, lacunarity=2.0)

    # Apply wood transformation
    wood = np.sin(wood * 8 + Y * 2) * 0.5 + 0.5

    return wood


def generate_fire(width: int, height: int, seed: int = 42) -> np.ndarray:
    """Generate fire-like patterns using simplex noise."""
    noise = SimplexNoise(seed)

    x = np.linspace(0, 4, width)
    y = np.linspace(0, 4, height)
    X, Y = np.meshgrid(x, y)

    # Generate fire pattern
    fire = noise.fractal_2d(X, Y, octaves=5, persistence=0.6, lacunarity=2.0)

    # Apply fire transformation
    fire = np.abs(fire)
    fire = np.power(fire, 0.3)

    # Add vertical gradient
    fire = fire * (1.0 - Y / height)

    return fire


def main():
    print("=== Image Generation Example ===\n")

    # Generate different types of images
    width, height = 512, 512

    print("Generating textures...")
    texture = generate_texture(width, height, seed=42)
    clouds = generate_clouds(width, height, seed=123)
    marble = generate_marble(width, height, seed=456)
    wood = generate_wood(width, height, seed=789)
    fire = generate_fire(width, height, seed=321)

    # Create visualization
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))

    # Texture
    axes[0, 0].imshow(texture, cmap="gray", origin="lower")
    axes[0, 0].set_title("Procedural Texture")
    axes[0, 0].axis("off")

    # Clouds
    axes[0, 1].imshow(clouds, cmap="Blues", origin="lower")
    axes[0, 1].set_title("Clouds")
    axes[0, 1].axis("off")

    # Marble
    axes[0, 2].imshow(marble, cmap="gray", origin="lower")
    axes[0, 2].set_title("Marble")
    axes[0, 2].axis("off")

    # Wood
    axes[1, 0].imshow(wood, cmap="YlOrBr", origin="lower")
    axes[1, 0].set_title("Wood")
    axes[1, 0].axis("off")

    # Fire
    axes[1, 1].imshow(fire, cmap="hot", origin="lower")
    axes[1, 1].set_title("Fire")
    axes[1, 1].axis("off")

    # Hide the last subplot
    axes[1, 2].axis("off")

    plt.tight_layout()
    plt.savefig("texture_examples.png", dpi=150, bbox_inches="tight")
    print("Texture examples saved as 'texture_examples.png'")

    # Generate colored versions
    print("\nGenerating colored versions...")

    # Colored clouds
    cloud_colors = np.zeros((height, width, 3), dtype=np.uint8)
    cloud_colors[:, :, 0] = (clouds * 200).astype(np.uint8)  # Red
    cloud_colors[:, :, 1] = (clouds * 200).astype(np.uint8)  # Green
    cloud_colors[:, :, 2] = (clouds * 255).astype(np.uint8)  # Blue

    # Colored marble
    marble_colors = np.zeros((height, width, 3), dtype=np.uint8)
    marble_colors[:, :, 0] = (marble * 200 + 55).astype(np.uint8)  # Red
    marble_colors[:, :, 1] = (marble * 150 + 105).astype(np.uint8)  # Green
    marble_colors[:, :, 2] = (marble * 100 + 155).astype(np.uint8)  # Blue

    # Colored wood
    wood_colors = np.zeros((height, width, 3), dtype=np.uint8)
    wood_colors[:, :, 0] = (wood * 150 + 105).astype(np.uint8)  # Red
    wood_colors[:, :, 1] = (wood * 100 + 155).astype(np.uint8)  # Green
    wood_colors[:, :, 2] = (wood * 50 + 205).astype(np.uint8)  # Blue

    # Colored fire
    fire_colors = np.zeros((height, width, 3), dtype=np.uint8)
    fire_colors[:, :, 0] = (fire * 255).astype(np.uint8)  # Red
    fire_colors[:, :, 1] = (fire * 150).astype(np.uint8)  # Green
    fire_colors[:, :, 2] = (fire * 50).astype(np.uint8)  # Blue

    # Save colored images
    fig, axes = plt.subplots(2, 2, figsize=(10, 10))

    axes[0, 0].imshow(cloud_colors, origin="lower")
    axes[0, 0].set_title("Colored Clouds")
    axes[0, 0].axis("off")

    axes[0, 1].imshow(marble_colors, origin="lower")
    axes[0, 1].set_title("Colored Marble")
    axes[0, 1].axis("off")

    axes[1, 0].imshow(wood_colors, origin="lower")
    axes[1, 0].set_title("Colored Wood")
    axes[1, 0].axis("off")

    axes[1, 1].imshow(fire_colors, origin="lower")
    axes[1, 1].set_title("Colored Fire")
    axes[1, 1].axis("off")

    plt.tight_layout()
    plt.savefig("colored_textures.png", dpi=150, bbox_inches="tight")
    print("Colored textures saved as 'colored_textures.png'")

    print("\n=== Image Generation Complete ===")


if __name__ == "__main__":
    main()
