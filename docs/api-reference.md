# API Reference

This document provides comprehensive API documentation for the Simplex Noise Library.

## Core Functions

### Initialization

#### `simplex_noise_init(unsigned int seed)`

Initialize the noise generator with a seed.

**Parameters:**

- `seed` - Random seed (0 for time-based seed)

**Example:**

```c
simplex_noise_init(12345);
```

#### `simplex_noise_init_advanced(const simplex_config_t* config)`

Initialize with advanced configuration.

**Parameters:**

- `config` - Configuration structure

**Returns:**

- `0` on success, negative error code on failure

**Example:**

```c
simplex_config_t config = simplex_get_default_config();
config.octaves = 6;
config.persistence = 0.6;
simplex_noise_init_advanced(&config);
```

### Basic Noise Functions

#### `double simplex_noise_1d(double x)`

Generate 1D simplex noise.

**Parameters:**

- `x` - Input coordinate

**Returns:**

- Noise value in range [-1, 1]

#### `double simplex_noise_2d(double x, double y)`

Generate 2D simplex noise.

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate

**Returns:**

- Noise value in range [-1, 1]

#### `double simplex_noise_3d(double x, double y, double z)`

Generate 3D simplex noise.

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `z` - Input z coordinate

**Returns:**

- Noise value in range [-1, 1]

#### `double simplex_noise_4d(double x, double y, double z, double w)`

Generate 4D simplex noise.

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `z` - Input z coordinate
- `w` - Input w coordinate

**Returns:**

- Noise value in range [-1, 1]

### Fractal Noise Functions

#### `double simplex_fractal_2d(double x, double y, int octaves, double persistence, double lacunarity)`

Generate 2D fractal noise.

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `octaves` - Number of octaves
- `persistence` - Amplitude multiplier per octave
- `lacunarity` - Frequency multiplier per octave

**Returns:**

- Fractal noise value

#### `double simplex_fractal_3d(double x, double y, double z, int octaves, double persistence, double lacunarity)`

Generate 3D fractal noise.

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `z` - Input z coordinate
- `octaves` - Number of octaves
- `persistence` - Amplitude multiplier per octave
- `lacunarity` - Frequency multiplier per octave

**Returns:**

- Fractal noise value

### Advanced Noise Variants

#### `double simplex_ridged_2d(double x, double y)`

Generate ridged noise (2D).

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate

**Returns:**

- Ridged noise value

#### `double simplex_billowy_2d(double x, double y)`

Generate billowy noise (2D).

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate

**Returns:**

- Billowy noise value

#### `double simplex_fbm_2d(double x, double y, int octaves, double persistence, double lacunarity)`

Generate Fractional Brownian Motion noise (2D).

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `octaves` - Number of octaves
- `persistence` - Amplitude multiplier per octave
- `lacunarity` - Frequency multiplier per octave

**Returns:**

- fBm noise value

#### `double simplex_hybrid_multifractal_2d(double x, double y, int octaves, double persistence, double lacunarity, double offset)`

Generate Hybrid Multi-Fractal noise (2D).

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `octaves` - Number of octaves
- `persistence` - Amplitude multiplier per octave
- `lacunarity` - Frequency multiplier per octave
- `offset` - Offset value for hybrid calculation

**Returns:**

- Hybrid Multi-Fractal noise value

#### `double simplex_domain_warp_2d(double x, double y, double warp_strength)`

Generate Domain Warping noise (2D).

**Parameters:**

- `x` - Input x coordinate
- `y` - Input y coordinate
- `warp_strength` - Strength of domain warping

**Returns:**

- Domain warped noise value

### Bulk Generation Functions

#### `int simplex_noise_array_2d(double x_start, double y_start, int width, int height, double step, double* output)`

Generate 2D noise array for bulk processing.

**Parameters:**

- `x_start` - Starting x coordinate
- `y_start` - Starting y coordinate
- `width` - Width of the array
- `height` - Height of the array
- `step` - Step size between samples
- `output` - Array to store results (must be width\*height elements)

**Returns:**

- `0` on success, negative error code on failure

#### `int simplex_noise_array_3d(double x_start, double y_start, double z_start, int width, int height, int depth, double step, double* output)`

Generate 3D noise array for bulk processing.

**Parameters:**

- `x_start` - Starting x coordinate
- `y_start` - Starting y coordinate
- `z_start` - Starting z coordinate
- `width` - Width of the array
- `height` - Height of the array
- `depth` - Depth of the array
- `step` - Step size between samples
- `output` - Array to store results (must be width*height*depth elements)

**Returns:**

- `0` on success, negative error code on failure

### Configuration Functions

#### `simplex_config_t simplex_get_default_config(void)`

Get default configuration.

**Returns:**

- Default configuration structure

#### `int simplex_set_prng(simplex_prng_type_t prng_type)`

Set PRNG algorithm.

**Parameters:**

- `prng_type` - Type of PRNG to use

**Returns:**

- `0` on success, negative error code on failure

#### `int simplex_set_noise_variant(simplex_noise_variant_t variant)`

Set noise variant.

**Parameters:**

- `variant` - Type of noise variant to use

**Returns:**

- `0` on success, negative error code on failure

#### `int simplex_set_interpolation(simplex_interp_type_t interp_type)`

Set interpolation method.

**Parameters:**

- `interp_type` - Type of interpolation to use

**Returns:**

- `0` on success, negative error code on failure

### Performance Functions

#### `int simplex_get_performance_stats(simplex_perf_stats_t* stats)`

Get performance statistics.

**Parameters:**

- `stats` - Pointer to statistics structure to fill

**Returns:**

- `0` on success, negative error code on failure

#### `void simplex_reset_performance_stats(void)`

Reset performance statistics.

#### `size_t simplex_get_function_call_count(void)`

Get function call count.

**Returns:**

- Number of function calls made

#### `int simplex_get_cache_hits(void)`

Get cache hit count.

**Returns:**

- Number of cache hits

#### `int simplex_get_cache_misses(void)`

Get cache miss count.

**Returns:**

- Number of cache misses

### Cleanup Functions

#### `void simplex_cleanup(void)`

Cleanup and free resources.

## Data Types

### `simplex_config_t`

Configuration structure containing all noise parameters.

### `simplex_prng_type_t`

PRNG algorithm types:

- `SIMPLEX_PRG_LINEAR_CONGRUENTIAL`
- `SIMPLEX_PRG_MERSENNE_TWISTER`
- `SIMPLEX_PRG_XORSHIFT`
- `SIMPLEX_PRG_PCG`

### `simplex_noise_variant_t`

Noise variant types:

- `SIMPLEX_NOISE_CLASSIC`
- `SIMPLEX_NOISE_RIDGED`
- `SIMPLEX_NOISE_BILLOWY`
- `SIMPLEX_NOISE_FBM`
- `SIMPLEX_NOISE_HYBRID_MULTIFRACTAL`
- `SIMPLEX_NOISE_DOMAIN_WARP`

### `simplex_interp_type_t`

Interpolation types:

- `SIMPLEX_INTERP_LINEAR`
- `SIMPLEX_INTERP_CUBIC`
- `SIMPLEX_INTERP_HERMITE`
- `SIMPLEX_INTERP_SMOOTHSTEP`

### `simplex_perf_stats_t`

Performance statistics structure.

---

_For image generation API, see [Image Generation](image-generation.md)_
