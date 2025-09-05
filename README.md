# Simplex Noise Library Documentation

Welcome to the comprehensive documentation for the Simplex Noise Library - a professional-grade, Pure C implementation of Ken Perlin's improved noise algorithm.

## Table of Contents

- [Overview](overview.md)
- [API Reference](api-reference.md)
- [Getting Started](getting-started.md)
- [Examples](examples.md)
- [Advanced Usage](advanced-usage.md)
- [Performance Guide](performance.md)
- [Configuration](configuration.md)
- [Image Generation](image-generation.md)
- [Build System](build-system.md)
- [Contributing](contributing.md)

## Quick Start

```c
#include "simplex_noise.h"

// Initialize with default settings
simplex_noise_init(12345);

// Generate 2D noise
double noise = simplex_noise_2d(1.0, 2.0);

// Generate fractal noise
double fractal = simplex_fractal_2d(1.0, 2.0, 4, 0.5, 2.0);
```

## Features

- ✅ **1D, 2D, 3D, and 4D** simplex noise generation
- ✅ **Multiple PRNG algorithms** (LCG, Mersenne Twister, Xorshift, PCG)
- ✅ **Advanced noise variants** (Ridged, Billowy, fBm, Hybrid Multi-Fractal)
- ✅ **Fractal noise** with configurable octaves, persistence, and lacunarity
- ✅ **Performance optimizations** including caching and bulk generation
- ✅ **Image generation** (PPM, PGM, PNG support)
- ✅ **Cross-platform build system** (CMake)
- ✅ **Comprehensive configuration** management
- ✅ **Professional documentation** and examples

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## Author

**Adrian Paredez** - _Initial work_ - [GitHub](https://github.com/paredezadrian)

---

_For detailed information, please refer to the individual documentation files in this directory._
