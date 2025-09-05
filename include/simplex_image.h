/**
 * @file simplex_image.h
 * @brief Image generation utilities for simplex noise
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 *
 * @details This header provides functions for generating images from simplex noise,
 *          including PNG and PPM format support. Perfect for creating heightmaps,
 *          textures, and visualizations of noise patterns.
 *
 * @author Adrian Paredez
 * @version 2.0.0
 * @date 9/5/2025
 */

#ifndef SIMPLEX_IMAGE_H
#define SIMPLEX_IMAGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== IMAGE FORMATS ===== */

/**
 * @brief Supported image formats
 */
typedef enum {
    SIMPLEX_IMAGE_PNG = 0, /**< PNG format (requires libpng) */
    SIMPLEX_IMAGE_PPM,     /**< PPM format (portable pixmap) */
    SIMPLEX_IMAGE_PGM,     /**< PGM format (portable graymap) */
    SIMPLEX_IMAGE_RAW,     /**< Raw binary data */
    SIMPLEX_IMAGE_COUNT
} simplex_image_format_t;

/**
 * @brief Color modes for image generation
 */
typedef enum {
    SIMPLEX_COLOR_GRAYSCALE = 0, /**< Grayscale (single channel) */
    SIMPLEX_COLOR_RGB,           /**< RGB color (3 channels) */
    SIMPLEX_COLOR_RGBA,          /**< RGBA color (4 channels) */
    SIMPLEX_COLOR_HEIGHTMAP,     /**< Heightmap visualization */
    SIMPLEX_COLOR_TERRAIN,       /**< Terrain-like colors */
    SIMPLEX_COLOR_COUNT
} simplex_color_mode_t;

/**
 * @brief Image generation configuration
 */
typedef struct {
    int width;                       /**< Image width in pixels */
    int height;                      /**< Image height in pixels */
    simplex_image_format_t format;   /**< Output image format */
    simplex_color_mode_t color_mode; /**< Color mode */
    double scale;                    /**< Noise scale factor */
    double offset_x;                 /**< X offset for noise sampling */
    double offset_y;                 /**< Y offset for noise sampling */
    double offset_z;                 /**< Z offset for 3D noise */
    int octaves;                     /**< Number of octaves for fractal noise */
    double persistence;              /**< Persistence for fractal noise */
    double lacunarity;               /**< Lacunarity for fractal noise */
    double min_value;                /**< Minimum noise value for normalization */
    double max_value;                /**< Maximum noise value for normalization */
    int auto_normalize;              /**< Auto-normalize noise values to 0-1 */
    uint32_t seed;                   /**< Random seed for noise generation */
    char filename[256];              /**< Output filename */  // NOLINT(readability-magic-numbers)
} simplex_image_config_t;

/* ===== CORE IMAGE FUNCTIONS ===== */

/**
 * @brief Generate a 2D noise image
 * @param config Image generation configuration
 * @return 0 on success, -1 on error
 */
int simplex_generate_2d_image(const simplex_image_config_t* config);

/**
 * @brief Generate a 3D noise image (slice)
 * @param config Image generation configuration
 * @param z_slice Z coordinate for the slice
 * @return 0 on success, -1 on error
 */
int simplex_generate_3d_image(const simplex_image_config_t* config, double z_slice);

/**
 * @brief Generate a fractal noise image
 * @param config Image generation configuration
 * @return 0 on success, -1 on error
 */
int simplex_generate_fractal_image(const simplex_image_config_t* config);

/**
 * @brief Generate a heightmap image
 * @param config Image generation configuration
 * @return 0 on success, -1 on error
 */
int simplex_generate_heightmap(const simplex_image_config_t* config);

/**
 * @brief Generate a texture image
 * @param config Image generation configuration
 * @return 0 on success, -1 on error
 */
int simplex_generate_texture(const simplex_image_config_t* config);

/* ===== CONFIGURATION FUNCTIONS ===== */

/**
 * @brief Get default image configuration
 * @return Default configuration structure
 */
simplex_image_config_t simplex_get_default_image_config(void);

/**
 * @brief Set image dimensions
 * @param config Configuration to modify
 * @param width Image width
 * @param height Image height
 */
void simplex_set_image_size(simplex_image_config_t* config, int width, int height);

/**
 * @brief Set output filename
 * @param config Configuration to modify
 * @param filename Output filename
 */
void simplex_set_image_filename(simplex_image_config_t* config, const char* filename);

/**
 * @brief Set noise parameters
 * @param config Configuration to modify
 * @param scale Noise scale factor
 * @param octaves Number of octaves
 * @param persistence Persistence value
 * @param lacunarity Lacunarity value
 */
void simplex_set_noise_params(simplex_image_config_t* config, double scale, int octaves,
                              double persistence, double lacunarity);

/**
 * @brief Set color mode
 * @param config Configuration to modify
 * @param color_mode Color mode to use
 */
void simplex_set_color_mode(simplex_image_config_t* config, simplex_color_mode_t color_mode);

/* ===== UTILITY FUNCTIONS ===== */

/**
 * @brief Normalize noise values to 0-1 range
 * @param data Noise data array
 * @param count Number of elements
 * @param min_val Minimum value in data
 * @param max_val Maximum value in data
 */
void simplex_normalize_data(double* data, size_t count, double min_val, double max_val);

/**
 * @brief Convert noise value to grayscale pixel
 * @param noise_value Noise value (-1 to 1)
 * @return Grayscale pixel value (0-255)
 */
uint8_t simplex_noise_to_grayscale(double noise_value);

/**
 * @brief Convert noise value to RGB pixel
 * @param noise_value Noise value (-1 to 1)
 * @param r Red component output
 * @param g Green component output
 * @param b Blue component output
 */
void simplex_noise_to_rgb(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b);

/**
 * @brief Convert noise value to heightmap color
 * @param noise_value Noise value (-1 to 1)
 * @param r Red component output
 * @param g Green component output
 * @param b Blue component output
 */
void simplex_noise_to_heightmap(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b);

/**
 * @brief Convert noise value to terrain color
 * @param noise_value Noise value (-1 to 1)
 * @param r Red component output
 * @param g Green component output
 * @param b Blue component output
 */
void simplex_noise_to_terrain(double noise_value, uint8_t* r, uint8_t* g, uint8_t* b);

/* ===== BATCH GENERATION ===== */

/**
 * @brief Generate multiple images with different parameters
 * @param base_config Base configuration
 * @param count Number of images to generate
 * @param scale_variations Array of scale variations
 * @param seed_variations Array of seed variations
 * @return 0 on success, -1 on error
 */
int simplex_generate_image_series(const simplex_image_config_t* base_config, int count,
                                  const double* scale_variations, const uint32_t* seed_variations);

/**
 * @brief Generate animation frames
 * @param config Base configuration
 * @param frame_count Number of frames
 * @param time_step Time step between frames
 * @param output_dir Output directory for frames
 * @return 0 on success, -1 on error
 */
int simplex_generate_animation(const simplex_image_config_t* config, int frame_count,
                               double time_step, const char* output_dir);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEX_IMAGE_H */
