# Image Generation Guide

This guide covers the image generation capabilities of the Simplex Noise Library, including creating heightmaps, textures, and visualizations.

## Overview

The library provides comprehensive image generation capabilities through the `simplex_image.h` header, supporting multiple formats and color modes for various applications.

## Supported Formats

### Image Formats

```c
typedef enum {
    SIMPLEX_IMAGE_PNG,    // PNG format (requires libpng)
    SIMPLEX_IMAGE_PPM,    // PPM format (portable pixmap)
    SIMPLEX_IMAGE_PGM,    // PGM format (portable graymap)
    SIMPLEX_IMAGE_RAW     // Raw binary data
} simplex_image_format_t;
```

### Color Modes

```c
typedef enum {
    SIMPLEX_COLOR_GRAYSCALE,  // Grayscale (single channel)
    SIMPLEX_COLOR_RGB,        // RGB color (3 channels)
    SIMPLEX_COLOR_RGBA,       // RGBA color (4 channels)
    SIMPLEX_COLOR_HEIGHTMAP,  // Heightmap visualization
    SIMPLEX_COLOR_TERRAIN     // Terrain-like colors
} simplex_color_mode_t;
```

## Basic Image Generation

### Simple 2D Image

```c
#include "simplex_image.h"

int main() {
    // Create default configuration
    simplex_image_config_t config = simplex_get_default_image_config();

    // Set image properties
    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "noise.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);

    // Generate image
    if (simplex_generate_2d_image(&config) == 0) {
        printf("Image generated successfully!\n");
    }

    return 0;
}
```

### 3D Noise Slice

```c
int main() {
    simplex_image_config_t config = simplex_get_default_image_config();

    simplex_set_image_size(&config, 256, 256);
    simplex_set_image_filename(&config, "3d_slice.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);

    // Generate 3D noise at Z=0.5
    if (simplex_generate_3d_image(&config, 0.5) == 0) {
        printf("3D slice generated successfully!\n");
    }

    return 0;
}
```

## Fractal Images

### Terrain Generation

```c
int main() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Set terrain properties
    simplex_set_image_size(&config, 1024, 1024);
    simplex_set_image_filename(&config, "terrain.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_HEIGHTMAP);
    simplex_set_noise_params(&config, 0.005, 8, 0.6, 2.0);

    // Generate fractal terrain
    if (simplex_generate_fractal_image(&config) == 0) {
        printf("Terrain generated successfully!\n");
    }

    return 0;
}
```

### Cloud Generation

```c
int main() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Set cloud properties
    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "clouds.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);
    simplex_set_noise_params(&config, 0.02, 4, 0.5, 2.0);

    // Generate clouds
    if (simplex_generate_2d_image(&config) == 0) {
        printf("Clouds generated successfully!\n");
    }

    return 0;
}
```

## Color Modes

### Grayscale Images

```c
void generate_grayscale_image() {
    simplex_image_config_t config = simplex_get_default_image_config();

    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "grayscale.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);

    simplex_generate_2d_image(&config);
}
```

### RGB Images

```c
void generate_rgb_image() {
    simplex_image_config_t config = simplex_get_default_image_config();

    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "rgb.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_RGB);

    simplex_generate_2d_image(&config);
}
```

### Heightmap Visualization

```c
void generate_heightmap() {
    simplex_image_config_t config = simplex_get_default_image_config();

    simplex_set_image_size(&config, 1024, 1024);
    simplex_set_image_filename(&config, "heightmap.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_HEIGHTMAP);
    simplex_set_noise_params(&config, 0.01, 6, 0.5, 2.0);

    simplex_generate_2d_image(&config);
}
```

### Terrain Visualization

```c
void generate_terrain() {
    simplex_image_config_t config = simplex_get_default_image_config();

    simplex_set_image_size(&config, 1024, 1024);
    simplex_set_image_filename(&config, "terrain.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_TERRAIN);
    simplex_set_noise_params(&config, 0.005, 8, 0.6, 2.0);

    simplex_generate_2d_image(&config);
}
```

## Advanced Image Generation

### Custom Color Functions

```c
void custom_color_function(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b) {
    double normalized = (noise_value + 1.0) * 0.5;  // 0 to 1

    if (normalized < 0.3) {
        // Water - blue
        *r = 0;
        *g = (uint8_t)(normalized * 100);
        *b = (uint8_t)(150 + normalized * 105);
    } else if (normalized < 0.6) {
        // Land - green
        *r = (uint8_t)(normalized * 50);
        *g = (uint8_t)(100 + normalized * 155);
        *b = (uint8_t)(normalized * 50);
    } else {
        // Mountains - gray
        *r = (uint8_t)(100 + normalized * 155);
        *g = (uint8_t)(100 + normalized * 155);
        *b = (uint8_t)(100 + normalized * 155);
    }
}
```

### Batch Image Generation

```c
void generate_image_series() {
    simplex_image_config_t base_config = simplex_get_default_image_config();

    // Set base properties
    simplex_set_image_size(&base_config, 256, 256);
    simplex_set_color_mode(&base_config, SIMPLEX_COLOR_HEIGHTMAP);

    // Generate series with different scales
    double scales[] = {0.01, 0.02, 0.05, 0.1};
    uint32_t seeds[] = {12345, 54321, 98765, 13579};

    if (simplex_generate_image_series(&base_config, 4, scales, seeds) == 0) {
        printf("Image series generated successfully!\n");
    }
}
```

### Animation Generation

```c
void generate_animation() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Set animation properties
    simplex_set_image_size(&config, 512, 512);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_HEIGHTMAP);
    simplex_set_noise_params(&config, 0.01, 4, 0.5, 2.0);

    // Generate 60 frames
    if (simplex_generate_animation(&config, 60, 0.1, "frames") == 0) {
        printf("Animation frames generated successfully!\n");
    }
}
```

## Image Processing Utilities

### Data Normalization

```c
void normalize_noise_data(double* data, size_t count) {
    // Find min/max values
    double min_val = data[0];
    double max_val = data[0];

    for (size_t i = 1; i < count; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }

    // Normalize to 0-1 range
    double range = max_val - min_val;
    if (range > 0) {
        for (size_t i = 0; i < count; i++) {
            data[i] = (data[i] - min_val) / range;
        }
    }
}
```

### Color Conversion

```c
// Convert noise value to grayscale
uint8_t noise_to_grayscale(double noise_value) {
    return (uint8_t)((noise_value + 1.0) * 127.5);
}

// Convert noise value to RGB
void noise_to_rgb(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b) {
    double normalized = (noise_value + 1.0) * 0.5;
    *r = (uint8_t)(normalized * 255);
    *g = (uint8_t)(normalized * 255);
    *b = (uint8_t)(normalized * 255);
}
```

## Specialized Image Types

### Heightmap Generation

```c
void generate_heightmap() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Configure for heightmap
    simplex_set_image_size(&config, 1024, 1024);
    simplex_set_image_filename(&config, "heightmap.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);
    simplex_set_noise_params(&config, 0.01, 6, 0.5, 2.0);

    // Generate heightmap
    if (simplex_generate_heightmap(&config) == 0) {
        printf("Heightmap generated successfully!\n");
    }
}
```

### Texture Generation

```c
void generate_texture() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Configure for texture
    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "texture.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_TERRAIN);
    simplex_set_noise_params(&config, 0.02, 3, 0.5, 2.0);

    // Generate texture
    if (simplex_generate_texture(&config) == 0) {
        printf("Texture generated successfully!\n");
    }
}
```

## Configuration Examples

### High-Resolution Terrain

```c
void generate_high_res_terrain() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // High resolution
    simplex_set_image_size(&config, 2048, 2048);
    simplex_set_image_filename(&config, "terrain_4k.ppm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_TERRAIN);

    // High detail noise
    simplex_set_noise_params(&config, 0.002, 10, 0.6, 2.0);

    simplex_generate_2d_image(&config);
}
```

### Low-Resolution Preview

```c
void generate_preview() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Low resolution for preview
    simplex_set_image_size(&config, 128, 128);
    simplex_set_image_filename(&config, "preview.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);

    // Simple noise for speed
    simplex_set_noise_params(&config, 0.05, 2, 0.5, 2.0);

    simplex_generate_2d_image(&config);
}
```

### Scientific Visualization

```c
void generate_scientific_plot() {
    simplex_image_config_t config = simplex_get_default_image_config();

    // Square aspect ratio
    simplex_set_image_size(&config, 512, 512);
    simplex_set_image_filename(&config, "scientific.pgm");
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);

    // High precision noise
    simplex_set_noise_params(&config, 0.01, 8, 0.5, 2.0);

    simplex_generate_2d_image(&config);
}
```

## File Format Details

### PPM Format (Portable Pixmap)

- **Header**: `P6\n<width> <height>\n<max_color>\n`
- **Data**: Binary RGB values (3 bytes per pixel)
- **Use case**: Color images, terrain visualization

### PGM Format (Portable Graymap)

- **Header**: `P5\n<width> <height>\n<max_color>\n`
- **Data**: Binary grayscale values (1 byte per pixel)
- **Use case**: Heightmaps, scientific visualization

### PNG Format (Portable Network Graphics)

- **Requirements**: libpng library
- **Use case**: Web display, high-quality images
- **Note**: Currently falls back to PPM format

## Performance Considerations

### Memory Usage

```c
// For large images, consider memory usage
simplex_image_config_t config = simplex_get_default_image_config();
simplex_set_image_size(&config, 4096, 4096);  // 16MP image

// This will use approximately:
// - 64MB for noise data (double precision)
// - 48MB for RGB pixels (8-bit)
// - Total: ~112MB memory
```

### Batch Processing

```c
// Process multiple images efficiently
void process_image_batch() {
    simplex_image_config_t base_config = simplex_get_default_image_config();

    // Set common properties
    simplex_set_image_size(&base_config, 512, 512);
    simplex_set_color_mode(&base_config, SIMPLEX_COLOR_HEIGHTMAP);

    // Generate multiple variations
    for (int i = 0; i < 10; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "batch_%d.ppm", i);
        simplex_set_image_filename(&base_config, filename);

        // Vary noise parameters
        simplex_set_noise_params(&base_config, 0.01 + i * 0.005, 4, 0.5, 2.0);

        simplex_generate_2d_image(&base_config);
    }
}
```

## Troubleshooting

### Common Issues

1. **File not found**: Check file path and permissions
2. **Memory allocation failed**: Reduce image size or increase memory limits
3. **Invalid format**: Ensure format is supported and properly configured
4. **Poor quality**: Adjust noise parameters or use higher resolution

### Debug Tips

```c
// Enable verbose mode for debugging
simplex_image_config_t config = simplex_get_default_image_config();
config.verbose_mode = 1;

// Check return values
if (simplex_generate_2d_image(&config) != 0) {
    printf("Error: Failed to generate image\n");
    // Handle error
}
```

---

_For more advanced image generation techniques, see [Examples](examples.md)_
