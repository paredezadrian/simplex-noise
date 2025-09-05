# Examples

This document provides practical examples demonstrating various uses of the Simplex Noise Library.

## Basic Examples

### Simple 2D Noise

```c
#include <stdio.h>
#include "simplex_noise.h"

int main() {
    simplex_noise_init(12345);

    // Generate a 10x10 grid of noise
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            double noise = simplex_noise_2d(x * 0.1, y * 0.1);
            printf("%6.3f ", noise);
        }
        printf("\n");
    }

    return 0;
}
```

### 3D Noise Slices

```c
#include <stdio.h>
#include "simplex_noise.h"

int main() {
    simplex_noise_init(12345);

    // Generate 3D noise at different Z levels
    double z_levels[] = {0.0, 0.5, 1.0, 1.5, 2.0};

    for (int z_idx = 0; z_idx < 5; z_idx++) {
        printf("Z = %.1f:\n", z_levels[z_idx]);
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                double noise = simplex_noise_3d(x * 0.2, y * 0.2, z_levels[z_idx]);
                printf("%6.3f ", noise);
            }
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}
```

## Fractal Noise Examples

### Terrain Generation

```c
#include <stdio.h>
#include <stdlib.h>
#include "simplex_noise.h"

void generate_terrain(int width, int height) {
    double* terrain = malloc(width * height * sizeof(double));

    // Generate fractal noise for terrain
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise = simplex_fractal_2d(x * 0.01, y * 0.01, 6, 0.5, 2.0);
            terrain[y * width + x] = noise;
        }
    }

    // Convert to height values (0-100)
    printf("Terrain Heightmap:\n");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int height_val = (int)((terrain[y * width + x] + 1.0) * 50.0);
            printf("%3d ", height_val);
        }
        printf("\n");
    }

    free(terrain);
}

int main() {
    simplex_noise_init(12345);
    generate_terrain(20, 20);
    return 0;
}
```

### Cloud Generation

```c
#include <stdio.h>
#include <stdlib.h>
#include "simplex_noise.h"

void generate_clouds(int width, int height) {
    double* clouds = malloc(width * height * sizeof(double));

    // Generate multiple octaves for cloud-like appearance
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise = 0.0;
            double amplitude = 1.0;
            double frequency = 0.02;

            // Add multiple octaves
            for (int i = 0; i < 4; i++) {
                noise += simplex_noise_2d(x * frequency, y * frequency) * amplitude;
                amplitude *= 0.5;
                frequency *= 2.0;
            }

            clouds[y * width + x] = noise;
        }
    }

    // Display cloud density
    printf("Cloud Density:\n");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char cloud_char = clouds[y * width + x] > 0.2 ? '#' : ' ';
            printf("%c", cloud_char);
        }
        printf("\n");
    }

    free(clouds);
}

int main() {
    simplex_noise_init(12345);
    generate_clouds(40, 20);
    return 0;
}
```

## Advanced Examples

### Biome Generation

```c
#include <stdio.h>
#include <stdlib.h>
#include "simplex_noise.h"

typedef enum {
    BIOME_OCEAN,
    BIOME_DESERT,
    BIOME_GRASSLAND,
    BIOME_FOREST,
    BIOME_TUNDRA,
    BIOME_MOUNTAIN
} biome_type_t;

biome_type_t get_biome(double x, double y) {
    // Temperature and humidity noise
    double temperature = simplex_noise_2d(x * 0.01, y * 0.01);
    double humidity = simplex_noise_2d(x * 0.01 + 1000, y * 0.01 + 1000);

    // Elevation noise
    double elevation = simplex_fractal_2d(x * 0.005, y * 0.005, 4, 0.5, 2.0);

    // Determine biome based on conditions
    if (elevation < -0.3) {
        return BIOME_OCEAN;
    } else if (elevation > 0.7) {
        return BIOME_MOUNTAIN;
    } else if (temperature < -0.2) {
        return BIOME_TUNDRA;
    } else if (temperature > 0.3 && humidity > 0.2) {
        return BIOME_FOREST;
    } else if (temperature > 0.1 && humidity > -0.1) {
        return BIOME_GRASSLAND;
    } else {
        return BIOME_DESERT;
    }
}

void generate_world_map(int width, int height) {
    const char* biome_chars = "ODGFTM";  // Ocean, Desert, Grassland, Forest, Tundra, Mountain

    printf("World Map:\n");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            biome_type_t biome = get_biome(x, y);
            printf("%c", biome_chars[biome]);
        }
        printf("\n");
    }
}

int main() {
    simplex_noise_init(12345);
    generate_world_map(60, 30);
    return 0;
}
```

### Cave System Generation

```c
#include <stdio.h>
#include <stdlib.h>
#include "simplex_noise.h"

void generate_cave_system(int width, int height, int depth) {
    printf("Cave System (Z slices):\n");

    for (int z = 0; z < depth; z++) {
        printf("\nZ = %d:\n", z);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // 3D noise for cave generation
                double cave_noise = simplex_noise_3d(x * 0.1, y * 0.1, z * 0.1);

                // Add some fractal noise for more complex caves
                double fractal_noise = simplex_fractal_3d(x * 0.05, y * 0.05, z * 0.05, 3, 0.5, 2.0);

                double combined = cave_noise + fractal_noise * 0.3;

                if (combined > 0.1) {
                    printf(" ");  // Air (cave)
                } else {
                    printf("#");  // Solid rock
                }
            }
            printf("\n");
        }
    }
}

int main() {
    simplex_noise_init(12345);
    generate_cave_system(20, 20, 5);
    return 0;
}
```

## Image Generation Examples

### Heightmap Generation

```c
#include "simplex_image.h"

int main() {
    // Create heightmap configuration
    simplex_image_config_t config = simplex_get_default_image_config();

    // Set properties
    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "heightmap.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);
    simplex_set_noise_params(&config, 0.01, 6, 0.5, 2.0);

    // Generate heightmap
    if (simplex_generate_2d_image(&config) == 0) {
        printf("Heightmap generated: heightmap.pgm\n");
    }

    return 0;
}
```

### Terrain Visualization

```c
#include "simplex_image.h"

int main() {
    // Create terrain configuration
    simplex_image_config_t config = simplex_get_default_image_config();

    // Set properties
    simplex_set_image_size(&config, 1024, 1024);
    simplex_set_image_filename(&config, "terrain.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_TERRAIN);
    simplex_set_noise_params(&config, 0.005, 8, 0.6, 2.0);

    // Generate terrain
    if (simplex_generate_2d_image(&config) == 0) {
        printf("Terrain generated: terrain.ppm\n");
    }

    return 0;
}
```

### Animation Generation

```c
#include "simplex_image.h"

int main() {
    // Create base configuration
    simplex_image_config_t config = simplex_get_default_image_config();
    simplex_set_image_size(&config, 256, 256);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_HEIGHTMAP);
    simplex_set_noise_params(&config, 0.02, 4, 0.5, 2.0);

    // Generate animation frames
    if (simplex_generate_animation(&config, 60, 0.1, "frames") == 0) {
        printf("Animation frames generated in 'frames' directory\n");
    }

    return 0;
}
```

## Performance Examples

### Bulk Generation

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simplex_noise.h"

void benchmark_bulk_generation() {
    const int width = 1000;
    const int height = 1000;
    double* noise_data = malloc(width * height * sizeof(double));

    simplex_noise_init(12345);

    clock_t start = clock();

    // Generate noise in bulk
    simplex_noise_array_2d(0, 0, width, height, 0.01, noise_data);

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Generated %d x %d noise samples in %.3f seconds\n",
           width, height, time_taken);
    printf("Performance: %.0f samples/second\n",
           (width * height) / time_taken);

    free(noise_data);
}

int main() {
    benchmark_bulk_generation();
    return 0;
}
```

### Caching Performance

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simplex_image.h"

void test_caching_performance() {
    simplex_config_t config = simplex_get_default_config();
    config.enable_caching = 1;
    config.enable_profiling = 1;

    simplex_noise_init_advanced(&config);

    // Generate same noise multiple times
    for (int i = 0; i < 1000; i++) {
        simplex_noise_2d(1.0, 2.0);
    }

    // Check performance stats
    simplex_perf_stats_t stats;
    simplex_get_performance_stats(&stats);

    printf("Function calls: %zu\n", stats.function_calls);
    printf("Cache hits: %zu\n", stats.cache_hits);
    printf("Cache misses: %zu\n", stats.cache_misses);
    printf("Hit rate: %.1f%%\n",
           (double)stats.cache_hits / stats.function_calls * 100.0);
}

int main() {
    test_caching_performance();
    return 0;
}
```

## Complete Application Example

### Simple Terrain Generator

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplex_noise.h"
#include "simplex_image.h"

typedef struct {
    int width;
    int height;
    double* heightmap;
    char* biome_map;
} world_t;

world_t* create_world(int width, int height) {
    world_t* world = malloc(sizeof(world_t));
    world->width = width;
    world->height = height;
    world->heightmap = malloc(width * height * sizeof(double));
    world->biome_map = malloc(width * height * sizeof(char));
    return world;
}

void destroy_world(world_t* world) {
    free(world->heightmap);
    free(world->biome_map);
    free(world);
}

void generate_heightmap(world_t* world) {
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            double noise = simplex_fractal_2d(x * 0.01, y * 0.01, 6, 0.5, 2.0);
            world->heightmap[y * world->width + x] = noise;
        }
    }
}

void generate_biomes(world_t* world) {
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            double height = world->heightmap[y * world->width + x];
            double temperature = simplex_noise_2d(x * 0.005, y * 0.005);
            double humidity = simplex_noise_2d(x * 0.005 + 1000, y * 0.005 + 1000);

            char biome;
            if (height < -0.3) {
                biome = 'O';  // Ocean
            } else if (height > 0.7) {
                biome = 'M';  // Mountain
            } else if (temperature < -0.2) {
                biome = 'T';  // Tundra
            } else if (temperature > 0.3 && humidity > 0.2) {
                biome = 'F';  // Forest
            } else if (temperature > 0.1 && humidity > -0.1) {
                biome = 'G';  // Grassland
            } else {
                biome = 'D';  // Desert
            }

            world->biome_map[y * world->width + x] = biome;
        }
    }
}

void save_world_image(world_t* world, const char* filename) {
    simplex_image_config_t config = simplex_get_default_image_config();
    simplex_set_image_size(&config, world->width, world->height);
    simplex_set_image_filename(&config, filename);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_HEIGHTMAP);
    simplex_set_noise_params(&config, 0.01, 6, 0.5, 2.0);

    simplex_generate_2d_image(&config);
}

void print_world(world_t* world) {
    printf("World Map (%dx%d):\n", world->width, world->height);
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            printf("%c", world->biome_map[y * world->width + x]);
        }
        printf("\n");
    }
}

int main() {
    // Initialize noise
    simplex_noise_init(12345);

    // Create world
    world_t* world = create_world(80, 40);

    // Generate world
    printf("Generating heightmap...\n");
    generate_heightmap(world);

    printf("Generating biomes...\n");
    generate_biomes(world);

    // Display world
    print_world(world);

    // Save image
    printf("Saving world image...\n");
    save_world_image(world, "world.ppm");

    // Cleanup
    destroy_world(world);
    simplex_cleanup();

    return 0;
}
```

---

_These examples demonstrate the versatility and power of the Simplex Noise Library. Experiment with different parameters and combinations to create your own procedural content!_
