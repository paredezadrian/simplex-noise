# Performance Guide

This guide covers performance optimization techniques and best practices for the Simplex Noise Library.

## Performance Characteristics

### Baseline Performance

- **Single noise call**: ~1-5 microseconds
- **Bulk generation**: ~1M samples/second on modern hardware
- **Memory usage**: ~50KB base + configurable cache
- **Thread safety**: Safe for concurrent access

### Performance Factors

1. **PRNG Algorithm** - Different algorithms have different speeds
2. **Noise Variant** - Some variants are more expensive than others
3. **Fractal Octaves** - More octaves = more computation
4. **Caching** - Can significantly improve repeated access
5. **Bulk Generation** - Much faster than individual calls

## Optimization Techniques

### 1. Choose the Right PRNG

```c
simplex_config_t config = simplex_get_default_config();

// Fastest (but lower quality)
config.prng_type = SIMPLEX_PRG_LINEAR_CONGRUENTIAL;

// Balanced (good quality and speed)
config.prng_type = SIMPLEX_PRG_PCG;

// Highest quality (slower)
config.prng_type = SIMPLEX_PRG_MERSENNE_TWISTER;
```

**Performance Ranking:**

1. Linear Congruential (fastest)
2. PCG (balanced)
3. Xorshift (good)
4. Mersenne Twister (slowest, highest quality)

### 2. Use Bulk Generation

**❌ Inefficient - Individual calls:**

```c
double* noise_data = malloc(width * height * sizeof(double));
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        noise_data[y * width + x] = simplex_noise_2d(x * 0.1, y * 0.1);
    }
}
```

**✅ Efficient - Bulk generation:**

```c
double* noise_data = malloc(width * height * sizeof(double));
simplex_noise_array_2d(0, 0, width, height, 0.1, noise_data);
```

**Performance improvement:** 3-5x faster for large arrays.

### 3. Enable Caching

```c
simplex_config_t config = simplex_get_default_config();
config.enable_caching = 1;
config.cache_size_mb = 64.0;  // Adjust based on needs
simplex_noise_init_advanced(&config);
```

**When to use caching:**

- Repeated access to same coordinates
- Interactive applications with real-time updates
- Memory is not a constraint

**Cache hit rates:**

- Terrain generation: 60-80%
- Texture synthesis: 40-60%
- Random sampling: 10-20%

### 4. Optimize Fractal Parameters

```c
// Fewer octaves = faster generation
config.octaves = 4;  // Instead of 8

// Lower persistence = less computation per octave
config.persistence = 0.5;  // Instead of 0.7

// Higher lacunarity = fewer samples per octave
config.lacunarity = 2.0;  // Good default
```

### 5. Use Appropriate Noise Variants

**Performance ranking (fastest to slowest):**

1. Classic simplex noise
2. Billowy noise
3. Ridged noise
4. Fractional Brownian Motion
5. Hybrid Multi-Fractal
6. Domain Warping

### 6. Memory Management

```c
// Set appropriate memory limits
config.memory_limit_mb = 256.0;  // Adjust based on available memory
config.cache_size_mb = 64.0;     // Balance between speed and memory

// Clean up when done
simplex_cleanup();
```

## Benchmarking

### Performance Testing

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simplex_noise.h"

void benchmark_noise_generation() {
    const int iterations = 1000000;
    clock_t start, end;
    double time_taken;

    simplex_noise_init(12345);

    // Benchmark 2D noise
    start = clock();
    for (int i = 0; i < iterations; i++) {
        double noise = simplex_noise_2d(i * 0.001, i * 0.001);
        (void)noise;  // Prevent optimization
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("2D Noise: %.0f calls/second\n", iterations / time_taken);

    // Benchmark fractal noise
    start = clock();
    for (int i = 0; i < iterations / 10; i++) {  // Fewer iterations due to complexity
        double noise = simplex_fractal_2d(i * 0.001, i * 0.001, 4, 0.5, 2.0);
        (void)noise;
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Fractal Noise: %.0f calls/second\n", (iterations / 10) / time_taken);
}

int main() {
    benchmark_noise_generation();
    return 0;
}
```

### Profiling with Built-in Stats

```c
void profile_application() {
    simplex_config_t config = simplex_get_default_config();
    config.enable_profiling = 1;
    config.enable_caching = 1;

    simplex_noise_init_advanced(&config);

    // Your application code here
    for (int i = 0; i < 1000; i++) {
        simplex_noise_2d(i * 0.1, i * 0.1);
    }

    // Get performance statistics
    simplex_perf_stats_t stats;
    simplex_get_performance_stats(&stats);

    printf("Performance Statistics:\n");
    printf("Function calls: %zu\n", stats.function_calls);
    printf("Cache hits: %zu\n", stats.cache_hits);
    printf("Cache misses: %zu\n", stats.cache_misses);
    printf("Cache hit rate: %.1f%%\n",
           (double)stats.cache_hits / stats.function_calls * 100.0);
    printf("Average execution time: %.3f microseconds\n",
           stats.average_execution_time);
}
```

## Real-World Performance Tips

### 1. Terrain Generation

```c
// For large terrain chunks, use bulk generation
void generate_terrain_chunk(int chunk_x, int chunk_z, int chunk_size) {
    double* heightmap = malloc(chunk_size * chunk_size * sizeof(double));

    // Use bulk generation for the entire chunk
    simplex_noise_array_2d(
        chunk_x * chunk_size * 0.01,
        chunk_z * chunk_size * 0.01,
        chunk_size,
        chunk_size,
        0.01,
        heightmap
    );

    // Process heightmap...
    free(heightmap);
}
```

### 2. Texture Generation

```c
// For texture synthesis, consider caching
void generate_texture(int width, int height) {
    simplex_config_t config = simplex_get_default_config();
    config.enable_caching = 1;  // Enable for repeated access
    simplex_noise_init_advanced(&config);

    // Generate texture with multiple octaves
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise = simplex_fractal_2d(x * 0.01, y * 0.01, 3, 0.5, 2.0);
            // Use noise for texture...
        }
    }
}
```

### 3. Real-time Applications

```c
// For real-time applications, optimize for speed
void setup_realtime_noise() {
    simplex_config_t config = simplex_get_default_config();

    // Use fastest PRNG
    config.prng_type = SIMPLEX_PRG_LINEAR_CONGRUENTIAL;

    // Enable caching for repeated access
    config.enable_caching = 1;
    config.cache_size_mb = 32.0;

    // Use fewer octaves for fractal noise
    config.octaves = 3;

    simplex_noise_init_advanced(&config);
}
```

## Memory Optimization

### 1. Configurable Memory Limits

```c
// Set memory limits based on available resources
simplex_config_t config = simplex_get_default_config();

// For memory-constrained systems
config.memory_limit_mb = 64.0;
config.cache_size_mb = 16.0;

// For high-performance systems
config.memory_limit_mb = 512.0;
config.cache_size_mb = 128.0;
```

### 2. Cleanup Strategies

```c
// Clean up when switching between different noise types
void switch_noise_type(simplex_noise_variant_t new_variant) {
    simplex_cleanup();  // Clear cache and reset state

    simplex_config_t config = simplex_get_default_config();
    config.noise_variant = new_variant;
    simplex_noise_init_advanced(&config);
}
```

## Threading Considerations

### Thread Safety

The library is thread-safe for read operations, but configuration changes should be synchronized:

```c
// Safe: Multiple threads can call noise functions
double noise1 = simplex_noise_2d(x1, y1);  // Thread 1
double noise2 = simplex_noise_2d(x2, y2);  // Thread 2

// Unsafe: Configuration changes need synchronization
simplex_set_prng(SIMPLEX_PRG_PCG);  // Needs mutex protection
```

### Multi-threaded Generation

```c
#include <pthread.h>

typedef struct {
    int start_y, end_y;
    int width;
    double* noise_data;
} thread_data_t;

void* generate_noise_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    for (int y = data->start_y; y < data->end_y; y++) {
        for (int x = 0; x < data->width; x++) {
            data->noise_data[y * data->width + x] =
                simplex_noise_2d(x * 0.01, y * 0.01);
        }
    }

    return NULL;
}

void generate_noise_parallel(int width, int height, int num_threads) {
    double* noise_data = malloc(width * height * sizeof(double));
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(num_threads * sizeof(thread_data_t));

    int rows_per_thread = height / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start_y = i * rows_per_thread;
        thread_data[i].end_y = (i + 1) * rows_per_thread;
        thread_data[i].width = width;
        thread_data[i].noise_data = noise_data;

        pthread_create(&threads[i], NULL, generate_noise_thread, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_data);
    free(noise_data);
}
```

## Performance Monitoring

### Built-in Profiling

```c
void monitor_performance() {
    simplex_config_t config = simplex_get_default_config();
    config.enable_profiling = 1;
    simplex_noise_init_advanced(&config);

    // Your application code...

    // Check performance periodically
    simplex_perf_stats_t stats;
    simplex_get_performance_stats(&stats);

    if (stats.cache_hits < stats.function_calls * 0.5) {
        printf("Warning: Low cache hit rate (%.1f%%)\n",
               (double)stats.cache_hits / stats.function_calls * 100.0);
    }

    if (stats.memory_used > config.memory_limit_mb * 1024 * 1024 * 0.9) {
        printf("Warning: High memory usage (%.1f MB)\n",
               stats.memory_used / (1024.0 * 1024.0));
    }
}
```

## Best Practices Summary

1. **Use bulk generation** for large arrays
2. **Enable caching** for repeated access patterns
3. **Choose appropriate PRNG** for your quality/speed needs
4. **Optimize fractal parameters** based on visual requirements
5. **Monitor performance** with built-in profiling
6. **Clean up resources** when switching configurations
7. **Use threading** for parallel generation when appropriate
8. **Set memory limits** based on available resources

---

_For more advanced optimization techniques, see [Advanced Usage](advanced-usage.md)_
