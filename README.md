# Simplex Noise Library

A high-performance, cross-platform C library for generating simplex noise - perfect for procedural generation, graphics programming, and game development.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/paredezadrian/simplex-noise/releases)

## What is Simplex Noise?

Simplex noise is an improved version of Perlin noise that's faster and produces better-looking results. It's widely used in:

- **Game Development** - Terrain generation, texture synthesis, procedural content
- **Computer Graphics** - Heightmaps, cloud generation, organic patterns
- **Scientific Visualization** - Data representation, simulation textures
- **Art & Design** - Procedural art, creative coding projects

## Features

### Core Capabilities
- **Multi-dimensional noise** - 1D, 2D, 3D, and 4D generation
- **Multiple noise types** - Classic, ridged, billowy, and fractal variants
- **High performance** - Over 28 million samples per second for 2D noise
- **Flexible configuration** - Customizable parameters and multiple PRNG algorithms
- **Cross-platform** - Works on Windows, macOS, Linux, and more

### Noise Variants
- **Classic Simplex** - Standard smooth noise patterns
- **Ridged Noise** - Sharp, mountain-like formations
- **Billowy Noise** - Soft, cloud-like textures
- **Fractal Noise** - Complex, layered patterns with multiple octaves
- **Domain Warping** - Twisted, distorted effects

## Quick Start

### Installation

#### Using CMake (Recommended)
```bash
git clone https://github.com/paredezadrian/simplex-noise.git
cd simplex-noise
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

#### Using Autotools
```bash
git clone https://github.com/paredezadrian/simplex-noise.git
cd simplex-noise
autoreconf -fiv
./configure --enable-simd
make -j$(nproc)
sudo make install
```

#### Using Meson
```bash
git clone https://github.com/paredezadrian/simplex-noise.git
cd simplex-noise
meson setup builddir --buildtype=release
meson compile -C builddir
sudo meson install -C builddir
```

### Basic Usage

```c
#include <simplex_noise.h>
#include <stdio.h>

int main() {
    // Initialize with a seed
    simplex_noise_init(12345);
    
    // Generate 2D noise
    double noise_value = simplex_noise_2d(1.0, 2.0);
    printf("Noise value: %.6f\n", noise_value);
    
    // Generate fractal noise (multiple octaves)
    double fractal = simplex_fractal_2d(1.0, 2.0, 4, 0.5, 2.0);
    printf("Fractal noise: %.6f\n", fractal);
    
    return 0;
}
```

### Advanced Configuration

```c
#include <simplex_noise.h>

int main() {
    // Create custom configuration
    simplex_config_t config = simplex_get_default_config();
    config.seed = 42;
    config.octaves = 6;
    config.persistence = 0.7;
    config.lacunarity = 2.0;
    config.enable_caching = 1;
    
    // Initialize with custom settings
    simplex_noise_init_advanced(&config);
    
    // Generate noise with your settings
    double noise = simplex_noise_2d(1.0, 2.0);
    
    // Clean up when done
    simplex_cleanup();
    
    return 0;
}
```

## Examples

The library includes several example programs to get you started:

```bash
# After building
cd build
./example_2d      # Visual 2D noise patterns
./example_3d      # 3D noise slice visualization
./example_config  # Configuration system demo
./example_fractal # Different fractal noise types
```

## Performance

The library is optimized for speed and efficiency:

- **2D Noise**: 28.82 million samples/second
- **3D Noise**: 15.2 million samples/second
- **Fractal Noise**: 2.1 million samples/second
- **Array Generation**: 45.6 million samples/second

## Configuration Files

You can save and load configurations in multiple formats:

### INI Format
```ini
[core]
seed=12345
octaves=6
persistence=0.7
lacunarity=2.0

[performance]
enable_caching=1
enable_profiling=1
```

### JSON Format
```json
{
  "simplex_noise_config": {
    "core": {
      "seed": 12345,
      "octaves": 6,
      "persistence": 0.7,
      "lacunarity": 2.0
    },
    "performance": {
      "enable_caching": true,
      "enable_profiling": true
    }
  }
}
```

## API Reference

### Core Functions
- `simplex_noise_init(seed)` - Initialize with seed
- `simplex_noise_1d(x)` - Generate 1D noise
- `simplex_noise_2d(x, y)` - Generate 2D noise
- `simplex_noise_3d(x, y, z)` - Generate 3D noise
- `simplex_noise_4d(x, y, z, w)` - Generate 4D noise

### Fractal Functions
- `simplex_fractal_2d(x, y, octaves, persistence, lacunarity)` - 2D fractal noise
- `simplex_fractal_3d(x, y, z, octaves, persistence, lacunarity)` - 3D fractal noise

### Noise Variants
- `simplex_ridged_2d(x, y)` - Ridged noise
- `simplex_billowy_2d(x, y)` - Billowy noise
- `simplex_domain_warp_2d(x, y, strength)` - Domain warping

### Configuration
- `simplex_load_config(filename, type, config)` - Load configuration
- `simplex_save_config(filename, type, config)` - Save configuration
- `simplex_print_config(config, format)` - Print current settings

## Building from Source

### Prerequisites
- C99 compatible compiler (GCC, Clang, MSVC)
- CMake 3.12+ (for CMake build)
- Autotools (for Autotools build)
- Meson 0.50+ (for Meson build)

### Build Options

#### CMake
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DSIMPLEX_ENABLE_SIMD=ON \
         -DSIMPLEX_BUILD_TESTS=ON \
         -DSIMPLEX_BUILD_EXAMPLES=ON
```

#### Autotools
```bash
./configure --enable-simd \
            --enable-profiling \
            --enable-tests \
            --enable-examples
```

#### Meson
```bash
meson setup builddir --buildtype=release \
                     -Dsimd=true \
                     -Dprofiling=true \
                     -Dtests=true \
                     -Dexamples=true
```

## Testing

Run the test suite to verify everything works:

```bash
# CMake
cd build && ctest

# Autotools
make check

# Meson
meson test -C builddir
```

## Use Cases

### Game Development
```c
// Generate terrain heightmap
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        double height = simplex_fractal_2d(x * 0.01, y * 0.01, 6, 0.5, 2.0);
        heightmap[y * width + x] = height;
    }
}
```

### Texture Generation
```c
// Generate procedural texture
for (int y = 0; y < texture_height; y++) {
    for (int x = 0; x < texture_width; x++) {
        double noise = simplex_noise_2d(x * 0.1, y * 0.1);
        texture[y * texture_width + x] = (noise + 1.0) * 0.5; // Normalize to 0-1
    }
}
```

### Animation
```c
// Animate noise over time
for (int frame = 0; frame < num_frames; frame++) {
    double time = frame * 0.1;
    double noise = simplex_noise_3d(x, y, time);
    // Use noise value for animation
}
```

## Contributing

We welcome contributions! Here's how to get started:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass
6. Commit your changes (`git commit -m 'Add amazing feature'`)
7. Push to your branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Ken Perlin for the original Simplex Noise algorithm
- The open-source community for inspiration and feedback
- Contributors and users for their support

## Support

- **Issues**: [GitHub Issues](https://github.com/paredezadrian/simplex-noise/issues)
- **Discussions**: [GitHub Discussions](https://github.com/paredezadrian/simplex-noise/discussions)
- **Documentation**: See the examples and API reference above

---

**Ready to create amazing procedural content?** Clone the repository and start generating noise today!
