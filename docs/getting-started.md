# Getting Started

This guide will help you get up and running with the Simplex Noise Library quickly.

## Prerequisites

- C99 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10 or later (for building)
- Make or Ninja (for compilation)

## Installation

### Building from Source

1. **Clone the repository:**

```bash
git clone https://github.com/paredezadrian/simplex-noise.git
cd simplex-noise
```

2. **Create build directory:**

```bash
mkdir build
cd build
```

3. **Configure with CMake:**

```bash
cmake ..
```

4. **Build the library:**

```bash
make
```

5. **Install (optional):**

```bash
sudo make install
```

### Quick Build

```bash
mkdir build && cd build
cmake .. && make
```

## Basic Usage

### Simple Example

Create a file called `hello_noise.c`:

```c
#include <stdio.h>
#include "simplex_noise.h"

int main() {
    // Initialize with seed
    simplex_noise_init(12345);

    // Generate some 2D noise
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            double noise = simplex_noise_2d(x * 0.1, y * 0.1);
            printf("%.3f ", noise);
        }
        printf("\n");
    }

    return 0;
}
```

### Compile and Run

```bash
gcc -o hello_noise hello_noise.c -lsimplex_noise -lm
./hello_noise
```

## Advanced Example

```c
#include <stdio.h>
#include <stdlib.h>
#include "simplex_noise.h"

int main() {
    // Create advanced configuration
    simplex_config_t config = simplex_get_default_config();
    config.octaves = 6;
    config.persistence = 0.6;
    config.lacunarity = 2.0;
    config.prng_type = SIMPLEX_PRG_PCG;

    // Initialize with configuration
    if (simplex_noise_init_advanced(&config) != 0) {
        fprintf(stderr, "Failed to initialize noise\n");
        return 1;
    }

    // Generate fractal noise
    double fractal = simplex_fractal_2d(1.0, 2.0, 4, 0.5, 2.0);
    printf("Fractal noise: %.6f\n", fractal);

    // Generate ridged noise
    double ridged = simplex_ridged_2d(1.0, 2.0);
    printf("Ridged noise: %.6f\n", ridged);

    // Cleanup
    simplex_cleanup();

    return 0;
}
```

## Image Generation Example

```c
#include "simplex_image.h"

int main() {
    // Create image configuration
    simplex_image_config_t img_config = simplex_get_default_image_config();

    // Set image properties
    simplex_set_image_size(&img_config, 512, 512);
    simplex_set_image_filename(&img_config, "terrain.ppm");
    simplex_set_color_mode(&img_config, SIMPLEX_COLOR_HEIGHTMAP);
    simplex_set_noise_params(&img_config, 0.01, 6, 0.5, 2.0);

    // Generate terrain image
    if (simplex_generate_2d_image(&img_config) == 0) {
        printf("Terrain image generated successfully!\n");
    } else {
        printf("Failed to generate image\n");
    }

    return 0;
}
```

## Common Patterns

### Terrain Generation

```c
// Generate heightmap for terrain
double* heightmap = malloc(width * height * sizeof(double));
simplex_noise_array_2d(0, 0, width, height, 0.1, heightmap);

// Convert to block heights
for (int i = 0; i < width * height; i++) {
    int block_height = (int)((heightmap[i] + 1.0) * 64.0);
    // Place blocks up to block_height
}
```

### Biome Generation

```c
// Different noise for different biomes
double temperature = simplex_noise_2d(x * 0.01, y * 0.01);
double humidity = simplex_noise_2d(x * 0.01 + 1000, y * 0.01 + 1000);

if (temperature > 0.3 && humidity > 0.5) {
    // Forest biome
} else if (temperature > 0.1) {
    // Grassland biome
} else {
    // Desert biome
}
```

### Cave Generation

```c
// 3D noise for cave systems
double cave_noise = simplex_noise_3d(x, y, z);
if (cave_noise > 0.2) {
    // Place air block (cave)
} else {
    // Place solid block
}
```

## Configuration Options

### PRNG Selection

```c
simplex_config_t config = simplex_get_default_config();

// Choose PRNG algorithm
config.prng_type = SIMPLEX_PRG_PCG;  // Best quality
// config.prng_type = SIMPLEX_PRG_MERSENNE_TWISTER;  // High quality
// config.prng_type = SIMPLEX_PRG_LINEAR_CONGRUENTIAL;  // Fastest
```

### Noise Variants

```c
// Different noise types
config.noise_variant = SIMPLEX_NOISE_CLASSIC;  // Standard simplex
// config.noise_variant = SIMPLEX_NOISE_RIDGED;  // Ridged patterns
// config.noise_variant = SIMPLEX_NOISE_BILLOWY;  // Billowy patterns
```

### Performance Tuning

```c
// Enable caching for repeated calls
config.enable_caching = 1;

// Enable profiling to monitor performance
config.enable_profiling = 1;

// Set memory limits
config.memory_limit_mb = 256.0;
```

## Troubleshooting

### Common Issues

1. **Compilation errors:**

   - Ensure you're using a C99 compatible compiler
   - Check that all required headers are included
   - Verify library linking with `-lsimplex_noise -lm`

2. **Runtime errors:**

   - Always initialize the library before use
   - Check return values from functions
   - Ensure proper memory allocation

3. **Performance issues:**
   - Use bulk generation functions for large arrays
   - Enable caching for repeated coordinates
   - Consider using faster PRNG algorithms

### Debug Tips

```c
// Enable profiling to see performance stats
simplex_config_t config = simplex_get_default_config();
config.enable_profiling = 1;
simplex_noise_init_advanced(&config);

// ... use noise functions ...

// Check performance
simplex_perf_stats_t stats;
simplex_get_performance_stats(&stats);
printf("Function calls: %zu\n", stats.function_calls);
printf("Cache hits: %zu\n", stats.cache_hits);
```

## Next Steps

- Explore [Advanced Usage](advanced-usage.md) for complex scenarios
- Check [Examples](examples.md) for more code samples
- Read [Performance Guide](performance.md) for optimization tips
- See [Image Generation](image-generation.md) for visualization tools

---

_Ready to dive deeper? Check out the [API Reference](api-reference.md) for complete function documentation._
