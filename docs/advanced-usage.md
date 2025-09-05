# Advanced Usage Guide

This guide covers advanced techniques and patterns for using the Simplex Noise Library in complex applications.

## Advanced Configuration Patterns

### Configuration Inheritance

```c
// Base configuration for all noise types
simplex_config_t base_config = simplex_get_default_config();
base_config.enable_caching = 1;
base_config.enable_profiling = 1;
base_config.memory_limit_mb = 512.0;

// Terrain-specific configuration
simplex_config_t terrain_config = base_config;
terrain_config.noise_variant = SIMPLEX_NOISE_FBM;
terrain_config.octaves = 8;
terrain_config.persistence = 0.6;
terrain_config.lacunarity = 2.0;

// Texture-specific configuration
simplex_config_t texture_config = base_config;
texture_config.noise_variant = SIMPLEX_NOISE_CLASSIC;
texture_config.octaves = 3;
texture_config.persistence = 0.5;
texture_config.lacunarity = 2.0;
```

### Dynamic Configuration Switching

```c
typedef struct {
    simplex_config_t config;
    const char* name;
} noise_preset_t;

noise_preset_t presets[] = {
    {.config = {0}, .name = "terrain"},
    {.config = {0}, .name = "clouds"},
    {.config = {0}, .name = "textures"}
};

void initialize_presets() {
    // Terrain preset
    presets[0].config = simplex_get_default_config();
    presets[0].config.noise_variant = SIMPLEX_NOISE_FBM;
    presets[0].config.octaves = 8;
    presets[0].config.persistence = 0.6;

    // Cloud preset
    presets[1].config = simplex_get_default_config();
    presets[1].config.noise_variant = SIMPLEX_NOISE_CLASSIC;
    presets[1].config.octaves = 4;
    presets[1].config.persistence = 0.5;

    // Texture preset
    presets[2].config = simplex_get_default_config();
    presets[2].config.noise_variant = SIMPLEX_NOISE_BILLOWY;
    presets[2].config.octaves = 3;
    presets[2].config.persistence = 0.4;
}

void switch_preset(int preset_index) {
    if (preset_index >= 0 && preset_index < 3) {
        simplex_cleanup();
        simplex_noise_init_advanced(&presets[preset_index].config);
        printf("Switched to %s preset\n", presets[preset_index].name);
    }
}
```

## Multi-Threaded Applications

### Thread-Safe Noise Generation

```c
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int thread_id;
    int start_x, end_x;
    int start_y, end_y;
    double* noise_data;
    double scale;
    sem_t* semaphore;
} thread_data_t;

void* noise_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    // Generate noise for assigned region
    for (int y = data->start_y; y < data->end_y; y++) {
        for (int x = data->start_x; x < data->end_x; x++) {
            double noise = simplex_noise_2d(x * data->scale, y * data->scale);
            data->noise_data[y * 1000 + x] = noise;  // Assuming 1000x1000 grid
        }
    }

    // Signal completion
    sem_post(data->semaphore);
    return NULL;
}

void generate_noise_parallel(int width, int height, int num_threads) {
    double* noise_data = malloc(width * height * sizeof(double));
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(num_threads * sizeof(thread_data_t));
    sem_t semaphore;

    sem_init(&semaphore, 0, 0);

    int rows_per_thread = height / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start_x = 0;
        thread_data[i].end_x = width;
        thread_data[i].start_y = i * rows_per_thread;
        thread_data[i].end_y = (i + 1) * rows_per_thread;
        thread_data[i].noise_data = noise_data;
        thread_data[i].scale = 0.01;
        thread_data[i].semaphore = &semaphore;

        pthread_create(&threads[i], NULL, noise_thread, &thread_data[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        sem_wait(&semaphore);
    }

    // Cleanup
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_data);
    free(noise_data);
    sem_destroy(&semaphore);
}
```

### Thread Pool Implementation

```c
typedef struct {
    pthread_t* threads;
    int num_threads;
    sem_t work_semaphore;
    sem_t result_semaphore;
    pthread_mutex_t work_mutex;
    int shutdown;
} thread_pool_t;

typedef struct {
    int x, y;
    double result;
    int completed;
} work_item_t;

thread_pool_t* create_thread_pool(int num_threads) {
    thread_pool_t* pool = malloc(sizeof(thread_pool_t));
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    pool->num_threads = num_threads;
    pool->shutdown = 0;

    sem_init(&pool->work_semaphore, 0, 0);
    sem_init(&pool->result_semaphore, 0, 0);
    pthread_mutex_init(&pool->work_mutex, NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }

    return pool;
}

void* worker_thread(void* arg) {
    thread_pool_t* pool = (thread_pool_t*)arg;

    while (!pool->shutdown) {
        sem_wait(&pool->work_semaphore);

        if (!pool->shutdown) {
            // Process work item
            work_item_t* item = get_next_work_item(pool);
            if (item) {
                item->result = simplex_noise_2d(item->x * 0.01, item->y * 0.01);
                item->completed = 1;
                sem_post(&pool->result_semaphore);
            }
        }
    }

    return NULL;
}
```

## Memory Management

### Custom Memory Allocators

```c
typedef struct {
    void* (*malloc)(size_t size);
    void* (*realloc)(void* ptr, size_t size);
    void (*free)(void* ptr);
} custom_allocator_t;

custom_allocator_t* create_allocator() {
    custom_allocator_t* allocator = malloc(sizeof(custom_allocator_t));
    allocator->malloc = my_malloc;
    allocator->realloc = my_realloc;
    allocator->free = my_free;
    return allocator;
}

void* my_malloc(size_t size) {
    printf("Allocating %zu bytes\n", size);
    return malloc(size);
}

void my_free(void* ptr) {
    printf("Freeing memory\n");
    free(ptr);
}
```

### Memory Pool Implementation

```c
typedef struct {
    void* memory;
    size_t size;
    size_t used;
    pthread_mutex_t mutex;
} memory_pool_t;

memory_pool_t* create_memory_pool(size_t size) {
    memory_pool_t* pool = malloc(sizeof(memory_pool_t));
    pool->memory = malloc(size);
    pool->size = size;
    pool->used = 0;
    pthread_mutex_init(&pool->mutex, NULL);
    return pool;
}

void* pool_alloc(memory_pool_t* pool, size_t size) {
    pthread_mutex_lock(&pool->mutex);

    if (pool->used + size > pool->size) {
        pthread_mutex_unlock(&pool->mutex);
        return NULL;  // Pool exhausted
    }

    void* ptr = (char*)pool->memory + pool->used;
    pool->used += size;

    pthread_mutex_unlock(&pool->mutex);
    return ptr;
}
```

## Performance Optimization

### SIMD Optimizations

```c
#ifdef __SSE2__
#include <emmintrin.h>

void generate_noise_simd(double* output, int count) {
    for (int i = 0; i < count; i += 2) {
        // Process two values at once using SSE2
        __m128d x = _mm_set_pd(i * 0.01, (i + 1) * 0.01);
        __m128d y = _mm_set_pd(0.0, 0.0);

        // Generate noise for both values
        double noise1 = simplex_noise_2d(i * 0.01, 0.0);
        double noise2 = simplex_noise_2d((i + 1) * 0.01, 0.0);

        output[i] = noise1;
        output[i + 1] = noise2;
    }
}
#endif
```

### Cache-Optimized Access Patterns

```c
void generate_noise_cache_friendly(double* output, int width, int height) {
    // Process in cache-friendly blocks
    const int block_size = 64;

    for (int block_y = 0; block_y < height; block_y += block_size) {
        for (int block_x = 0; block_x < width; block_x += block_size) {
            int end_y = (block_y + block_size < height) ? block_y + block_size : height;
            int end_x = (block_x + block_size < width) ? block_x + block_size : width;

            for (int y = block_y; y < end_y; y++) {
                for (int x = block_x; x < end_x; x++) {
                    output[y * width + x] = simplex_noise_2d(x * 0.01, y * 0.01);
                }
            }
        }
    }
}
```

### Lazy Evaluation

```c
typedef struct {
    double x, y;
    double result;
    int computed;
} lazy_noise_t;

double get_noise_lazy(lazy_noise_t* lazy, double x, double y) {
    if (!lazy->computed || lazy->x != x || lazy->y != y) {
        lazy->x = x;
        lazy->y = y;
        lazy->result = simplex_noise_2d(x, y);
        lazy->computed = 1;
    }
    return lazy->result;
}
```

## Advanced Noise Patterns

### Layered Noise

```c
double layered_noise(double x, double y, int layers) {
    double result = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;

    for (int i = 0; i < layers; i++) {
        result += simplex_noise_2d(x * frequency, y * frequency) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }

    return result;
}
```

### Noise Blending

```c
double blend_noise(double x, double y, double blend_factor) {
    double noise1 = simplex_noise_2d(x, y);
    double noise2 = simplex_noise_2d(x + 1000, y + 1000);

    // Smooth blending
    double t = (blend_factor + 1.0) * 0.5;  // Normalize to 0-1
    t = t * t * (3.0 - 2.0 * t);  // Smoothstep

    return noise1 * (1.0 - t) + noise2 * t;
}
```

### Noise Distortion

```c
double distorted_noise(double x, double y, double distortion_strength) {
    // Use noise to distort the input coordinates
    double distortion_x = simplex_noise_2d(x * 0.1, y * 0.1) * distortion_strength;
    double distortion_y = simplex_noise_2d(x * 0.1 + 1000, y * 0.1 + 1000) * distortion_strength;

    // Generate noise with distorted coordinates
    return simplex_noise_2d(x + distortion_x, y + distortion_y);
}
```

## Custom Noise Functions

### Wrapping Noise

```c
double wrapping_noise(double x, double y, double wrap_x, double wrap_y) {
    // Wrap coordinates to create seamless noise
    double wrapped_x = fmod(x, wrap_x);
    double wrapped_y = fmod(y, wrap_y);

    if (wrapped_x < 0) wrapped_x += wrap_x;
    if (wrapped_y < 0) wrapped_y += wrap_y;

    return simplex_noise_2d(wrapped_x, wrapped_y);
}
```

### Seamless Noise

```c
double seamless_noise(double x, double y, double period) {
    // Create seamless noise by blending edges
    double noise1 = simplex_noise_2d(x, y);
    double noise2 = simplex_noise_2d(x + period, y);
    double noise3 = simplex_noise_2d(x, y + period);
    double noise4 = simplex_noise_2d(x + period, y + period);

    // Bilinear interpolation for seamless blending
    double fx = fmod(x, period) / period;
    double fy = fmod(y, period) / period;

    double top = noise1 * (1.0 - fx) + noise2 * fx;
    double bottom = noise3 * (1.0 - fx) + noise4 * fx;

    return top * (1.0 - fy) + bottom * fy;
}
```

## Integration Patterns

### Plugin Architecture

```c
typedef struct {
    const char* name;
    double (*noise_func)(double x, double y);
    void* user_data;
} noise_plugin_t;

typedef struct {
    noise_plugin_t* plugins;
    int count;
    int capacity;
} plugin_manager_t;

plugin_manager_t* create_plugin_manager() {
    plugin_manager_t* manager = malloc(sizeof(plugin_manager_t));
    manager->plugins = malloc(10 * sizeof(noise_plugin_t));
    manager->count = 0;
    manager->capacity = 10;
    return manager;
}

void register_plugin(plugin_manager_t* manager, const char* name,
                    double (*noise_func)(double x, double y), void* user_data) {
    if (manager->count < manager->capacity) {
        manager->plugins[manager->count].name = name;
        manager->plugins[manager->count].noise_func = noise_func;
        manager->plugins[manager->count].user_data = user_data;
        manager->count++;
    }
}

double call_plugin(plugin_manager_t* manager, int index, double x, double y) {
    if (index >= 0 && index < manager->count) {
        return manager->plugins[index].noise_func(x, y);
    }
    return 0.0;
}
```

### Event System

```c
typedef struct {
    const char* event_name;
    void (*callback)(void* data);
    void* user_data;
} event_handler_t;

typedef struct {
    event_handler_t* handlers;
    int count;
    int capacity;
} event_system_t;

void emit_event(event_system_t* system, const char* event_name, void* data) {
    for (int i = 0; i < system->count; i++) {
        if (strcmp(system->handlers[i].event_name, event_name) == 0) {
            system->handlers[i].callback(data);
        }
    }
}

void on_noise_generated(void* data) {
    printf("Noise generation completed\n");
}

// Register event handler
event_system_t* event_system = create_event_system();
register_event_handler(event_system, "noise_generated", on_noise_generated, NULL);
```

## Debugging and Profiling

### Custom Profiling

```c
typedef struct {
    const char* name;
    clock_t start_time;
    clock_t total_time;
    int call_count;
} profiler_t;

profiler_t* create_profiler(const char* name) {
    profiler_t* profiler = malloc(sizeof(profiler_t));
    profiler->name = name;
    profiler->start_time = 0;
    profiler->total_time = 0;
    profiler->call_count = 0;
    return profiler;
}

void start_profiling(profiler_t* profiler) {
    profiler->start_time = clock();
}

void end_profiling(profiler_t* profiler) {
    profiler->total_time += clock() - profiler->start_time;
    profiler->call_count++;
}

void print_profiler_stats(profiler_t* profiler) {
    double avg_time = (double)profiler->total_time / profiler->call_count / CLOCKS_PER_SEC;
    printf("%s: %d calls, %.6f seconds total, %.6f seconds average\n",
           profiler->name, profiler->call_count,
           (double)profiler->total_time / CLOCKS_PER_SEC, avg_time);
}
```

### Memory Tracking

```c
typedef struct {
    size_t total_allocated;
    size_t peak_usage;
    int allocation_count;
} memory_tracker_t;

memory_tracker_t* create_memory_tracker() {
    memory_tracker_t* tracker = malloc(sizeof(memory_tracker_t));
    tracker->total_allocated = 0;
    tracker->peak_usage = 0;
    tracker->allocation_count = 0;
    return tracker;
}

void* tracked_malloc(memory_tracker_t* tracker, size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        tracker->total_allocated += size;
        tracker->allocation_count++;
        if (tracker->total_allocated > tracker->peak_usage) {
            tracker->peak_usage = tracker->total_allocated;
        }
    }
    return ptr;
}
```

## Best Practices

1. **Use appropriate data structures** - Choose the right structure for your use case
2. **Implement proper error handling** - Always check return values and handle errors
3. **Use memory pools** - For frequent allocations, use memory pools
4. **Profile your code** - Identify bottlenecks and optimize accordingly
5. **Use threading wisely** - Don't over-thread, consider overhead
6. **Cache frequently used data** - Avoid redundant calculations
7. **Use const where appropriate** - Mark data that won't change as const
8. **Document your code** - Write clear comments and documentation

---

_For more examples and patterns, see [Examples](examples.md)_
