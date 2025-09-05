# Pure C Simplex Noise - Python Wrapper

A high-performance Python wrapper for the Pure C Simplex Noise library, providing easy access to procedural noise generation with NumPy integration.

## Features

- **High Performance**: Direct C library integration for maximum speed
- **NumPy Integration**: Generate noise for arrays and matrices
- **Multiple Noise Types**: Classic, fractal, ridged, billowy, and FBM noise
- **Image Generation**: Create textures, terrains, and procedural images
- **Easy to Use**: Simple Python API with comprehensive examples
- **Cross-Platform**: Works on Windows, macOS, and Linux

## Quick Start

### Installation

1. **Build the C library first** (from the project root):

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

2. **Install the Python wrapper**:
   ```bash
   cd python
   pip install simplex-noise
   ```

### Basic Usage

```python
from simplex_noise import SimplexNoise
import numpy as np

# Initialize noise generator
noise = SimplexNoise(seed=42)

# Generate single values
value = noise.noise_2d(1.0, 2.0)
print(f"2D noise: {value}")

# Generate arrays
x = np.linspace(0, 10, 100)
y = np.linspace(0, 10, 100)
X, Y = np.meshgrid(x, y)
noise_array = noise.noise_2d(X, Y)

# Generate fractal noise
fractal = noise.fractal_2d(X, Y, octaves=4, persistence=0.5, lacunarity=2.0)

# Generate images
noise.generate_image("terrain.png", width=512, height=512,
                    color_mode="heightmap", octaves=6)
```

## Examples

### Terrain Generation

```python
from simplex_noise import SimplexNoise
import numpy as np
import matplotlib.pyplot as plt

# Create terrain
noise = SimplexNoise(seed=42)
x = np.linspace(0, 20, 512)
y = np.linspace(0, 20, 512)
X, Y = np.meshgrid(x, y)

# Generate terrain with multiple layers
base_terrain = noise.fractal_2d(X, Y, octaves=6, persistence=0.5, lacunarity=2.0)
mountains = noise.fractal_2d(X * 2, Y * 2, octaves=4, persistence=0.3, lacunarity=2.5)
hills = noise.fractal_2d(X * 4, Y * 4, octaves=3, persistence=0.2, lacunarity=2.0)

terrain = base_terrain + 0.3 * mountains + 0.1 * hills

# Visualize
plt.imshow(terrain, cmap='terrain')
plt.colorbar()
plt.title('Procedural Terrain')
plt.show()
```

### Texture Generation

```python
from simplex_noise import SimplexNoise

noise = SimplexNoise(seed=123)

# Generate different textures
noise.generate_image("clouds.png", 512, 512, color_mode="grayscale", octaves=6)
noise.generate_image("marble.png", 512, 512, color_mode="rgb", octaves=4)
noise.generate_image("terrain.png", 512, 512, color_mode="heightmap", octaves=6)
```

## API Reference

### SimplexNoise Class

#### Constructor

```python
SimplexNoise(seed=0, library_path=None)
```

#### Methods

**Basic Noise Generation:**

- `noise_2d(x, y)` - Generate 2D simplex noise
- `noise_3d(x, y, z)` - Generate 3D simplex noise
- `noise_4d(x, y, z, w)` - Generate 4D simplex noise

**Fractal Noise:**

- `fractal_2d(x, y, octaves=4, persistence=0.5, lacunarity=2.0)` - 2D fractal noise
- `fractal_3d(x, y, z, octaves=4, persistence=0.5, lacunarity=2.0)` - 3D fractal noise

**Noise Variants:**

- `ridged_2d(x, y, offset=1.0)` - Ridged noise
- `billowy_2d(x, y, offset=1.0)` - Billowy noise
- `fbm_2d(x, y, octaves=4, persistence=0.5, lacunarity=2.0)` - Fractional Brownian Motion

**Image Generation:**

- `generate_image(filename, width=512, height=512, color_mode="grayscale", ...)` - Generate noise images

### Convenience Functions

- `noise_2d(x, y, seed=0)` - Quick 2D noise generation
- `fractal_2d(x, y, octaves=4, persistence=0.5, lacunarity=2.0, seed=0)` - Quick fractal noise
- `generate_terrain(width, height, seed=0, ...)` - Generate terrain heightmap

## Requirements

- Python 3.7+
- NumPy 1.15.0+
- Pillow 6.0.0+ (for image generation)
- Compiled C library (built automatically during installation)

## Development

### Running Tests

```bash
cd python
python -m pytest tests/
```

### Running Examples

```bash
cd python
python examples/basic_usage.py
python examples/terrain_generation.py
python examples/image_generation.py
```

## Performance

The Python wrapper provides near-native C performance:

- **Single values**: ~1-2 microseconds per call
- **Array operations**: ~10-50 nanoseconds per element
- **Image generation**: ~100-500 milliseconds for 512x512 images

## License

MIT License - see the main project LICENSE.md for details.

## Contributing

Contributions are welcome! Please see the main project repository for contribution guidelines.

## Links

- [Main Project Repository](https://github.com/paredezadrian/noise)
- [Documentation](https://paredezadrian.github.io/noise/)
- [C Library Source](https://github.com/paredezadrian/noise/tree/main/src)
