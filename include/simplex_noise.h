/**
 * @file simplex_noise.h
 * @brief Advanced Pure C implementation of Simplex Noise algorithm
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 *
 * @details This implementation is based on Ken Perlin's improved noise algorithm
 * and provides 1D, 2D, 3D, and 4D simplex noise generation with fractal support.
 * Features multiple PRNG algorithms, advanced interpolation methods, performance
 * optimizations, and comprehensive noise variants for professional applications.
 *
 * @author Adrian Paredez
 * @version 2.0
 * @date 9/5/2025
 */

#ifndef SIMPLEX_NOISE_H
#define SIMPLEX_NOISE_H

#include <stddef.h>
#include <stdint.h>

/* ===== CONSTANTS ===== */
static const size_t SIMPLEX_MAX_ERROR_COUNT = 10;

/* PRNG Algorithm Types */
typedef enum {
    SIMPLEX_PRG_LINEAR_CONGRUENTIAL = 0,
    SIMPLEX_PRG_MERSENNE_TWISTER,
    SIMPLEX_PRG_XORSHIFT,
    SIMPLEX_PRG_PCG,
    SIMPLEX_PRG_CUSTOM,
    SIMPLEX_PRG_COUNT
} simplex_prng_type_t;

/* Noise Variant Types */
typedef enum {
    SIMPLEX_NOISE_CLASSIC = 0,
    SIMPLEX_NOISE_RIDGED,
    SIMPLEX_NOISE_BILLOWY,
    SIMPLEX_NOISE_FBM,
    SIMPLEX_NOISE_HYBRID_MULTIFRACTAL,
    SIMPLEX_NOISE_DOMAIN_WARP,
    SIMPLEX_NOISE_COUNT
} simplex_noise_variant_t;

/* Interpolation Methods */
typedef enum {
    SIMPLEX_INTERP_LINEAR = 0,
    SIMPLEX_INTERP_CUBIC,
    SIMPLEX_INTERP_HERMITE,
    SIMPLEX_INTERP_SMOOTHSTEP,
    SIMPLEX_INTERP_COUNT
} simplex_interp_type_t;

/* Precision Levels */
typedef enum {
    SIMPLEX_PRECISION_SINGLE = 0,
    SIMPLEX_PRECISION_DOUBLE,
    SIMPLEX_PRECISION_EXTENDED,
    SIMPLEX_PRECISION_COUNT
} simplex_precision_t;

/* Configuration Structure */
typedef struct {
    simplex_prng_type_t prng_type;
    simplex_noise_variant_t noise_variant;
    simplex_interp_type_t interp_type;
    simplex_precision_t precision;
    uint32_t seed;
    int enable_simd;
    int enable_caching;
    int enable_profiling;
    double persistence;
    double lacunarity;
    int octaves;
    double frequency;
    double amplitude;
    double offset;
    double scale;

    /* Advanced Configuration Options */
    char config_file
        [256];  // NOLINT(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-avoid-magic-numbers)
    char output_file
        [256];  // NOLINT(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-avoid-magic-numbers)
    int verbose_mode;
    int debug_mode;
    int auto_save;
    int validate_inputs;
    double cache_size_mb;
    int max_threads;
    int chunk_size;
    double memory_limit_mb;
} simplex_config_t;

/* Configuration File Types */
typedef enum {
    SIMPLEX_CONFIG_JSON = 0,
    SIMPLEX_CONFIG_YAML,
    SIMPLEX_CONFIG_INI,
    SIMPLEX_CONFIG_BINARY,
    SIMPLEX_CONFIG_COUNT
} simplex_config_type_t;

/* Configuration Validation Result */
typedef struct {
    int valid;
    int error_count;
    char errors
        [10]
        [256];  // NOLINT(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-avoid-magic-numbers)
    char warnings
        [10]
        [256];  // NOLINT(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-avoid-magic-numbers)
    int warning_count;
} simplex_config_validation_t;

/* Performance Statistics */
typedef struct {
    double generation_time;
    size_t memory_used;
    size_t cache_hits;
    size_t cache_misses;
    size_t function_calls;
    double average_execution_time;
} simplex_perf_stats_t;

/* ===== ADVANCED INITIALIZATION & CONFIGURATION ===== */

/**
 * Initialize simplex noise with advanced configuration
 * @param config Configuration structure with all parameters
 * @return 0 on success, negative error code on failure
 */
int simplex_noise_init_advanced(const simplex_config_t* config);

/**
 * Initialize simplex noise with a seed (legacy function)
 * @param seed Random seed (0 for time-based seed)
 */
void simplex_noise_init(unsigned int seed);

/**
 * Get default configuration
 * @return Default configuration structure
 */
simplex_config_t simplex_get_default_config(void);

/**
 * Set PRNG algorithm
 * @param prng_type Type of PRNG to use
 * @return 0 on success, negative error code on failure
 */
int simplex_set_prng(simplex_prng_type_t prng_type);

/**
 * Set noise variant
 * @param variant Type of noise variant to use
 * @return 0 on success, negative error code on failure
 */
int simplex_set_noise_variant(simplex_noise_variant_t variant);

/**
 * Set interpolation method
 * @param interp_type Type of interpolation to use
 * @return 0 on success, negative error code on failure
 */
int simplex_set_interpolation(simplex_interp_type_t interp_type);

/**
 * Enable or disable SIMD optimizations
 * @param enable 1 to enable, 0 to disable
 * @return 0 on success, negative error code on failure
 */
int simplex_set_simd(int enable);

/**
 * Enable or disable caching
 * @param enable 1 to enable, 0 to disable
 * @return 0 on success, negative error code on failure
 */
int simplex_set_caching(int enable);

/**
 * Enable or disable performance profiling
 * @param enable 1 to enable, 0 to disable
 * @return 0 on success, negative error code on failure
 */
int simplex_set_profiling(int enable);

/* ===== CONFIGURATION MANAGEMENT ===== */

/**
 * Load configuration from file
 * @param filename Path to configuration file
 * @param config_type Type of configuration file (JSON, YAML, INI, etc.)
 * @param config Pointer to configuration structure to fill
 * @return 0 on success, negative error code on failure
 */
int simplex_load_config(const char* filename, simplex_config_type_t config_type,
                        simplex_config_t* config);

/**
 * Save configuration to file
 * @param filename Path to output file
 * @param config_type Type of configuration file to save
 * @param config Pointer to configuration structure to save
 * @return 0 on success, negative error code on failure
 */
int simplex_save_config(const char* filename, simplex_config_type_t config_type,
                        const simplex_config_t* config);

/**
 * Validate configuration
 * @param config Pointer to configuration structure to validate
 * @param validation Pointer to validation result structure
 * @return 0 on success, negative error code on failure
 */
int simplex_validate_config(const simplex_config_t* config,
                            simplex_config_validation_t* validation);

/**
 * Reset configuration to defaults
 * @param config Pointer to configuration structure to reset
 * @return 0 on success, negative error code on failure
 */
int simplex_reset_config(simplex_config_t* config);

/**
 * Merge two configurations (base + override)
 * @param base Base configuration
 * @param override Override configuration
 * @param result Resulting merged configuration
 * @return 0 on success, negative error code on failure
 */
int simplex_merge_config(const simplex_config_t* base, const simplex_config_t* override,
                         simplex_config_t* result);

/**
 * Get configuration value as string
 * @param config Pointer to configuration structure
 * @param key Configuration key name
 * @param value Buffer to store value string
 * @param max_len Maximum length of value buffer
 * @return 0 on success, negative error code on failure
 */
int simplex_get_config_string(const simplex_config_t* config, const char* key, char* value,
                              size_t max_len);

/**
 * Set configuration value from string
 * @param config Pointer to configuration structure
 * @param key Configuration key name
 * @param value Value string to set
 * @return 0 on success, negative error code on failure
 */
int simplex_set_config_string(simplex_config_t* config, const char* key, const char* value);

/**
 * Get configuration value as double
 * @param config Pointer to configuration structure
 * @param key Configuration key name
 * @param value Pointer to store double value
 * @return 0 on success, negative error code on failure
 */
int simplex_get_config_double(const simplex_config_t* config, const char* key, double* value);

/**
 * Set configuration value from double
 * @param config Pointer to configuration structure
 * @param key Configuration key name
 * @param value Double value to set
 * @return 0 on success, negative error code on failure
 */
int simplex_set_config_double(simplex_config_t* config, const char* key, double value);

/**
 * Get configuration value as integer
 * @param config Pointer to configuration structure
 * @param key Configuration key name
 * @param value Pointer to store integer value
 * @return 0 on success, negative error code on failure
 */
int simplex_get_config_int(const simplex_config_t* config, const char* key, int* value);

/**
 * Set configuration value from integer
 * @param config Pointer to configuration structure
 * @param key Configuration key name
 * @param value Integer value to set
 * @return 0 on success, negative error code on failure
 */
int simplex_set_config_int(simplex_config_t* config, const char* key, int value);

/**
 * Print configuration to stdout
 * @param config Pointer to configuration structure
 * @param format Output format (0=compact, 1=verbose, 2=JSON)
 * @return 0 on success, negative error code on failure
 */
int simplex_print_config(const simplex_config_t* config, int format);

/**
 * Create example configuration file
 * @param filename Path to output file
 * @param config_type Type of configuration file to create
 * @return 0 on success, negative error code on failure
 */
int simplex_create_example_config(const char* filename, simplex_config_type_t config_type);

/* ===== CORE NOISE FUNCTIONS ===== */

/**
 * Generate 1D simplex noise
 * @param x Input coordinate
 * @return Noise value in range [-1, 1]
 */
double simplex_noise_1d(double x);

/**
 * Generate 2D simplex noise
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @return Noise value in range [-1, 1]
 */
double simplex_noise_2d(double x, double y);

/**
 * Generate 3D simplex noise
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param z Input z coordinate
 * @return Noise value in range [-1, 1]
 */
double simplex_noise_3d(double x, double y, double z);

/**
 * Generate 4D simplex noise
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param z Input z coordinate
 * @param w Input w coordinate
 * @return Noise value in range [-1, 1]
 */
double simplex_noise_4d(double x, double y, double z, double w);

/* ===== ADVANCED NOISE VARIANTS ===== */

/**
 * Generate ridged noise (1D)
 * @param x Input coordinate
 * @return Ridged noise value
 */
double simplex_ridged_1d(double x);

/**
 * Generate ridged noise (2D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @return Ridged noise value
 */
double simplex_ridged_2d(double x, double y);

/**
 * Generate ridged noise (3D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param z Input z coordinate
 * @return Ridged noise value
 */
double simplex_ridged_3d(double x, double y, double z);

/**
 * Generate billowy noise (1D)
 * @param x Input coordinate
 * @return Billowy noise value
 */
double simplex_billowy_1d(double x);

/**
 * Generate billowy noise (2D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @return Billowy noise value
 */
double simplex_billowy_2d(double x, double y);

/**
 * Generate billowy noise (3D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param z Input z coordinate
 * @return Billowy noise value
 */
double simplex_billowy_3d(double x, double y, double z);

/**
 * Generate Fractional Brownian Motion (fBm) noise (2D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param octaves Number of octaves
 * @param persistence Amplitude multiplier per octave
 * @param lacunarity Frequency multiplier per octave
 * @return fBm noise value
 */
double simplex_fbm_2d(double x, double y, int octaves, double persistence, double lacunarity);

/**
 * Generate Fractional Brownian Motion (fBm) noise (3D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param z Input z coordinate
 * @param octaves Number of octaves
 * @param persistence Amplitude multiplier per octave
 * @param lacunarity Frequency multiplier per octave
 * @return fBm noise value
 */
double simplex_fbm_3d(double x, double y, double z, int octaves, double persistence,
                      double lacunarity);

/**
 * Generate Hybrid Multi-Fractal noise (2D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param octaves Number of octaves
 * @param persistence Amplitude multiplier per octave
 * @param lacunarity Frequency multiplier per octave
 * @param offset Offset value for hybrid calculation
 * @return Hybrid Multi-Fractal noise value
 */
double simplex_hybrid_multifractal_2d(double x, double y, int octaves, double persistence,
                                      double lacunarity, double offset);

/**
 * Generate Domain Warping noise (2D)
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param warp_strength Strength of domain warping
 * @return Domain warped noise value
 */
double simplex_domain_warp_2d(double x, double y, double warp_strength);

/* ===== FRACTAL NOISE FUNCTIONS ===== */

/**
 * Generate fractal noise (multiple octaves) for 2D
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param octaves Number of octaves (default: 4)
 * @param persistence Amplitude multiplier per octave (default: 0.5)
 * @param lacunarity Frequency multiplier per octave (default: 2.0)
 * @return Fractal noise value
 */
double simplex_fractal_2d(double x, double y, int octaves, double persistence, double lacunarity);

/**
 * Generate fractal noise (multiple octaves) for 3D
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param z Input z coordinate
 * @param octaves Number of octaves (default: 4)
 * @param persistence Amplitude multiplier per octave (default: 0.5)
 * @param lacunarity Frequency multiplier per octave (default: 2.0)
 * @return Fractal noise value
 */
double simplex_fractal_3d(double x, double y, double z, int octaves, double persistence,
                          double lacunarity);

/* ===== PERFORMANCE & UTILITY FUNCTIONS ===== */

/**
 * Get performance statistics
 * @param stats Pointer to statistics structure to fill
 * @return 0 on success, negative error code on failure
 */
int simplex_get_performance_stats(simplex_perf_stats_t* stats);

/**
 * Reset performance statistics
 */
void simplex_reset_performance_stats(void);

/**
 * Get function call count
 * @return Number of function calls made
 */
size_t simplex_get_function_call_count(void);

/**
 * Get cache hit count
 * @return Number of cache hits
 */
int simplex_get_cache_hits(void);

/**
 * Get cache miss count
 * @return Number of cache misses
 */
int simplex_get_cache_misses(void);

/**
 * Generate noise array (2D) - optimized for bulk generation
 * @param x_start Starting x coordinate
 * @param y_start Starting y coordinate
 * @param width Width of the array
 * @param height Height of the array
 * @param step Step size between samples
 * @param output Array to store results (must be width*height elements)
 * @return 0 on success, negative error code on failure
 */
int simplex_noise_array_2d(double x_start, double y_start, int width, int height, double step,
                           double* output);

/**
 * Generate noise array (3D) - optimized for bulk generation
 * @param x_start Starting x coordinate
 * @param y_start Starting y coordinate
 * @param z_start Starting z coordinate
 * @param width Width of the array
 * @param height Height of the array
 * @param depth Depth of the array
 * @param step Step size between samples
 * @param output Array to store results (must be width*height*depth elements)
 * @return 0 on success, negative error code on failure
 */
int simplex_noise_array_3d(double x_start, double y_start, double z_start, int width, int height,
                           int depth, double step, double* output);

/**
 * Cleanup and free resources
 */
void simplex_cleanup(void);

#endif  // SIMPLEX_NOISE_H
