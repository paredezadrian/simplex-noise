# Configuration Guide

This guide covers the comprehensive configuration system of the Simplex Noise Library.

## Configuration Overview

The library provides a flexible configuration system that allows you to customize:

- PRNG algorithms and seeding
- Noise variants and interpolation methods
- Performance settings and optimizations
- File I/O and output options
- Memory management and caching

## Basic Configuration

### Default Configuration

```c
simplex_config_t config = simplex_get_default_config();
```

**Default values:**

- PRNG: PCG (high quality)
- Noise variant: Classic simplex
- Interpolation: Smoothstep
- Precision: Double
- Octaves: 4
- Persistence: 0.5
- Lacunarity: 2.0
- Caching: Enabled
- Profiling: Disabled

### Simple Initialization

```c
// Initialize with seed only
simplex_noise_init(12345);

// Initialize with full configuration
simplex_config_t config = simplex_get_default_config();
config.seed = 12345;
config.octaves = 6;
simplex_noise_init_advanced(&config);
```

## PRNG Configuration

### Available PRNGs

```c
typedef enum {
    SIMPLEX_PRG_LINEAR_CONGRUENTIAL,  // Fastest, lower quality
    SIMPLEX_PRG_MERSENNE_TWISTER,     // High quality, slower
    SIMPLEX_PRG_XORSHIFT,             // Good balance
    SIMPLEX_PRG_PCG,                  // Best quality, good speed
    SIMPLEX_PRG_CUSTOM                // User-defined
} simplex_prng_type_t;
```

### Setting PRNG

```c
simplex_config_t config = simplex_get_default_config();

// Choose PRNG algorithm
config.prng_type = SIMPLEX_PRG_PCG;

// Or use the setter function
simplex_set_prng(SIMPLEX_PRG_PCG);
```

### PRNG Performance Comparison

| PRNG Type           | Speed   | Quality | Memory | Use Case                |
| ------------------- | ------- | ------- | ------ | ----------------------- |
| Linear Congruential | Fastest | Low     | Low    | Real-time, simple needs |
| Xorshift            | Fast    | Good    | Low    | Balanced applications   |
| PCG                 | Medium  | High    | Medium | General purpose         |
| Mersenne Twister    | Slowest | Highest | High   | High-quality generation |

## Noise Variants

### Available Variants

```c
typedef enum {
    SIMPLEX_NOISE_CLASSIC,           // Standard simplex noise
    SIMPLEX_NOISE_RIDGED,            // Ridged patterns
    SIMPLEX_NOISE_BILLOWY,           // Billowy patterns
    SIMPLEX_NOISE_FBM,               // Fractional Brownian Motion
    SIMPLEX_NOISE_HYBRID_MULTIFRACTAL, // Hybrid Multi-Fractal
    SIMPLEX_NOISE_DOMAIN_WARP        // Domain Warping
} simplex_noise_variant_t;
```

### Setting Noise Variant

```c
simplex_config_t config = simplex_get_default_config();
config.noise_variant = SIMPLEX_NOISE_RIDGED;

// Or use the setter function
simplex_set_noise_variant(SIMPLEX_NOISE_RIDGED);
```

### Noise Variant Characteristics

| Variant              | Description               | Use Case                 | Performance |
| -------------------- | ------------------------- | ------------------------ | ----------- |
| Classic              | Standard simplex noise    | General purpose          | Fastest     |
| Ridged               | Sharp ridges and valleys  | Mountain ranges, canyons | Fast        |
| Billowy              | Soft, cloud-like patterns | Clouds, organic shapes   | Fast        |
| fBm                  | Multiple octaves combined | Terrain, textures        | Medium      |
| Hybrid Multi-Fractal | Complex terrain patterns  | Realistic terrain        | Slower      |
| Domain Warp          | Distorted noise patterns  | Organic, flowing shapes  | Slowest     |

## Interpolation Methods

### Available Methods

```c
typedef enum {
    SIMPLEX_INTERP_LINEAR,      // Linear interpolation
    SIMPLEX_INTERP_CUBIC,       // Cubic interpolation
    SIMPLEX_INTERP_HERMITE,     // Hermite interpolation
    SIMPLEX_INTERP_SMOOTHSTEP   // Smoothstep interpolation
} simplex_interp_type_t;
```

### Setting Interpolation

```c
simplex_config_t config = simplex_get_default_config();
config.interp_type = SIMPLEX_INTERP_SMOOTHSTEP;

// Or use the setter function
simplex_set_interpolation(SIMPLEX_INTERP_SMOOTHSTEP);
```

### Interpolation Characteristics

| Method     | Smoothness | Performance | Use Case                  |
| ---------- | ---------- | ----------- | ------------------------- |
| Linear     | Basic      | Fastest     | Simple applications       |
| Cubic      | Good       | Fast        | General purpose           |
| Hermite    | Very good  | Medium      | High-quality output       |
| Smoothstep | Best       | Medium      | Professional applications |

## Performance Configuration

### Caching Settings

```c
simplex_config_t config = simplex_get_default_config();

// Enable/disable caching
config.enable_caching = 1;
config.cache_size_mb = 64.0;  // Cache size in MB

// Or use setter functions
simplex_set_caching(1);
```

### SIMD Optimization

```c
simplex_config_t config = simplex_get_default_config();
config.enable_simd = 1;  // Enable SIMD optimizations

// Or use setter function
simplex_set_simd(1);
```

### Profiling

```c
simplex_config_t config = simplex_get_default_config();
config.enable_profiling = 1;  // Enable performance profiling

// Or use setter function
simplex_set_profiling(1);
```

### Memory Management

```c
simplex_config_t config = simplex_get_default_config();

// Set memory limits
config.memory_limit_mb = 256.0;    // Maximum memory usage
config.cache_size_mb = 64.0;       // Cache size
config.max_threads = 4;            // Maximum threads
config.chunk_size = 1024;          // Processing chunk size
```

## Fractal Parameters

### Octaves

```c
simplex_config_t config = simplex_get_default_config();
config.octaves = 6;  // Number of octaves (1-16)

// More octaves = more detail but slower
// Fewer octaves = less detail but faster
```

### Persistence

```c
config.persistence = 0.5;  // Amplitude multiplier per octave (0.0-1.0)

// Higher persistence = more contrast between octaves
// Lower persistence = smoother transitions
```

### Lacunarity

```c
config.lacunarity = 2.0;  // Frequency multiplier per octave (1.0-4.0)

// Higher lacunarity = more detail at higher frequencies
// Lower lacunarity = smoother, more uniform patterns
```

### Frequency and Amplitude

```c
config.frequency = 1.0;  // Base frequency multiplier
config.amplitude = 1.0;  // Base amplitude multiplier
config.offset = 0.0;     // Base offset
config.scale = 1.0;      // Overall scale factor
```

## File I/O Configuration

### Configuration Files

```c
simplex_config_t config = simplex_get_default_config();

// Set configuration file paths
strcpy(config.config_file, "noise_config.ini");
strcpy(config.output_file, "output.ppm");

// Enable auto-save
config.auto_save = 1;
```

### Verbose and Debug Modes

```c
config.verbose_mode = 1;  // Enable verbose output
config.debug_mode = 1;    // Enable debug information
config.validate_inputs = 1;  // Validate input parameters
```

## Configuration File Management

### Loading Configuration

```c
simplex_config_t config;

// Load from INI file
if (simplex_load_config("config.ini", SIMPLEX_CONFIG_INI, &config) == 0) {
    printf("Configuration loaded successfully\n");
}

// Load from JSON file
if (simplex_load_config("config.json", SIMPLEX_CONFIG_JSON, &config) == 0) {
    printf("Configuration loaded successfully\n");
}
```

### Saving Configuration

```c
simplex_config_t config = simplex_get_default_config();
// ... modify configuration ...

// Save to INI file
if (simplex_save_config("config.ini", SIMPLEX_CONFIG_INI, &config) == 0) {
    printf("Configuration saved successfully\n");
}

// Save to JSON file
if (simplex_save_config("config.json", SIMPLEX_CONFIG_JSON, &config) == 0) {
    printf("Configuration saved successfully\n");
}
```

### Configuration Validation

```c
simplex_config_t config = simplex_get_default_config();
simplex_config_validation_t validation;

if (simplex_validate_config(&config, &validation) == 0) {
    if (validation.valid) {
        printf("Configuration is valid\n");
    } else {
        printf("Configuration has %d errors:\n", validation.error_count);
        for (int i = 0; i < validation.error_count; i++) {
            printf("  Error: %s\n", validation.errors[i]);
        }
    }

    if (validation.warning_count > 0) {
        printf("Configuration has %d warnings:\n", validation.warning_count);
        for (int i = 0; i < validation.warning_count; i++) {
            printf("  Warning: %s\n", validation.warnings[i]);
        }
    }
}
```

## Runtime Configuration

### Dynamic Configuration Changes

```c
// Change PRNG at runtime
simplex_set_prng(SIMPLEX_PRG_MERSENNE_TWISTER);

// Change noise variant
simplex_set_noise_variant(SIMPLEX_NOISE_RIDGED);

// Toggle caching
simplex_set_caching(0);  // Disable caching
simplex_set_caching(1);  // Enable caching

// Toggle profiling
simplex_set_profiling(1);  // Enable profiling
```

### Configuration Merging

```c
simplex_config_t base_config = simplex_get_default_config();
simplex_config_t override_config = simplex_get_default_config();
simplex_config_t merged_config;

// Modify override configuration
override_config.octaves = 8;
override_config.persistence = 0.7;

// Merge configurations
if (simplex_merge_config(&base_config, &override_config, &merged_config) == 0) {
    // merged_config now contains base_config with override_config changes
    simplex_noise_init_advanced(&merged_config);
}
```

## Configuration Examples

### High-Quality Configuration

```c
simplex_config_t config = simplex_get_default_config();

// Use highest quality PRNG
config.prng_type = SIMPLEX_PRG_MERSENNE_TWISTER;

// Use smooth interpolation
config.interp_type = SIMPLEX_INTERP_SMOOTHSTEP;

// Enable all optimizations
config.enable_simd = 1;
config.enable_caching = 1;
config.enable_profiling = 1;

// High-quality fractal parameters
config.octaves = 8;
config.persistence = 0.6;
config.lacunarity = 2.0;

// Large memory allocation
config.memory_limit_mb = 512.0;
config.cache_size_mb = 128.0;

simplex_noise_init_advanced(&config);
```

### Real-Time Configuration

```c
simplex_config_t config = simplex_get_default_config();

// Use fastest PRNG
config.prng_type = SIMPLEX_PRG_LINEAR_CONGRUENTIAL;

// Use linear interpolation for speed
config.interp_type = SIMPLEX_INTERP_LINEAR;

// Enable caching for repeated access
config.enable_caching = 1;
config.cache_size_mb = 32.0;

// Fewer octaves for speed
config.octaves = 3;
config.persistence = 0.5;

// Conservative memory usage
config.memory_limit_mb = 128.0;

simplex_noise_init_advanced(&config);
```

### Memory-Constrained Configuration

```c
simplex_config_t config = simplex_get_default_config();

// Use balanced PRNG
config.prng_type = SIMPLEX_PRG_PCG;

// Disable caching to save memory
config.enable_caching = 0;

// Fewer octaves
config.octaves = 2;

// Small memory allocation
config.memory_limit_mb = 64.0;
config.cache_size_mb = 0.0;

simplex_noise_init_advanced(&config);
```

## Configuration Best Practices

1. **Start with defaults** - Use `simplex_get_default_config()` as a starting point
2. **Validate configurations** - Always validate before using
3. **Profile performance** - Enable profiling to monitor performance
4. **Use appropriate PRNG** - Choose based on quality vs speed needs
5. **Optimize memory usage** - Set appropriate memory limits
6. **Save configurations** - Save working configurations for reuse
7. **Document changes** - Keep track of configuration modifications

---

_For more advanced configuration techniques, see [Advanced Usage](advanced-usage.md)_
