/**
 * @file simplex_noise.c
 * @brief Pure C implementation of Simplex Noise algorithm
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 *
 * @details This implementation is based on Ken Perlin's improved noise algorithm
 * and provides 1D, 2D, 3D, and 4D simplex noise generation with fractal support.
 * The algorithm produces smooth, continuous noise values in the range [-1, 1]
 * and is commonly used for procedural generation in games and graphics.
 *
 * @author Adrian Paredez
 * @version 1.0
 * @date 9/5/2025
 */

#include "simplex_noise.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ===== GLOBAL STATE MANAGEMENT ===== */

// Global permutation table
static int perm[512];
static int initialized = 0;

// Advanced configuration
static simplex_config_t global_config;
static simplex_perf_stats_t perf_stats;

// PRNG state for different algorithms
static struct {
    uint32_t lcg_state;
    uint32_t mersenne_state[624];
    int mersenne_index;
    uint64_t xorshift_state[4];
    uint64_t pcg_state;
    uint64_t pcg_inc;
} prng_state;

// Caching system
#define CACHE_SIZE 1024
static struct {
    double x, y, z, w;
    double result;
    int valid;
} noise_cache[CACHE_SIZE];
static int cache_enabled = 0;
static int cache_hits = 0;
static int cache_misses = 0;

// Performance tracking
static int profiling_enabled = 0;
static size_t function_call_count = 0;

// Gradients for 2D noise
static const double grad2[8][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1},
                                   {1, 0}, {-1, 0}, {0, 1},  {0, -1}};

// Gradients for 3D noise
static const double grad3[12][3] = {{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
                                    {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
                                    {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}};

// Gradients for 4D noise
static const double grad4[32][4] = {{0, 1, 1, 1},  {0, 1, 1, -1},  {0, 1, -1, 1},  {0, 1, -1, -1},
                                    {0, -1, 1, 1}, {0, -1, 1, -1}, {0, -1, -1, 1}, {0, -1, -1, -1},
                                    {1, 0, 1, 1},  {1, 0, 1, -1},  {1, 0, -1, 1},  {1, 0, -1, -1},
                                    {-1, 0, 1, 1}, {-1, 0, 1, -1}, {-1, 0, -1, 1}, {-1, 0, -1, -1},
                                    {1, 1, 0, 1},  {1, 1, 0, -1},  {1, -1, 0, 1},  {1, -1, 0, -1},
                                    {-1, 1, 0, 1}, {-1, 1, 0, -1}, {-1, -1, 0, 1}, {-1, -1, 0, -1},
                                    {1, 1, 1, 0},  {1, 1, -1, 0},  {1, -1, 1, 0},  {1, -1, -1, 0},
                                    {-1, 1, 1, 0}, {-1, 1, -1, 0}, {-1, -1, 1, 0}, {-1, -1, -1, 0}};

/* ===== ADVANCED PRNG IMPLEMENTATIONS ===== */

// Linear Congruential Generator
static uint32_t lcg_next(void) {
    prng_state.lcg_state = prng_state.lcg_state * 1103515245 + 12345;
    return prng_state.lcg_state;
}

// Mersenne Twister implementation
static void mersenne_init(uint32_t seed) {
    prng_state.mersenne_state[0] = seed;
    for (int i = 1; i < 624; i++) {
        prng_state.mersenne_state[i] = (1812433253 * (prng_state.mersenne_state[i - 1] ^
                                                      (prng_state.mersenne_state[i - 1] >> 30)) +
                                        i) &
                                       0xFFFFFFFF;
    }
    prng_state.mersenne_index = 0;
}

static void mersenne_generate(void) {
    for (int i = 0; i < 624; i++) {
        uint32_t y = (prng_state.mersenne_state[i] & 0x80000000) +
                     (prng_state.mersenne_state[(i + 1) % 624] & 0x7FFFFFFF);
        prng_state.mersenne_state[i] = prng_state.mersenne_state[(i + 397) % 624] ^ (y >> 1);
        if (y % 2) {
            prng_state.mersenne_state[i] ^= 0x9908B0DF;
        }
    }
}

static uint32_t mersenne_next(void) {
    if (prng_state.mersenne_index == 0) {
        mersenne_generate();
    }
    uint32_t y = prng_state.mersenne_state[prng_state.mersenne_index];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9D2C5680;
    y ^= (y << 15) & 0xEFC60000;
    y ^= (y >> 18);
    prng_state.mersenne_index = (prng_state.mersenne_index + 1) % 624;
    return y;
}

// Xorshift implementation
static void xorshift_init(uint64_t seed) {
    prng_state.xorshift_state[0] = seed;
    prng_state.xorshift_state[1] = seed ^ 0x123456789ABCDEF0;
    prng_state.xorshift_state[2] = seed ^ 0xFEDCBA9876543210;
    prng_state.xorshift_state[3] = seed ^ 0x13579BDF2468ACE0;
}

static uint64_t xorshift_next(void) {
    uint64_t t = prng_state.xorshift_state[0];
    uint64_t s = prng_state.xorshift_state[3];
    prng_state.xorshift_state[0] = prng_state.xorshift_state[1];
    prng_state.xorshift_state[1] = prng_state.xorshift_state[2];
    prng_state.xorshift_state[2] = s;
    t ^= t << 11;
    t ^= t >> 8;
    prng_state.xorshift_state[3] = t ^ s ^ (s >> 19);
    return prng_state.xorshift_state[3];
}

// PCG implementation
static uint32_t pcg_next(void) {
    uint64_t oldstate = prng_state.pcg_state;
    prng_state.pcg_state = oldstate * 6364136223846793005ULL + prng_state.pcg_inc;
    uint32_t xorshifted = ((oldstate >> 18) ^ oldstate) >> 27;
    uint32_t rot = oldstate >> 59;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static void pcg_init(uint64_t seed) {
    prng_state.pcg_state = 0;
    prng_state.pcg_inc = (seed << 1) | 1;
    pcg_next();
    prng_state.pcg_state += seed;
    pcg_next();
}

// Unified PRNG interface
static uint32_t prng_next(void) {
    switch (global_config.prng_type) {
    case SIMPLEX_PRG_LINEAR_CONGRUENTIAL:
        return lcg_next();
    case SIMPLEX_PRG_MERSENNE_TWISTER:
        return mersenne_next();
    case SIMPLEX_PRG_XORSHIFT:
        return (uint32_t)xorshift_next();
    case SIMPLEX_PRG_PCG:
        return pcg_next();
    default:
        return lcg_next();
    }
}

static void prng_init(uint32_t seed) {
    switch (global_config.prng_type) {
    case SIMPLEX_PRG_LINEAR_CONGRUENTIAL:
        prng_state.lcg_state = seed;
        break;
    case SIMPLEX_PRG_MERSENNE_TWISTER:
        mersenne_init(seed);
        break;
    case SIMPLEX_PRG_XORSHIFT:
        xorshift_init(seed);
        break;
    case SIMPLEX_PRG_PCG:
        pcg_init(seed);
        break;
    default:
        prng_state.lcg_state = seed;
        break;
    }
}

/* ===== CONFIGURATION MANAGEMENT ===== */

simplex_config_t simplex_get_default_config(void) {
    simplex_config_t config = {.prng_type = SIMPLEX_PRG_PCG,
                               .noise_variant = SIMPLEX_NOISE_CLASSIC,
                               .interp_type = SIMPLEX_INTERP_SMOOTHSTEP,
                               .precision = SIMPLEX_PRECISION_DOUBLE,
                               .seed = 0,
                               .enable_simd = 0,
                               .enable_caching = 1,
                               .enable_profiling = 0,
                               .persistence = 0.5,
                               .lacunarity = 2.0,
                               .octaves = 4,
                               .frequency = 1.0,
                               .amplitude = 1.0,
                               .offset = 0.0,
                               .scale = 1.0,

                               /* Advanced Configuration Options */
                               .config_file = "",
                               .output_file = "",
                               .verbose_mode = 0,
                               .debug_mode = 0,
                               .auto_save = 0,
                               .validate_inputs = 1,
                               .cache_size_mb = 16.0,
                               .max_threads = 1,
                               .chunk_size = 1024,
                               .memory_limit_mb = 256.0};
    return config;
}

int simplex_noise_init_advanced(const simplex_config_t* config) {
    if (!config)
        return -1;

    global_config = *config;

    // Initialize PRNG
    if (global_config.seed == 0) {
        global_config.seed = (uint32_t)time(NULL);
    }
    prng_init(global_config.seed);

    // Initialize permutation table
    for (int i = 0; i < 256; i++) {
        perm[i] = i;
    }

    // Shuffle using selected PRNG
    for (int i = 255; i > 0; i--) {
        uint32_t j = prng_next() % (i + 1);
        int temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }

    // Duplicate for wrapping
    for (int i = 0; i < 256; i++) {
        perm[256 + i] = perm[i];
    }

    // Initialize performance stats
    memset(&perf_stats, 0, sizeof(perf_stats));
    function_call_count = 0;

    // Initialize cache
    if (global_config.enable_caching) {
        cache_enabled = 1;
        for (int i = 0; i < CACHE_SIZE; i++) {
            noise_cache[i].valid = 0;
        }
    }

    profiling_enabled = global_config.enable_profiling;
    initialized = 1;

    return 0;
}

int simplex_set_prng(simplex_prng_type_t prng_type) {
    if (prng_type >= SIMPLEX_PRG_COUNT)
        return -1;
    global_config.prng_type = prng_type;
    prng_init(global_config.seed);
    return 0;
}

int simplex_set_noise_variant(simplex_noise_variant_t variant) {
    if (variant >= SIMPLEX_NOISE_COUNT)
        return -1;
    global_config.noise_variant = variant;
    return 0;
}

int simplex_set_interpolation(simplex_interp_type_t interp_type) {
    if (interp_type >= SIMPLEX_INTERP_COUNT)
        return -1;
    global_config.interp_type = interp_type;
    return 0;
}

int simplex_set_simd(int enable) {
    global_config.enable_simd = enable ? 1 : 0;
    return 0;
}

int simplex_set_caching(int enable) {
    global_config.enable_caching = enable ? 1 : 0;
    cache_enabled = enable ? 1 : 0;
    return 0;
}

int simplex_set_profiling(int enable) {
    global_config.enable_profiling = enable ? 1 : 0;
    profiling_enabled = enable ? 1 : 0;
    return 0;
}

/* ===== CONFIGURATION FILE MANAGEMENT ===== */

// Helper function to trim whitespace
static void trim_whitespace(char* str) {
    char* start = str;
    char* end = str + strlen(str) - 1;

    while (isspace(*start))
        start++;
    while (end > start && isspace(*end))
        end--;

    *(end + 1) = '\0';
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

// Helper function to parse JSON-like key-value pairs
static int parse_key_value(const char* line, char* key, char* value, size_t max_len) {
    const char* eq = strchr(line, '=');
    if (!eq)
        return -1;

    size_t key_len = eq - line;
    size_t value_len = strlen(eq + 1);

    if (key_len >= max_len || value_len >= max_len)
        return -1;

    strncpy(key, line, key_len);
    key[key_len] = '\0';
    trim_whitespace(key);

    strncpy(value, eq + 1, value_len);
    value[value_len] = '\0';
    trim_whitespace(value);

    // Remove quotes if present
    if (value[0] == '"' && value[strlen(value) - 1] == '"') {
        memmove(value, value + 1, strlen(value) - 2);
        value[strlen(value) - 2] = '\0';
    }

    return 0;
}

// Load configuration from INI file
static int load_ini_config(const char* filename, simplex_config_t* config) {
    FILE* file = fopen(filename, "r");
    if (!file)
        return -1;

    char line[512];
    char key[256];
    char value[256];

    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);

        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == ';' || line[0] == '\0')
            continue;

        if (parse_key_value(line, key, value, sizeof(key)) == 0) {
            // Parse configuration values
            if (strcmp(key, "prng_type") == 0) {
                config->prng_type = (simplex_prng_type_t)atoi(value);
            } else if (strcmp(key, "noise_variant") == 0) {
                config->noise_variant = (simplex_noise_variant_t)atoi(value);
            } else if (strcmp(key, "interp_type") == 0) {
                config->interp_type = (simplex_interp_type_t)atoi(value);
            } else if (strcmp(key, "precision") == 0) {
                config->precision = (simplex_precision_t)atoi(value);
            } else if (strcmp(key, "seed") == 0) {
                config->seed = (uint32_t)atoll(value);
            } else if (strcmp(key, "enable_simd") == 0) {
                config->enable_simd = atoi(value);
            } else if (strcmp(key, "enable_caching") == 0) {
                config->enable_caching = atoi(value);
            } else if (strcmp(key, "enable_profiling") == 0) {
                config->enable_profiling = atoi(value);
            } else if (strcmp(key, "persistence") == 0) {
                config->persistence = atof(value);
            } else if (strcmp(key, "lacunarity") == 0) {
                config->lacunarity = atof(value);
            } else if (strcmp(key, "octaves") == 0) {
                config->octaves = atoi(value);
            } else if (strcmp(key, "frequency") == 0) {
                config->frequency = atof(value);
            } else if (strcmp(key, "amplitude") == 0) {
                config->amplitude = atof(value);
            } else if (strcmp(key, "offset") == 0) {
                config->offset = atof(value);
            } else if (strcmp(key, "scale") == 0) {
                config->scale = atof(value);
            } else if (strcmp(key, "config_file") == 0) {
                strncpy(config->config_file, value, sizeof(config->config_file) - 1);
                config->config_file[sizeof(config->config_file) - 1] = '\0';
            } else if (strcmp(key, "output_file") == 0) {
                strncpy(config->output_file, value, sizeof(config->output_file) - 1);
                config->output_file[sizeof(config->output_file) - 1] = '\0';
            } else if (strcmp(key, "verbose_mode") == 0) {
                config->verbose_mode = atoi(value);
            } else if (strcmp(key, "debug_mode") == 0) {
                config->debug_mode = atoi(value);
            } else if (strcmp(key, "auto_save") == 0) {
                config->auto_save = atoi(value);
            } else if (strcmp(key, "validate_inputs") == 0) {
                config->validate_inputs = atoi(value);
            } else if (strcmp(key, "cache_size_mb") == 0) {
                config->cache_size_mb = atof(value);
            } else if (strcmp(key, "max_threads") == 0) {
                config->max_threads = atoi(value);
            } else if (strcmp(key, "chunk_size") == 0) {
                config->chunk_size = atoi(value);
            } else if (strcmp(key, "memory_limit_mb") == 0) {
                config->memory_limit_mb = atof(value);
            }
        }
    }

    fclose(file);
    return 0;
}

// Save configuration to INI file
static int save_ini_config(const char* filename, const simplex_config_t* config) {
    FILE* file = fopen(filename, "w");
    if (!file)
        return -1;

    fprintf(file, "# Simplex Noise Configuration File\n");
    fprintf(file, "# Generated on %s\n\n", __DATE__ " " __TIME__);

    fprintf(file, "[core]\n");
    fprintf(file, "prng_type=%d\n", config->prng_type);
    fprintf(file, "noise_variant=%d\n", config->noise_variant);
    fprintf(file, "interp_type=%d\n", config->interp_type);
    fprintf(file, "precision=%d\n", config->precision);
    fprintf(file, "seed=%u\n", config->seed);

    fprintf(file, "\n[performance]\n");
    fprintf(file, "enable_simd=%d\n", config->enable_simd);
    fprintf(file, "enable_caching=%d\n", config->enable_caching);
    fprintf(file, "enable_profiling=%d\n", config->enable_profiling);
    fprintf(file, "cache_size_mb=%.2f\n", config->cache_size_mb);
    fprintf(file, "max_threads=%d\n", config->max_threads);
    fprintf(file, "chunk_size=%d\n", config->chunk_size);
    fprintf(file, "memory_limit_mb=%.2f\n", config->memory_limit_mb);

    fprintf(file, "\n[noise_parameters]\n");
    fprintf(file, "persistence=%.6f\n", config->persistence);
    fprintf(file, "lacunarity=%.6f\n", config->lacunarity);
    fprintf(file, "octaves=%d\n", config->octaves);
    fprintf(file, "frequency=%.6f\n", config->frequency);
    fprintf(file, "amplitude=%.6f\n", config->amplitude);
    fprintf(file, "offset=%.6f\n", config->offset);
    fprintf(file, "scale=%.6f\n", config->scale);

    fprintf(file, "\n[advanced]\n");
    fprintf(file, "config_file=\"%s\"\n", config->config_file);
    fprintf(file, "output_file=\"%s\"\n", config->output_file);
    fprintf(file, "verbose_mode=%d\n", config->verbose_mode);
    fprintf(file, "debug_mode=%d\n", config->debug_mode);
    fprintf(file, "auto_save=%d\n", config->auto_save);
    fprintf(file, "validate_inputs=%d\n", config->validate_inputs);

    fclose(file);
    return 0;
}

// Load configuration from JSON file (simplified parser)
static int load_json_config(const char* filename, simplex_config_t* config) {
    FILE* file = fopen(filename, "r");
    if (!file)
        return -1;

    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);

        // Skip comments and empty lines
        if (line[0] == '/' || line[0] == '*' || line[0] == '\0')
            continue;

        // Simple JSON key-value parser
        if (strstr(line, "\"prng_type\"") && strchr(line, ':')) {
            int temp;
            sscanf(line, "\"prng_type\" : %d", &temp);
            config->prng_type = (simplex_prng_type_t)temp;
        } else if (strstr(line, "\"noise_variant\"") && strchr(line, ':')) {
            int temp;
            sscanf(line, "\"noise_variant\" : %d", &temp);
            config->noise_variant = (simplex_noise_variant_t)temp;
        } else if (strstr(line, "\"interp_type\"") && strchr(line, ':')) {
            int temp;
            sscanf(line, "\"interp_type\" : %d", &temp);
            config->interp_type = (simplex_interp_type_t)temp;
        } else if (strstr(line, "\"precision\"") && strchr(line, ':')) {
            int temp;
            sscanf(line, "\"precision\" : %d", &temp);
            config->precision = (simplex_precision_t)temp;
        } else if (strstr(line, "\"seed\"") && strchr(line, ':')) {
            sscanf(line, "\"seed\" : %u", &config->seed);
        } else if (strstr(line, "\"enable_simd\"") && strchr(line, ':')) {
            sscanf(line, "\"enable_simd\" : %d", &config->enable_simd);
        } else if (strstr(line, "\"enable_caching\"") && strchr(line, ':')) {
            sscanf(line, "\"enable_caching\" : %d", &config->enable_caching);
        } else if (strstr(line, "\"enable_profiling\"") && strchr(line, ':')) {
            sscanf(line, "\"enable_profiling\" : %d", &config->enable_profiling);
        } else if (strstr(line, "\"persistence\"") && strchr(line, ':')) {
            sscanf(line, "\"persistence\" : %lf", &config->persistence);
        } else if (strstr(line, "\"lacunarity\"") && strchr(line, ':')) {
            sscanf(line, "\"lacunarity\" : %lf", &config->lacunarity);
        } else if (strstr(line, "\"octaves\"") && strchr(line, ':')) {
            sscanf(line, "\"octaves\" : %d", &config->octaves);
        } else if (strstr(line, "\"frequency\"") && strchr(line, ':')) {
            sscanf(line, "\"frequency\" : %lf", &config->frequency);
        } else if (strstr(line, "\"amplitude\"") && strchr(line, ':')) {
            sscanf(line, "\"amplitude\" : %lf", &config->amplitude);
        } else if (strstr(line, "\"offset\"") && strchr(line, ':')) {
            sscanf(line, "\"offset\" : %lf", &config->offset);
        } else if (strstr(line, "\"scale\"") && strchr(line, ':')) {
            sscanf(line, "\"scale\" : %lf", &config->scale);
        } else if (strstr(line, "\"config_file\"") && strchr(line, ':')) {
            sscanf(line, "\"config_file\" : \"%255[^\"]\"", config->config_file);
        } else if (strstr(line, "\"output_file\"") && strchr(line, ':')) {
            sscanf(line, "\"output_file\" : \"%255[^\"]\"", config->output_file);
        } else if (strstr(line, "\"verbose_mode\"") && strchr(line, ':')) {
            sscanf(line, "\"verbose_mode\" : %d", &config->verbose_mode);
        } else if (strstr(line, "\"debug_mode\"") && strchr(line, ':')) {
            sscanf(line, "\"debug_mode\" : %d", &config->debug_mode);
        } else if (strstr(line, "\"auto_save\"") && strchr(line, ':')) {
            sscanf(line, "\"auto_save\" : %d", &config->auto_save);
        } else if (strstr(line, "\"validate_inputs\"") && strchr(line, ':')) {
            sscanf(line, "\"validate_inputs\" : %d", &config->validate_inputs);
        } else if (strstr(line, "\"cache_size_mb\"") && strchr(line, ':')) {
            sscanf(line, "\"cache_size_mb\" : %lf", &config->cache_size_mb);
        } else if (strstr(line, "\"max_threads\"") && strchr(line, ':')) {
            sscanf(line, "\"max_threads\" : %d", &config->max_threads);
        } else if (strstr(line, "\"chunk_size\"") && strchr(line, ':')) {
            sscanf(line, "\"chunk_size\" : %d", &config->chunk_size);
        } else if (strstr(line, "\"memory_limit_mb\"") && strchr(line, ':')) {
            sscanf(line, "\"memory_limit_mb\" : %lf", &config->memory_limit_mb);
        }
    }

    fclose(file);
    return 0;
}

// Save configuration to JSON file
static int save_json_config(const char* filename, const simplex_config_t* config) {
    FILE* file = fopen(filename, "w");
    if (!file)
        return -1;

    fprintf(file, "{\n");
    fprintf(file, "  \"simplex_noise_config\": {\n");
    fprintf(file, "    \"core\": {\n");
    fprintf(file, "      \"prng_type\": %d,\n", config->prng_type);
    fprintf(file, "      \"noise_variant\": %d,\n", config->noise_variant);
    fprintf(file, "      \"interp_type\": %d,\n", config->interp_type);
    fprintf(file, "      \"precision\": %d,\n", config->precision);
    fprintf(file, "      \"seed\": %u\n", config->seed);
    fprintf(file, "    },\n");
    fprintf(file, "    \"performance\": {\n");
    fprintf(file, "      \"enable_simd\": %d,\n", config->enable_simd);
    fprintf(file, "      \"enable_caching\": %d,\n", config->enable_caching);
    fprintf(file, "      \"enable_profiling\": %d,\n", config->enable_profiling);
    fprintf(file, "      \"cache_size_mb\": %.2f,\n", config->cache_size_mb);
    fprintf(file, "      \"max_threads\": %d,\n", config->max_threads);
    fprintf(file, "      \"chunk_size\": %d,\n", config->chunk_size);
    fprintf(file, "      \"memory_limit_mb\": %.2f\n", config->memory_limit_mb);
    fprintf(file, "    },\n");
    fprintf(file, "    \"noise_parameters\": {\n");
    fprintf(file, "      \"persistence\": %.6f,\n", config->persistence);
    fprintf(file, "      \"lacunarity\": %.6f,\n", config->lacunarity);
    fprintf(file, "      \"octaves\": %d,\n", config->octaves);
    fprintf(file, "      \"frequency\": %.6f,\n", config->frequency);
    fprintf(file, "      \"amplitude\": %.6f,\n", config->amplitude);
    fprintf(file, "      \"offset\": %.6f,\n", config->offset);
    fprintf(file, "      \"scale\": %.6f\n", config->scale);
    fprintf(file, "    },\n");
    fprintf(file, "    \"advanced\": {\n");
    fprintf(file, "      \"config_file\": \"%s\",\n", config->config_file);
    fprintf(file, "      \"output_file\": \"%s\",\n", config->output_file);
    fprintf(file, "      \"verbose_mode\": %d,\n", config->verbose_mode);
    fprintf(file, "      \"debug_mode\": %d,\n", config->debug_mode);
    fprintf(file, "      \"auto_save\": %d,\n", config->auto_save);
    fprintf(file, "      \"validate_inputs\": %d\n", config->validate_inputs);
    fprintf(file, "    }\n");
    fprintf(file, "  }\n");
    fprintf(file, "}\n");

    fclose(file);
    return 0;
}

// Public configuration management functions
int simplex_load_config(const char* filename, simplex_config_type_t config_type,
                        simplex_config_t* config) {
    if (!filename || !config)
        return -1;

    // Start with default configuration
    *config = simplex_get_default_config();

    switch (config_type) {
    case SIMPLEX_CONFIG_INI:
        return load_ini_config(filename, config);
    case SIMPLEX_CONFIG_JSON:
        return load_json_config(filename, config);
    case SIMPLEX_CONFIG_YAML:
        // YAML support would go here
        return -1;  // Not implemented yet
    case SIMPLEX_CONFIG_BINARY:
        // Binary support would go here
        return -1;  // Not implemented yet
    default:
        return -1;
    }
}

int simplex_save_config(const char* filename, simplex_config_type_t config_type,
                        const simplex_config_t* config) {
    if (!filename || !config)
        return -1;

    switch (config_type) {
    case SIMPLEX_CONFIG_INI:
        return save_ini_config(filename, config);
    case SIMPLEX_CONFIG_JSON:
        return save_json_config(filename, config);
    case SIMPLEX_CONFIG_YAML:
        // YAML support would go here
        return -1;  // Not implemented yet
    case SIMPLEX_CONFIG_BINARY:
        // Binary support would go here
        return -1;  // Not implemented yet
    default:
        return -1;
    }
}

int simplex_validate_config(const simplex_config_t* config,
                            simplex_config_validation_t* validation) {
    if (!config || !validation)
        return -1;

    memset(validation, 0, sizeof(simplex_config_validation_t));
    validation->valid = 1;

    // Validate PRNG type
    if (config->prng_type >= SIMPLEX_PRG_COUNT) {
        strcpy(validation->errors[validation->error_count], "Invalid PRNG type");
        validation->error_count++;
        validation->valid = 0;
    }

    // Validate noise variant
    if (config->noise_variant >= SIMPLEX_NOISE_COUNT) {
        strcpy(validation->errors[validation->error_count], "Invalid noise variant");
        validation->error_count++;
        validation->valid = 0;
    }

    // Validate interpolation type
    if (config->interp_type >= SIMPLEX_INTERP_COUNT) {
        strcpy(validation->errors[validation->error_count], "Invalid interpolation type");
        validation->error_count++;
        validation->valid = 0;
    }

    // Validate precision
    if (config->precision >= SIMPLEX_PRECISION_COUNT) {
        strcpy(validation->errors[validation->error_count], "Invalid precision type");
        validation->error_count++;
        validation->valid = 0;
    }

    // Validate octaves
    if (config->octaves < 1 || config->octaves > 16) {
        strcpy(validation->errors[validation->error_count], "Octaves must be between 1 and 16");
        validation->error_count++;
        validation->valid = 0;
    }

    // Validate persistence
    if (config->persistence < 0.0 || config->persistence > 1.0) {
        strcpy(validation->warnings[validation->warning_count],
               "Persistence should be between 0.0 and 1.0");
        validation->warning_count++;
    }

    // Validate lacunarity
    if (config->lacunarity < 1.0 || config->lacunarity > 4.0) {
        strcpy(validation->warnings[validation->warning_count],
               "Lacunarity should be between 1.0 and 4.0");
        validation->warning_count++;
    }

    // Validate cache size
    if (config->cache_size_mb < 0.0 || config->cache_size_mb > 1024.0) {
        strcpy(validation->warnings[validation->warning_count],
               "Cache size should be between 0.0 and 1024.0 MB");
        validation->warning_count++;
    }

    // Validate max threads
    if (config->max_threads < 1 || config->max_threads > 64) {
        strcpy(validation->warnings[validation->warning_count],
               "Max threads should be between 1 and 64");
        validation->warning_count++;
    }

    return 0;
}

int simplex_reset_config(simplex_config_t* config) {
    if (!config)
        return -1;
    *config = simplex_get_default_config();
    return 0;
}

int simplex_merge_config(const simplex_config_t* base, const simplex_config_t* override,
                         simplex_config_t* result) {
    if (!base || !override || !result)
        return -1;

    *result = *base;

    // Override with non-default values from override config
    if (override->prng_type != SIMPLEX_PRG_PCG)
        result->prng_type = override->prng_type;
    if (override->noise_variant != SIMPLEX_NOISE_CLASSIC)
        result->noise_variant = override->noise_variant;
    if (override->interp_type != SIMPLEX_INTERP_SMOOTHSTEP)
        result->interp_type = override->interp_type;
    if (override->precision != SIMPLEX_PRECISION_DOUBLE)
        result->precision = override->precision;
    if (override->seed != 0)
        result->seed = override->seed;
    if (override->enable_simd != 0)
        result->enable_simd = override->enable_simd;
    if (override->enable_caching != 1)
        result->enable_caching = override->enable_caching;
    if (override->enable_profiling != 0)
        result->enable_profiling = override->enable_profiling;
    if (override->persistence != 0.5)
        result->persistence = override->persistence;
    if (override->lacunarity != 2.0)
        result->lacunarity = override->lacunarity;
    if (override->octaves != 4)
        result->octaves = override->octaves;
    if (override->frequency != 1.0)
        result->frequency = override->frequency;
    if (override->amplitude != 1.0)
        result->amplitude = override->amplitude;
    if (override->offset != 0.0)
        result->offset = override->offset;
    if (override->scale != 1.0)
        result->scale = override->scale;

    // String fields
    if (strlen(override->config_file) > 0)
        strcpy(result->config_file, override->config_file);
    if (strlen(override->output_file) > 0)
        strcpy(result->output_file, override->output_file);

    // Advanced options
    if (override->verbose_mode != 0)
        result->verbose_mode = override->verbose_mode;
    if (override->debug_mode != 0)
        result->debug_mode = override->debug_mode;
    if (override->auto_save != 0)
        result->auto_save = override->auto_save;
    if (override->validate_inputs != 1)
        result->validate_inputs = override->validate_inputs;
    if (override->cache_size_mb != 16.0)
        result->cache_size_mb = override->cache_size_mb;
    if (override->max_threads != 1)
        result->max_threads = override->max_threads;
    if (override->chunk_size != 1024)
        result->chunk_size = override->chunk_size;
    if (override->memory_limit_mb != 256.0)
        result->memory_limit_mb = override->memory_limit_mb;

    return 0;
}

int simplex_get_config_string(const simplex_config_t* config, const char* key, char* value,
                              size_t max_len) {
    if (!config || !key || !value || max_len == 0)
        return -1;

    if (strcmp(key, "prng_type") == 0) {
        snprintf(value, max_len, "%d", config->prng_type);
    } else if (strcmp(key, "noise_variant") == 0) {
        snprintf(value, max_len, "%d", config->noise_variant);
    } else if (strcmp(key, "interp_type") == 0) {
        snprintf(value, max_len, "%d", config->interp_type);
    } else if (strcmp(key, "precision") == 0) {
        snprintf(value, max_len, "%d", config->precision);
    } else if (strcmp(key, "seed") == 0) {
        snprintf(value, max_len, "%u", config->seed);
    } else if (strcmp(key, "enable_simd") == 0) {
        snprintf(value, max_len, "%d", config->enable_simd);
    } else if (strcmp(key, "enable_caching") == 0) {
        snprintf(value, max_len, "%d", config->enable_caching);
    } else if (strcmp(key, "enable_profiling") == 0) {
        snprintf(value, max_len, "%d", config->enable_profiling);
    } else if (strcmp(key, "persistence") == 0) {
        snprintf(value, max_len, "%.6f", config->persistence);
    } else if (strcmp(key, "lacunarity") == 0) {
        snprintf(value, max_len, "%.6f", config->lacunarity);
    } else if (strcmp(key, "octaves") == 0) {
        snprintf(value, max_len, "%d", config->octaves);
    } else if (strcmp(key, "frequency") == 0) {
        snprintf(value, max_len, "%.6f", config->frequency);
    } else if (strcmp(key, "amplitude") == 0) {
        snprintf(value, max_len, "%.6f", config->amplitude);
    } else if (strcmp(key, "offset") == 0) {
        snprintf(value, max_len, "%.6f", config->offset);
    } else if (strcmp(key, "scale") == 0) {
        snprintf(value, max_len, "%.6f", config->scale);
    } else if (strcmp(key, "config_file") == 0) {
        snprintf(value, max_len, "%s", config->config_file);
    } else if (strcmp(key, "output_file") == 0) {
        snprintf(value, max_len, "%s", config->output_file);
    } else if (strcmp(key, "verbose_mode") == 0) {
        snprintf(value, max_len, "%d", config->verbose_mode);
    } else if (strcmp(key, "debug_mode") == 0) {
        snprintf(value, max_len, "%d", config->debug_mode);
    } else if (strcmp(key, "auto_save") == 0) {
        snprintf(value, max_len, "%d", config->auto_save);
    } else if (strcmp(key, "validate_inputs") == 0) {
        snprintf(value, max_len, "%d", config->validate_inputs);
    } else if (strcmp(key, "cache_size_mb") == 0) {
        snprintf(value, max_len, "%.2f", config->cache_size_mb);
    } else if (strcmp(key, "max_threads") == 0) {
        snprintf(value, max_len, "%d", config->max_threads);
    } else if (strcmp(key, "chunk_size") == 0) {
        snprintf(value, max_len, "%d", config->chunk_size);
    } else if (strcmp(key, "memory_limit_mb") == 0) {
        snprintf(value, max_len, "%.2f", config->memory_limit_mb);
    } else {
        return -1;  // Unknown key
    }

    return 0;
}

int simplex_set_config_string(simplex_config_t* config, const char* key, const char* value) {
    if (!config || !key || !value)
        return -1;

    if (strcmp(key, "prng_type") == 0) {
        config->prng_type = (simplex_prng_type_t)atoi(value);
    } else if (strcmp(key, "noise_variant") == 0) {
        config->noise_variant = (simplex_noise_variant_t)atoi(value);
    } else if (strcmp(key, "interp_type") == 0) {
        config->interp_type = (simplex_interp_type_t)atoi(value);
    } else if (strcmp(key, "precision") == 0) {
        config->precision = (simplex_precision_t)atoi(value);
    } else if (strcmp(key, "seed") == 0) {
        config->seed = (uint32_t)atoll(value);
    } else if (strcmp(key, "enable_simd") == 0) {
        config->enable_simd = atoi(value);
    } else if (strcmp(key, "enable_caching") == 0) {
        config->enable_caching = atoi(value);
    } else if (strcmp(key, "enable_profiling") == 0) {
        config->enable_profiling = atoi(value);
    } else if (strcmp(key, "persistence") == 0) {
        config->persistence = atof(value);
    } else if (strcmp(key, "lacunarity") == 0) {
        config->lacunarity = atof(value);
    } else if (strcmp(key, "octaves") == 0) {
        config->octaves = atoi(value);
    } else if (strcmp(key, "frequency") == 0) {
        config->frequency = atof(value);
    } else if (strcmp(key, "amplitude") == 0) {
        config->amplitude = atof(value);
    } else if (strcmp(key, "offset") == 0) {
        config->offset = atof(value);
    } else if (strcmp(key, "scale") == 0) {
        config->scale = atof(value);
    } else if (strcmp(key, "config_file") == 0) {
        strncpy(config->config_file, value, sizeof(config->config_file) - 1);
        config->config_file[sizeof(config->config_file) - 1] = '\0';
    } else if (strcmp(key, "output_file") == 0) {
        strncpy(config->output_file, value, sizeof(config->output_file) - 1);
        config->output_file[sizeof(config->output_file) - 1] = '\0';
    } else if (strcmp(key, "verbose_mode") == 0) {
        config->verbose_mode = atoi(value);
    } else if (strcmp(key, "debug_mode") == 0) {
        config->debug_mode = atoi(value);
    } else if (strcmp(key, "auto_save") == 0) {
        config->auto_save = atoi(value);
    } else if (strcmp(key, "validate_inputs") == 0) {
        config->validate_inputs = atoi(value);
    } else if (strcmp(key, "cache_size_mb") == 0) {
        config->cache_size_mb = atof(value);
    } else if (strcmp(key, "max_threads") == 0) {
        config->max_threads = atoi(value);
    } else if (strcmp(key, "chunk_size") == 0) {
        config->chunk_size = atoi(value);
    } else if (strcmp(key, "memory_limit_mb") == 0) {
        config->memory_limit_mb = atof(value);
    } else {
        return -1;  // Unknown key
    }

    return 0;
}

int simplex_get_config_double(const simplex_config_t* config, const char* key, double* value) {
    if (!config || !key || !value)
        return -1;

    if (strcmp(key, "persistence") == 0) {
        *value = config->persistence;
    } else if (strcmp(key, "lacunarity") == 0) {
        *value = config->lacunarity;
    } else if (strcmp(key, "frequency") == 0) {
        *value = config->frequency;
    } else if (strcmp(key, "amplitude") == 0) {
        *value = config->amplitude;
    } else if (strcmp(key, "offset") == 0) {
        *value = config->offset;
    } else if (strcmp(key, "scale") == 0) {
        *value = config->scale;
    } else if (strcmp(key, "cache_size_mb") == 0) {
        *value = config->cache_size_mb;
    } else if (strcmp(key, "memory_limit_mb") == 0) {
        *value = config->memory_limit_mb;
    } else {
        return -1;  // Unknown key
    }

    return 0;
}

int simplex_set_config_double(simplex_config_t* config, const char* key, double value) {
    if (!config || !key)
        return -1;

    if (strcmp(key, "persistence") == 0) {
        config->persistence = value;
    } else if (strcmp(key, "lacunarity") == 0) {
        config->lacunarity = value;
    } else if (strcmp(key, "frequency") == 0) {
        config->frequency = value;
    } else if (strcmp(key, "amplitude") == 0) {
        config->amplitude = value;
    } else if (strcmp(key, "offset") == 0) {
        config->offset = value;
    } else if (strcmp(key, "scale") == 0) {
        config->scale = value;
    } else if (strcmp(key, "cache_size_mb") == 0) {
        config->cache_size_mb = value;
    } else if (strcmp(key, "memory_limit_mb") == 0) {
        config->memory_limit_mb = value;
    } else {
        return -1;  // Unknown key
    }

    return 0;
}

int simplex_get_config_int(const simplex_config_t* config, const char* key, int* value) {
    if (!config || !key || !value)
        return -1;

    if (strcmp(key, "prng_type") == 0) {
        *value = config->prng_type;
    } else if (strcmp(key, "noise_variant") == 0) {
        *value = config->noise_variant;
    } else if (strcmp(key, "interp_type") == 0) {
        *value = config->interp_type;
    } else if (strcmp(key, "precision") == 0) {
        *value = config->precision;
    } else if (strcmp(key, "seed") == 0) {
        *value = (int)config->seed;
    } else if (strcmp(key, "enable_simd") == 0) {
        *value = config->enable_simd;
    } else if (strcmp(key, "enable_caching") == 0) {
        *value = config->enable_caching;
    } else if (strcmp(key, "enable_profiling") == 0) {
        *value = config->enable_profiling;
    } else if (strcmp(key, "octaves") == 0) {
        *value = config->octaves;
    } else if (strcmp(key, "verbose_mode") == 0) {
        *value = config->verbose_mode;
    } else if (strcmp(key, "debug_mode") == 0) {
        *value = config->debug_mode;
    } else if (strcmp(key, "auto_save") == 0) {
        *value = config->auto_save;
    } else if (strcmp(key, "validate_inputs") == 0) {
        *value = config->validate_inputs;
    } else if (strcmp(key, "max_threads") == 0) {
        *value = config->max_threads;
    } else if (strcmp(key, "chunk_size") == 0) {
        *value = config->chunk_size;
    } else {
        return -1;  // Unknown key
    }

    return 0;
}

int simplex_set_config_int(simplex_config_t* config, const char* key, int value) {
    if (!config || !key)
        return -1;

    if (strcmp(key, "prng_type") == 0) {
        config->prng_type = (simplex_prng_type_t)value;
    } else if (strcmp(key, "noise_variant") == 0) {
        config->noise_variant = (simplex_noise_variant_t)value;
    } else if (strcmp(key, "interp_type") == 0) {
        config->interp_type = (simplex_interp_type_t)value;
    } else if (strcmp(key, "precision") == 0) {
        config->precision = (simplex_precision_t)value;
    } else if (strcmp(key, "seed") == 0) {
        config->seed = (uint32_t)value;
    } else if (strcmp(key, "enable_simd") == 0) {
        config->enable_simd = value;
    } else if (strcmp(key, "enable_caching") == 0) {
        config->enable_caching = value;
    } else if (strcmp(key, "enable_profiling") == 0) {
        config->enable_profiling = value;
    } else if (strcmp(key, "octaves") == 0) {
        config->octaves = value;
    } else if (strcmp(key, "verbose_mode") == 0) {
        config->verbose_mode = value;
    } else if (strcmp(key, "debug_mode") == 0) {
        config->debug_mode = value;
    } else if (strcmp(key, "auto_save") == 0) {
        config->auto_save = value;
    } else if (strcmp(key, "validate_inputs") == 0) {
        config->validate_inputs = value;
    } else if (strcmp(key, "max_threads") == 0) {
        config->max_threads = value;
    } else if (strcmp(key, "chunk_size") == 0) {
        config->chunk_size = value;
    } else {
        return -1;  // Unknown key
    }

    return 0;
}

int simplex_print_config(const simplex_config_t* config, int format) {
    if (!config)
        return -1;

    if (format == 0) {  // Compact format
        printf("Simplex Noise Configuration:\n");
        printf("  PRNG: %d, Variant: %d, Interp: %d, Precision: %d\n", config->prng_type,
               config->noise_variant, config->interp_type, config->precision);
        printf("  Seed: %u, SIMD: %d, Cache: %d, Profile: %d\n", config->seed, config->enable_simd,
               config->enable_caching, config->enable_profiling);
        printf("  Persistence: %.3f, Lacunarity: %.3f, Octaves: %d\n", config->persistence,
               config->lacunarity, config->octaves);
        printf("  Frequency: %.3f, Amplitude: %.3f, Offset: %.3f, Scale: %.3f\n", config->frequency,
               config->amplitude, config->offset, config->scale);
    } else if (format == 1) {  // Verbose format
        printf("=== Simplex Noise Configuration ===\n");
        printf("Core Settings:\n");
        printf("  PRNG Type: %d\n", config->prng_type);
        printf("  Noise Variant: %d\n", config->noise_variant);
        printf("  Interpolation Type: %d\n", config->interp_type);
        printf("  Precision: %d\n", config->precision);
        printf("  Seed: %u\n", config->seed);
        printf("\nPerformance Settings:\n");
        printf("  SIMD Enabled: %d\n", config->enable_simd);
        printf("  Caching Enabled: %d\n", config->enable_caching);
        printf("  Profiling Enabled: %d\n", config->enable_profiling);
        printf("  Cache Size: %.2f MB\n", config->cache_size_mb);
        printf("  Max Threads: %d\n", config->max_threads);
        printf("  Chunk Size: %d\n", config->chunk_size);
        printf("  Memory Limit: %.2f MB\n", config->memory_limit_mb);
        printf("\nNoise Parameters:\n");
        printf("  Persistence: %.6f\n", config->persistence);
        printf("  Lacunarity: %.6f\n", config->lacunarity);
        printf("  Octaves: %d\n", config->octaves);
        printf("  Frequency: %.6f\n", config->frequency);
        printf("  Amplitude: %.6f\n", config->amplitude);
        printf("  Offset: %.6f\n", config->offset);
        printf("  Scale: %.6f\n", config->scale);
        printf("\nAdvanced Settings:\n");
        printf("  Config File: %s\n", config->config_file);
        printf("  Output File: %s\n", config->output_file);
        printf("  Verbose Mode: %d\n", config->verbose_mode);
        printf("  Debug Mode: %d\n", config->debug_mode);
        printf("  Auto Save: %d\n", config->auto_save);
        printf("  Validate Inputs: %d\n", config->validate_inputs);
    } else if (format == 2) {  // JSON format
        printf("{\n");
        printf("  \"simplex_noise_config\": {\n");
        printf("    \"core\": {\n");
        printf("      \"prng_type\": %d,\n", config->prng_type);
        printf("      \"noise_variant\": %d,\n", config->noise_variant);
        printf("      \"interp_type\": %d,\n", config->interp_type);
        printf("      \"precision\": %d,\n", config->precision);
        printf("      \"seed\": %u\n", config->seed);
        printf("    },\n");
        printf("    \"performance\": {\n");
        printf("      \"enable_simd\": %d,\n", config->enable_simd);
        printf("      \"enable_caching\": %d,\n", config->enable_caching);
        printf("      \"enable_profiling\": %d,\n", config->enable_profiling);
        printf("      \"cache_size_mb\": %.2f,\n", config->cache_size_mb);
        printf("      \"max_threads\": %d,\n", config->max_threads);
        printf("      \"chunk_size\": %d,\n", config->chunk_size);
        printf("      \"memory_limit_mb\": %.2f\n", config->memory_limit_mb);
        printf("    },\n");
        printf("    \"noise_parameters\": {\n");
        printf("      \"persistence\": %.6f,\n", config->persistence);
        printf("      \"lacunarity\": %.6f,\n", config->lacunarity);
        printf("      \"octaves\": %d,\n", config->octaves);
        printf("      \"frequency\": %.6f,\n", config->frequency);
        printf("      \"amplitude\": %.6f,\n", config->amplitude);
        printf("      \"offset\": %.6f,\n", config->offset);
        printf("      \"scale\": %.6f\n", config->scale);
        printf("    },\n");
        printf("    \"advanced\": {\n");
        printf("      \"config_file\": \"%s\",\n", config->config_file);
        printf("      \"output_file\": \"%s\",\n", config->output_file);
        printf("      \"verbose_mode\": %d,\n", config->verbose_mode);
        printf("      \"debug_mode\": %d,\n", config->debug_mode);
        printf("      \"auto_save\": %d,\n", config->auto_save);
        printf("      \"validate_inputs\": %d\n", config->validate_inputs);
        printf("    }\n");
        printf("  }\n");
        printf("}\n");
    }

    return 0;
}

int simplex_create_example_config(const char* filename, simplex_config_type_t config_type) {
    if (!filename)
        return -1;

    simplex_config_t config = simplex_get_default_config();
    return simplex_save_config(filename, config_type, &config);
}

/* ===== PERFORMANCE TRACKING ===== */

int simplex_get_performance_stats(simplex_perf_stats_t* stats) {
    if (!stats)
        return -1;
    *stats = perf_stats;
    return 0;
}

void simplex_reset_performance_stats(void) {
    memset(&perf_stats, 0, sizeof(perf_stats));
    function_call_count = 0;
    cache_hits = 0;
    cache_misses = 0;
}

size_t simplex_get_function_call_count(void) {
    return function_call_count;
}

int simplex_get_cache_hits(void) {
    return cache_hits;
}

int simplex_get_cache_misses(void) {
    return cache_misses;
}

/* ===== CACHING SYSTEM ===== */
/* Note: These functions are part of the advanced API and are intentionally
 * unused in the current implementation. They provide caching capabilities
 * for future performance optimizations. */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static double cache_lookup(double x, double y, double z, double w) {
    if (!cache_enabled)
        return 0.0;

    int hash = ((int)(x * 1000) ^ (int)(y * 1000) ^ (int)(z * 1000) ^ (int)(w * 1000)) % CACHE_SIZE;
    if (hash < 0)
        hash = -hash;

    if (noise_cache[hash].valid && fabs(noise_cache[hash].x - x) < 1e-9 &&
        fabs(noise_cache[hash].y - y) < 1e-9 && fabs(noise_cache[hash].z - z) < 1e-9 &&
        fabs(noise_cache[hash].w - w) < 1e-9) {
        cache_hits++;
        return noise_cache[hash].result;
    }

    cache_misses++;
    return 0.0;
}

static void cache_store(double x, double y, double z, double w, double result) {
    if (!cache_enabled)
        return;

    int hash = ((int)(x * 1000) ^ (int)(y * 1000) ^ (int)(z * 1000) ^ (int)(w * 1000)) % CACHE_SIZE;
    if (hash < 0)
        hash = -hash;

    noise_cache[hash].x = x;
    noise_cache[hash].y = y;
    noise_cache[hash].z = z;
    noise_cache[hash].w = w;
    noise_cache[hash].result = result;
    noise_cache[hash].valid = 1;
}

/* ===== ADVANCED INTERPOLATION ===== */
/* Note: These functions are part of the advanced API and are intentionally
 * unused in the current implementation. They provide advanced interpolation
 * capabilities for future enhancements. */

static double interpolate(double t, simplex_interp_type_t type) {
    switch (type) {
    case SIMPLEX_INTERP_LINEAR:
        return t;
    case SIMPLEX_INTERP_CUBIC:
        return t * t * (3.0 - 2.0 * t);
    case SIMPLEX_INTERP_HERMITE:
        return t * t * (2.0 * t - 3.0) + 1.0;
    case SIMPLEX_INTERP_SMOOTHSTEP:
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    default:
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    }
}

// Helper functions
static double dot2d(const double grad[2], double x, double y) {
    return grad[0] * x + grad[1] * y;
}

static double dot3d(const double grad[3], double x, double y, double z) {
    return grad[0] * x + grad[1] * y + grad[2] * z;
}

static double dot4d(const double grad[4], double x, double y, double z, double w) {
    return grad[0] * x + grad[1] * y + grad[2] * z + grad[3] * w;
}

static int fast_floor(double x) {
    return x > 0 ? (int)x : (int)x - 1;
}

/* Note: This function is part of the advanced API and is intentionally
 * unused in the current implementation. It provides advanced permutation
 * initialization for future enhancements. */
static void init_permutation(unsigned int seed) {
    if (seed == 0) {
        seed = (unsigned int)time(NULL);
    }

    // Initialize with values 0-255
    for (int i = 0; i < 256; i++) {
        perm[i] = i;
    }

    // Simple linear congruential generator for shuffling
    for (int i = 255; i > 0; i--) {
        seed = seed * 1103515245 + 12345;
        int j = seed % (i + 1);
        int temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }

    // Duplicate the permutation table for wrapping
    for (int i = 0; i < 256; i++) {
        perm[256 + i] = perm[i];
    }
}

void simplex_noise_init(unsigned int seed) {
    // Initialize with default configuration
    simplex_config_t config = simplex_get_default_config();
    config.seed = seed;
    simplex_noise_init_advanced(&config);
}

/* ===== ADVANCED NOISE VARIANTS ===== */

// Ridged noise implementation
double simplex_ridged_1d(double x) {
    double noise = simplex_noise_1d(x);
    return 1.0 - fabs(noise);
}

double simplex_ridged_2d(double x, double y) {
    double noise = simplex_noise_2d(x, y);
    return 1.0 - fabs(noise);
}

double simplex_ridged_3d(double x, double y, double z) {
    double noise = simplex_noise_3d(x, y, z);
    return 1.0 - fabs(noise);
}

// Billowy noise implementation
double simplex_billowy_1d(double x) {
    double noise = simplex_noise_1d(x);
    return fabs(noise);
}

double simplex_billowy_2d(double x, double y) {
    double noise = simplex_noise_2d(x, y);
    return fabs(noise);
}

double simplex_billowy_3d(double x, double y, double z) {
    double noise = simplex_noise_3d(x, y, z);
    return fabs(noise);
}

// Fractional Brownian Motion (fBm)
double simplex_fbm_2d(double x, double y, int octaves, double persistence, double lacunarity) {
    double value = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += simplex_noise_2d(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / maxValue;
}

double simplex_fbm_3d(double x, double y, double z, int octaves, double persistence,
                      double lacunarity) {
    double value = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += simplex_noise_3d(x * frequency, y * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / maxValue;
}

// Hybrid Multi-Fractal
double simplex_hybrid_multifractal_2d(double x, double y, int octaves, double persistence,
                                      double lacunarity, double offset) {
    double value = 1.0;
    double amplitude = 1.0;
    double frequency = 1.0;

    for (int i = 0; i < octaves; i++) {
        double noise = simplex_noise_2d(x * frequency, y * frequency);
        value *= (offset + fabs(noise)) * amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value;
}

// Domain Warping
double simplex_domain_warp_2d(double x, double y, double warp_strength) {
    double warp_x = x + simplex_noise_2d(x, y) * warp_strength;
    double warp_y = y + simplex_noise_2d(x + 100, y + 100) * warp_strength;
    return simplex_noise_2d(warp_x, warp_y);
}

/* ===== PERFORMANCE & UTILITY FUNCTIONS ===== */

int simplex_noise_array_2d(double x_start, double y_start, int width, int height, double step,
                           double* output) {
    if (!output || width <= 0 || height <= 0)
        return -1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise_x = x_start + x * step;
            double noise_y = y_start + y * step;
            output[y * width + x] = simplex_noise_2d(noise_x, noise_y);
        }
    }

    return 0;
}

int simplex_noise_array_3d(double x_start, double y_start, double z_start, int width, int height,
                           int depth, double step, double* output) {
    if (!output || width <= 0 || height <= 0 || depth <= 0)
        return -1;

    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double noise_x = x_start + x * step;
                double noise_y = y_start + y * step;
                double noise_z = z_start + z * step;
                output[(z * height + y) * width + x] = simplex_noise_3d(noise_x, noise_y, noise_z);
            }
        }
    }

    return 0;
}

void simplex_cleanup(void) {
    // Reset all state
    initialized = 0;
    cache_enabled = 0;
    profiling_enabled = 0;

    // Clear cache
    for (int i = 0; i < CACHE_SIZE; i++) {
        noise_cache[i].valid = 0;
    }

    // Reset performance stats
    simplex_reset_performance_stats();
}

double simplex_noise_1d(double x) {
    if (!initialized) {
        simplex_noise_init(0);
    }

    int i0 = fast_floor(x);
    int i1 = i0 + 1;

    double x0 = x - i0;
    double x1 = x0 - 1.0;

    double t0 = 1.0 - x0 * x0;
    double t1 = 1.0 - x1 * x1;

    t0 *= t0;
    t1 *= t1;

    double n0 = t0 * t0 * dot2d(grad2[perm[i0 & 0xff] & 7], x0, 0);
    double n1 = t1 * t1 * dot2d(grad2[perm[i1 & 0xff] & 7], x1, 0);

    return 70.0 * (n0 + n1);
}

double simplex_noise_2d(double x, double y) {
    if (!initialized) {
        simplex_noise_init(0);
    }

    const double F2 = 0.5 * (sqrt(3.0) - 1.0);
    const double G2 = (3.0 - sqrt(3.0)) / 6.0;

    double s = (x + y) * F2;
    int i = fast_floor(x + s);
    int j = fast_floor(y + s);

    double t = (i + j) * G2;
    double x0 = x - (i - t);
    double y0 = y - (j - t);

    int i1, j1;
    if (x0 > y0) {
        i1 = 1;
        j1 = 0;
    } else {
        i1 = 0;
        j1 = 1;
    }

    double x1 = x0 - i1 + G2;
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2;
    double y2 = y0 - 1.0 + 2.0 * G2;

    int ii = i & 0xff;
    int jj = j & 0xff;
    int gi0 = perm[ii + perm[jj]] % 8;
    int gi1 = perm[ii + i1 + perm[jj + j1]] % 8;
    int gi2 = perm[ii + 1 + perm[jj + 1]] % 8;

    double t0 = 0.5 - x0 * x0 - y0 * y0;
    double n0 = 0.0;
    if (t0 >= 0) {
        t0 *= t0;
        n0 = t0 * t0 * dot2d(grad2[gi0], x0, y0);
    }

    double t1 = 0.5 - x1 * x1 - y1 * y1;
    double n1 = 0.0;
    if (t1 >= 0) {
        t1 *= t1;
        n1 = t1 * t1 * dot2d(grad2[gi1], x1, y1);
    }

    double t2 = 0.5 - x2 * x2 - y2 * y2;
    double n2 = 0.0;
    if (t2 >= 0) {
        t2 *= t2;
        n2 = t2 * t2 * dot2d(grad2[gi2], x2, y2);
    }

    return 70.0 * (n0 + n1 + n2);
}

double simplex_noise_3d(double x, double y, double z) {
    if (!initialized) {
        simplex_noise_init(0);
    }

    const double F3 = 1.0 / 3.0;
    const double G3 = 1.0 / 6.0;

    double s = (x + y + z) * F3;
    int i = fast_floor(x + s);
    int j = fast_floor(y + s);
    int k = fast_floor(z + s);

    double t = (i + j + k) * G3;
    double x0 = x - (i - t);
    double y0 = y - (j - t);
    double z0 = z - (k - t);

    int i1, j1, k1;
    int i2, j2, k2;

    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0;
        } else if (x0 >= z0) {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 0;
            k2 = 1;
        } else {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 1;
            j2 = 0;
            k2 = 1;
        }
    } else {
        if (y0 < z0) {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 0;
            j2 = 1;
            k2 = 1;
        } else if (x0 < z0) {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 0;
            j2 = 1;
            k2 = 1;
        } else {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0;
        }
    }

    double x1 = x0 - i1 + G3;
    double y1 = y0 - j1 + G3;
    double z1 = z0 - k1 + G3;
    double x2 = x0 - i2 + 2.0 * G3;
    double y2 = y0 - j2 + 2.0 * G3;
    double z2 = z0 - k2 + 2.0 * G3;
    double x3 = x0 - 1.0 + 3.0 * G3;
    double y3 = y0 - 1.0 + 3.0 * G3;
    double z3 = z0 - 1.0 + 3.0 * G3;

    int ii = i & 0xff;
    int jj = j & 0xff;
    int kk = k & 0xff;
    int gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
    int gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12;
    int gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12;
    int gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12;

    double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
    double n0 = 0.0;
    if (t0 >= 0) {
        t0 *= t0;
        n0 = t0 * t0 * dot3d(grad3[gi0], x0, y0, z0);
    }

    double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
    double n1 = 0.0;
    if (t1 >= 0) {
        t1 *= t1;
        n1 = t1 * t1 * dot3d(grad3[gi1], x1, y1, z1);
    }

    double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2;
    double n2 = 0.0;
    if (t2 >= 0) {
        t2 *= t2;
        n2 = t2 * t2 * dot3d(grad3[gi2], x2, y2, z2);
    }

    double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;
    double n3 = 0.0;
    if (t3 >= 0) {
        t3 *= t3;
        n3 = t3 * t3 * dot3d(grad3[gi3], x3, y3, z3);
    }

    return 32.0 * (n0 + n1 + n2 + n3);
}

double simplex_noise_4d(double x, double y, double z, double w) {
    if (!initialized) {
        simplex_noise_init(0);
    }

    const double F4 = (sqrt(5.0) - 1.0) / 4.0;
    const double G4 = (5.0 - sqrt(5.0)) / 20.0;

    double s = (x + y + z + w) * F4;
    int i = fast_floor(x + s);
    int j = fast_floor(y + s);
    int k = fast_floor(z + s);
    int l = fast_floor(w + s);

    double t = (i + j + k + l) * G4;
    double x0 = x - (i - t);
    double y0 = y - (j - t);
    double z0 = z - (k - t);
    double w0 = w - (l - t);

    int c1 = (x0 > y0) ? 32 : 0;
    int c2 = (x0 > z0) ? 16 : 0;
    int c3 = (y0 > z0) ? 8 : 0;
    int c4 = (x0 > w0) ? 4 : 0;
    int c5 = (y0 > w0) ? 2 : 0;
    int c6 = (z0 > w0) ? 1 : 0;
    int c = c1 + c2 + c3 + c4 + c5 + c6;

    int i1, j1, k1, l1;
    int i2, j2, k2, l2;
    int i3, j3, k3, l3;

    static const int simplex[64][4] = {
        {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 0, 1, 1}, {0, 2, 3, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {1, 2, 3, 0}, {0, 2, 1, 3}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {1, 3, 0, 2}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {1, 2, 0, 3}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {1, 3, 2, 0}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {2, 3, 0, 1}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {0, 0, 1, 1}, {2, 3, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {0, 0, 1, 1}, {0, 0, 1, 1}, {2, 0, 3, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
        {3, 0, 1, 2}, {3, 1, 0, 2}, {0, 0, 1, 1}, {0, 0, 1, 1}};

    i1 = simplex[c][0] >= 3 ? 1 : 0;
    j1 = simplex[c][1] >= 3 ? 1 : 0;
    k1 = simplex[c][2] >= 3 ? 1 : 0;
    l1 = simplex[c][3] >= 3 ? 1 : 0;

    i2 = simplex[c][0] >= 2 ? 1 : 0;
    j2 = simplex[c][1] >= 2 ? 1 : 0;
    k2 = simplex[c][2] >= 2 ? 1 : 0;
    l2 = simplex[c][3] >= 2 ? 1 : 0;

    i3 = simplex[c][0] >= 1 ? 1 : 0;
    j3 = simplex[c][1] >= 1 ? 1 : 0;
    k3 = simplex[c][2] >= 1 ? 1 : 0;
    l3 = simplex[c][3] >= 1 ? 1 : 0;

    double x1 = x0 - i1 + G4;
    double y1 = y0 - j1 + G4;
    double z1 = z0 - k1 + G4;
    double w1 = w0 - l1 + G4;
    double x2 = x0 - i2 + 2.0 * G4;
    double y2 = y0 - j2 + 2.0 * G4;
    double z2 = z0 - k2 + 2.0 * G4;
    double w2 = w0 - l2 + 2.0 * G4;
    double x3 = x0 - i3 + 3.0 * G4;
    double y3 = y0 - j3 + 3.0 * G4;
    double z3 = z0 - k3 + 3.0 * G4;
    double w3 = w0 - l3 + 3.0 * G4;
    double x4 = x0 - 1.0 + 4.0 * G4;
    double y4 = y0 - 1.0 + 4.0 * G4;
    double z4 = z0 - 1.0 + 4.0 * G4;
    double w4 = w0 - 1.0 + 4.0 * G4;

    int ii = i & 0xff;
    int jj = j & 0xff;
    int kk = k & 0xff;
    int ll = l & 0xff;
    int gi0 = perm[ii + perm[jj + perm[kk + perm[ll]]]] % 32;
    int gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1 + perm[ll + l1]]]] % 32;
    int gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2 + perm[ll + l2]]]] % 32;
    int gi3 = perm[ii + i3 + perm[jj + j3 + perm[kk + k3 + perm[ll + l3]]]] % 32;
    int gi4 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] % 32;

    double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
    double n0 = 0.0;
    if (t0 >= 0) {
        t0 *= t0;
        n0 = t0 * t0 * dot4d(grad4[gi0], x0, y0, z0, w0);
    }

    double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
    double n1 = 0.0;
    if (t1 >= 0) {
        t1 *= t1;
        n1 = t1 * t1 * dot4d(grad4[gi1], x1, y1, z1, w1);
    }

    double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
    double n2 = 0.0;
    if (t2 >= 0) {
        t2 *= t2;
        n2 = t2 * t2 * dot4d(grad4[gi2], x2, y2, z2, w2);
    }

    double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
    double n3 = 0.0;
    if (t3 >= 0) {
        t3 *= t3;
        n3 = t3 * t3 * dot4d(grad4[gi3], x3, y3, z3, w3);
    }

    double t4 = 0.6 - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
    double n4 = 0.0;
    if (t4 >= 0) {
        t4 *= t4;
        n4 = t4 * t4 * dot4d(grad4[gi4], x4, y4, z4, w4);
    }

    return 27.0 * (n0 + n1 + n2 + n3 + n4);
}

double simplex_fractal_2d(double x, double y, int octaves, double persistence, double lacunarity) {
    double value = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += simplex_noise_2d(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / maxValue;
}

double simplex_fractal_3d(double x, double y, double z, int octaves, double persistence,
                          double lacunarity) {
    double value = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += simplex_noise_3d(x * frequency, y * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return value / maxValue;
}

#pragma GCC diagnostic pop
