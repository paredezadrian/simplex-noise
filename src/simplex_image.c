/**
 * @file simplex_image.c
 * @brief Image generation implementation for simplex noise
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 *
 * @details Implementation of image generation functions for creating
 *          visual representations of simplex noise patterns.
 *
 * @author Adrian Paredez
 * @version 2.0.0
 * @date 9/5/2025
 */

#include "../include/simplex_image.h"
#include "../include/simplex_noise.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== IMAGE PROCESSING CONSTANTS ===== */
#define MAX_COLOR_VALUE 255
#define NORMALIZATION_FACTOR 0.5
#define PIXEL_SCALE_FACTOR 127.5
#define TERRAIN_WATER_THRESHOLD 0.3
#define TERRAIN_WATER_R_SCALE 100
#define TERRAIN_WATER_G_SCALE 150
#define TERRAIN_WATER_B_SCALE 255
#define TERRAIN_SAND_THRESHOLD 0.5
#define TERRAIN_SAND_R_BASE 200
#define TERRAIN_SAND_R_SCALE 55
#define TERRAIN_SAND_G_BASE 180
#define TERRAIN_SAND_G_SCALE 75
#define TERRAIN_SAND_B_BASE 100
#define TERRAIN_SAND_B_SCALE 50
#define TERRAIN_GRASS_THRESHOLD 0.7
#define TERRAIN_GRASS_R_SCALE 100
#define TERRAIN_GRASS_G_BASE 100
#define TERRAIN_GRASS_G_SCALE 155
#define TERRAIN_GRASS_B_SCALE 50
#define TERRAIN_MOUNTAIN_THRESHOLD 0.9
#define TERRAIN_MOUNTAIN_R_SCALE 150
#define TERRAIN_MOUNTAIN_G_SCALE 100
#define TERRAIN_MOUNTAIN_B_SCALE 50
#define TERRAIN_ROCK_BASE 100
#define TERRAIN_ROCK_SCALE 100
#define TERRAIN_SNOW_R_SCALE 255
#define TERRAIN_SNOW_G_SCALE 255
#define TERRAIN_SNOW_B_SCALE 255

/* ===== INTERNAL FUNCTIONS ===== */

// PPM file writing functions
static int write_ppm_header(FILE* file, int width, int height, int max_color) {
    return fprintf(file, "P6\n%d %d\n%d\n", width, height, max_color);
}

static int write_pgm_header(FILE* file, int width, int height, int max_color) {
    return fprintf(file, "P5\n%d %d\n%d\n", width, height, max_color);
}

static int write_ppm_pixels(FILE* file, const uint8_t* pixels, int width, int height,
                            int channels) {
    size_t pixel_count = (size_t)width * height * channels;
    return fwrite(pixels, 1, pixel_count, file) == pixel_count ? 0 : -1;
}

static int write_pgm_pixels(FILE* file, const uint8_t* pixels, int width, int height) {
    size_t pixel_count = (size_t)width * height;
    return fwrite(pixels, 1, pixel_count, file) == pixel_count ? 0 : -1;
}

// PNG file writing (simple implementation without libpng dependency)
static int write_png_simple(FILE* file, const uint8_t* pixels, int width, int height,
                            int channels) {
    // For now, we'll write as PPM since we don't have libpng
    // In a full implementation, you'd use libpng here
    fprintf(stderr, "PNG support requires libpng. Writing as PPM instead.\n");
    return write_ppm_header(file, width, height, MAX_COLOR_VALUE) > 0 &&
                   write_ppm_pixels(file, pixels, width, height, channels) == 0
               ? 0
               : -1;
}

// Color conversion functions
static void noise_to_grayscale(double noise, uint8_t* pixel) {
    *pixel = (uint8_t)((noise + 1.0) * PIXEL_SCALE_FACTOR);
}

static void noise_to_rgb(double noise, uint8_t* r, uint8_t* g, uint8_t* b) {
    double normalized = (noise + 1.0) * NORMALIZATION_FACTOR;  // 0 to 1
    *r = (uint8_t)(normalized * MAX_COLOR_VALUE);
    *g = (uint8_t)(normalized * MAX_COLOR_VALUE);
    *b = (uint8_t)(normalized * MAX_COLOR_VALUE);
}

static void noise_to_heightmap(double noise, uint8_t* r, uint8_t* g, uint8_t* b) {
    double normalized = (noise + 1.0) * NORMALIZATION_FACTOR;  // 0 to 1

    if (normalized < TERRAIN_WATER_THRESHOLD) {
        // Water - blue
        *r = (uint8_t)(normalized * TERRAIN_WATER_R_SCALE);
        *g = (uint8_t)(normalized * TERRAIN_WATER_G_SCALE);
        *b = (uint8_t)(TERRAIN_WATER_B_SCALE);
    } else if (normalized < TERRAIN_SAND_THRESHOLD) {
        // Sand - yellow
        *r = (uint8_t)(TERRAIN_SAND_R_BASE + (normalized * TERRAIN_SAND_R_SCALE));
        *g = (uint8_t)(TERRAIN_SAND_G_BASE + (normalized * TERRAIN_SAND_G_SCALE));
        *b = (uint8_t)(TERRAIN_SAND_B_BASE + (normalized * TERRAIN_SAND_B_SCALE));
    } else if (normalized < TERRAIN_GRASS_THRESHOLD) {
        // Grass - green
        *r = (uint8_t)(normalized * TERRAIN_GRASS_R_SCALE);
        *g = (uint8_t)(TERRAIN_GRASS_G_BASE + (normalized * TERRAIN_GRASS_G_SCALE));
        *b = (uint8_t)(normalized * TERRAIN_GRASS_B_SCALE);
    } else if (normalized < TERRAIN_MOUNTAIN_THRESHOLD) {
        // Rock - gray
        *r = (uint8_t)(TERRAIN_ROCK_BASE + (normalized * TERRAIN_ROCK_SCALE));
        *g = (uint8_t)(TERRAIN_ROCK_BASE + (normalized * TERRAIN_ROCK_SCALE));
        *b = (uint8_t)(TERRAIN_ROCK_BASE + (normalized * TERRAIN_ROCK_SCALE));
    } else {
        // Snow - white
        *r = TERRAIN_SNOW_R_SCALE;
        *g = TERRAIN_SNOW_G_SCALE;
        *b = TERRAIN_SNOW_B_SCALE;
    }
}

static void noise_to_terrain(double noise, uint8_t* r, uint8_t* g, uint8_t* b) {
    double normalized = (noise + 1.0) * NORMALIZATION_FACTOR;  // 0 to 1

    if (normalized < 0.2) {
        // Deep water
        *r = 0;
        *g = 0;
        *b = (uint8_t)(100 + (normalized * 100));
    } else if (normalized < 0.4) {
        // Shallow water
        *r = 0;
        *g = (uint8_t)(50 + (normalized * 100));
        *b = (uint8_t)(150 + (normalized * 50));
    } else if (normalized < 0.6) {
        // Beach/sand
        *r = (uint8_t)(200 + (normalized * 55));
        *g = (uint8_t)(180 + (normalized * 75));
        *b = (uint8_t)(100 + (normalized * 50));
    } else if (normalized < 0.8) {
        // Forest
        *r = (uint8_t)(normalized * 50);
        *g = (uint8_t)(80 + (normalized * 120));
        *b = (uint8_t)(normalized * 30);
    } else {
        // Mountain peaks
        *r = (uint8_t)(150 + (normalized * 105));
        *g = (uint8_t)(150 + (normalized * 105));
        *b = (uint8_t)(150 + (normalized * 105));
    }
}

// Generate noise data for image
static int generate_noise_data(double* data, int width, int height,
                               const simplex_image_config_t* config, double z_offset) {
    (void)z_offset;  // Suppress unused parameter warning - used in 3D image generation
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise_x = (x + config->offset_x) * config->scale;
            double noise_y = (y + config->offset_y) * config->scale;

            double noise_value = NAN;

            if (config->octaves > 1) {
                // Fractal noise
                noise_value = simplex_fractal_2d(noise_x, noise_y, config->octaves,
                                                 config->persistence, config->lacunarity);
            } else {
                // Simple 2D noise
                noise_value = simplex_noise_2d(noise_x, noise_y);
            }

            data[(y * width) + x] = noise_value;
        }
    }

    return 0;
}

// Normalize data to 0-1 range
static void normalize_noise_data(double* data, int count, int auto_normalize) {
    if (!auto_normalize) {
        return;
    }
    double min_val = data[0];
    double max_val = data[0];

    // Find min/max
    for (int i = 1; i < count; i++) {
        if (data[i] < min_val) {
        min_val = data[i];
    }
        if (data[i] > max_val) {
        max_val = data[i];
    }
    }

    // Normalize to 0-1
    double range = max_val - min_val;
    if (range > 0) {
        for (int i = 0; i < count; i++) {
            data[i] = (data[i] - min_val) / range;
        }
    }
}

// Convert noise data to pixels
static int convert_to_pixels(const double* noise_data, uint8_t* pixels, int width, int height,
                             simplex_color_mode_t color_mode) {
    int channels =
        (color_mode == SIMPLEX_COLOR_GRAYSCALE || color_mode == SIMPLEX_COLOR_HEIGHTMAP) ? 1 : 3;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise = noise_data[y * width + x];
            int pixel_index = (y * width + x) * channels;

            switch (color_mode) {
            case SIMPLEX_COLOR_GRAYSCALE:
                noise_to_grayscale(noise, &pixels[pixel_index]);
                break;

            case SIMPLEX_COLOR_RGB:
            case SIMPLEX_COLOR_RGBA:
                noise_to_rgb(noise, &pixels[pixel_index], &pixels[pixel_index + 1],
                             &pixels[pixel_index + 2]);
                break;

            case SIMPLEX_COLOR_HEIGHTMAP:
                noise_to_heightmap(noise, &pixels[pixel_index], &pixels[pixel_index + 1],
                                   &pixels[pixel_index + 2]);
                break;

            case SIMPLEX_COLOR_TERRAIN:
                noise_to_terrain(noise, &pixels[pixel_index], &pixels[pixel_index + 1],
                                 &pixels[pixel_index + 2]);
                break;

            default:
                noise_to_grayscale(noise, &pixels[pixel_index]);
                break;
            }
        }
    }

    return 0;
}

/* ===== PUBLIC FUNCTIONS ===== */

simplex_image_config_t simplex_get_default_image_config(void) {
    simplex_image_config_t config = {0};
    config.width = 512;
    config.height = 512;
    config.format = SIMPLEX_IMAGE_PPM;
    config.color_mode = SIMPLEX_COLOR_GRAYSCALE;
    config.scale = 0.01;
    config.offset_x = 0.0;
    config.offset_y = 0.0;
    config.offset_z = 0.0;
    config.octaves = 4;
    config.persistence = 0.5;
    config.lacunarity = 2.0;
    config.min_value = -1.0;
    config.max_value = 1.0;
    config.auto_normalize = 1;
    config.seed = 12345;
    strcpy(config.filename, "simplex_noise.ppm");
    return config;
}

void simplex_set_image_size(simplex_image_config_t* config, int width, int height) {
    if (config) {
        config->width = width;
        config->height = height;
    }
}

void simplex_set_image_filename(simplex_image_config_t* config, const char* filename) {
    if (config && filename) {
        strncpy(config->filename, filename, sizeof(config->filename) - 1);
        config->filename[sizeof(config->filename) - 1] = '\0';
    }
}

void simplex_set_noise_params(simplex_image_config_t* config, double scale, int octaves,
                              double persistence, double lacunarity) {
    if (config) {
        config->scale = scale;
        config->octaves = octaves;
        config->persistence = persistence;
        config->lacunarity = lacunarity;
    }
}

void simplex_set_color_mode(simplex_image_config_t* config, simplex_color_mode_t color_mode) {
    if (config) {
        config->color_mode = color_mode;
    }
}

void simplex_normalize_data(double* data, size_t count, double min_val, double max_val) {
    if (!data || count == 0) {
        return;
    }
    double range = max_val - min_val;
    if (range > 0) {
        for (size_t i = 0; i < count; i++) {
            data[i] = (data[i] - min_val) / range;
        }
    }
}

uint8_t simplex_noise_to_grayscale(double noise_value) {
    return (uint8_t)((noise_value + 1.0) * 127.5);
}

void simplex_noise_to_rgb(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b) {
    noise_to_rgb(noise_value, r, g, b);
}

void simplex_noise_to_heightmap(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b) {
    noise_to_heightmap(noise_value, r, g, b);
}

void simplex_noise_to_terrain(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b) {
    noise_to_terrain(noise_value, r, g, b);
}

int simplex_generate_2d_image(const simplex_image_config_t* config) {
    if (!config) {
        return -1;
    }
    // Initialize noise
    simplex_noise_init(config->seed);

    // Allocate memory
    double* noise_data = malloc(config->width * config->height * sizeof(double));
    if (!noise_data) {
        return -1;
    }
    // Generate noise
    if (generate_noise_data(noise_data, config->width, config->height, config, 0.0) != 0) {
        free(noise_data);
        return -1;
    }

    // Normalize if requested
    if (config->auto_normalize) {
        normalize_noise_data(noise_data, config->width * config->height, 1);
    }

    // Convert to pixels
    int channels = (config->color_mode == SIMPLEX_COLOR_GRAYSCALE) ? 1 : 3;
    uint8_t* pixels = malloc(config->width * config->height * channels);
    if (!pixels) {
        free(noise_data);
        return -1;
    }

    convert_to_pixels(noise_data, pixels, config->width, config->height, config->color_mode);

    // Write file
    FILE* file = fopen(config->filename, "wb");
    if (!file) {
        free(noise_data);
        free(pixels);
        return -1;
    }

    int result = 0;
    switch (config->format) {
    case SIMPLEX_IMAGE_PPM:
        if (channels == 1) {
            result = write_pgm_header(file, config->width, config->height, 255) > 0 &&
                             write_pgm_pixels(file, pixels, config->width, config->height) == 0
                         ? 0
                         : -1;
        } else {
            result =
                write_ppm_header(file, config->width, config->height, 255) > 0 &&
                        write_ppm_pixels(file, pixels, config->width, config->height, channels) == 0
                    ? 0
                    : -1;
        }
        break;

    case SIMPLEX_IMAGE_PNG:
        result = write_png_simple(file, pixels, config->width, config->height, channels);
        break;

    default:
        result = -1;
        break;
    }

    fclose(file);
    free(noise_data);
    free(pixels);

    return result;
}

int simplex_generate_3d_image(const simplex_image_config_t* config, double z_slice) {
    if (!config) {
        return -1;
    }
    // Initialize noise
    simplex_noise_init(config->seed);

    // Allocate memory
    double* noise_data = malloc(config->width * config->height * sizeof(double));
    if (!noise_data) {
        return -1;
    }
    // Generate 3D noise slice
    for (int y = 0; y < config->height; y++) {
        for (int x = 0; x < config->width; x++) {
            double noise_x = (x + config->offset_x) * config->scale;
            double noise_y = (y + config->offset_y) * config->scale;
            double noise_z = (z_slice + config->offset_z) * config->scale;

            double noise_value = simplex_noise_3d(noise_x, noise_y, noise_z);
            noise_data[y * config->width + x] = noise_value;
        }
    }

    // Normalize if requested
    if (config->auto_normalize) {
        normalize_noise_data(noise_data, config->width * config->height, 1);
    }

    // Convert to pixels
    int channels = (config->color_mode == SIMPLEX_COLOR_GRAYSCALE) ? 1 : 3;
    uint8_t* pixels = malloc(config->width * config->height * channels);
    if (!pixels) {
        free(noise_data);
        return -1;
    }

    convert_to_pixels(noise_data, pixels, config->width, config->height, config->color_mode);

    // Write file
    FILE* file = fopen(config->filename, "wb");
    if (!file) {
        free(noise_data);
        free(pixels);
        return -1;
    }

    int result = 0;
    if (channels == 1) {
        result = write_pgm_header(file, config->width, config->height, 255) > 0 &&
                         write_pgm_pixels(file, pixels, config->width, config->height) == 0
                     ? 0
                     : -1;
    } else {
        result =
            write_ppm_header(file, config->width, config->height, 255) > 0 &&
                    write_ppm_pixels(file, pixels, config->width, config->height, channels) == 0
                ? 0
                : -1;
    }

    fclose(file);
    free(noise_data);
    free(pixels);

    return result;
}

int simplex_generate_fractal_image(const simplex_image_config_t* config) {
    if (!config) {
        return -1;
    }
    // Force fractal mode
    simplex_image_config_t fractal_config = *config;
    fractal_config.octaves = (fractal_config.octaves > 1) ? fractal_config.octaves : 4;

    return simplex_generate_2d_image(&fractal_config);
}

int simplex_generate_heightmap(const simplex_image_config_t* config) {
    if (!config) {
        return -1;
    }
    // Force heightmap mode
    simplex_image_config_t heightmap_config = *config;
    heightmap_config.color_mode = SIMPLEX_COLOR_HEIGHTMAP;
    heightmap_config.octaves = (heightmap_config.octaves > 1) ? heightmap_config.octaves : 6;

    return simplex_generate_2d_image(&heightmap_config);
}

int simplex_generate_texture(const simplex_image_config_t* config) {
    if (!config) {
        return -1;
    }
    // Force texture mode
    simplex_image_config_t texture_config = *config;
    texture_config.color_mode = SIMPLEX_COLOR_TERRAIN;
    texture_config.octaves = (texture_config.octaves > 1) ? texture_config.octaves : 3;

    return simplex_generate_2d_image(&texture_config);
}

int simplex_generate_image_series(const simplex_image_config_t* base_config, int count,
                                  const double* scale_variations, const uint32_t* seed_variations) {
    if (!base_config || count <= 0) {
        return -1;
    }
    for (int i = 0; i < count; i++) {
        simplex_image_config_t config = *base_config;

        // Modify filename to include index
        char filename[256];
        snprintf(filename, sizeof(filename), "simplex_series_%d.ppm", i);
        simplex_set_image_filename(&config, filename);

        // Apply variations
        if (scale_variations) {
            config.scale = scale_variations[i];
        }
        if (seed_variations) {
            config.seed = seed_variations[i];
        }

        if (simplex_generate_2d_image(&config) != 0) {
            return -1;
        }
    }

    return 0;
}

int simplex_generate_animation(const simplex_image_config_t* config, int frame_count,
                               double time_step, const char* output_dir) {
    if (!config || frame_count <= 0 || !output_dir) {
        return -1;
    }
    for (int frame = 0; frame < frame_count; frame++) {
        simplex_image_config_t frame_config = *config;

        // Modify filename for frame
        char filename[256];
        snprintf(filename, sizeof(filename), "%s/frame_%04d.ppm", output_dir, frame);
        simplex_set_image_filename(&frame_config, filename);

        // Add time offset
        frame_config.offset_z = frame * time_step;

        if (simplex_generate_3d_image(&frame_config, frame * time_step) != 0) {
            return -1;
        }
    }

    return 0;
}
