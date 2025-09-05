/**
 * @file test_performance.c
 * @brief Performance benchmark test
 * 
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 * 
 * @author Adrian Paredez
 * @version 1.0
 * @date 9/5/2025
 */

#include <simplex_noise.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(void) {
    printf("Simplex Noise Performance Benchmark\n");
    printf("===================================\n\n");
    
    // Initialize with profiling enabled
    simplex_config_t config = simplex_get_default_config();
    config.enable_profiling = 1;
    config.enable_caching = 1;
    simplex_noise_init_advanced(&config);
    
    const int iterations = 100000;
    clock_t start, end;
    double time_taken;
    
    // Test 1: 2D noise performance
    printf("Test 1: 2D noise performance...\n");
    start = clock();
    for (int i = 0; i < iterations; i++) {
        double x = (double)i * 0.01;
        double y = (double)i * 0.02;
        simplex_noise_2d(x, y);
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Generated %d 2D noise samples in %.6f seconds\n", iterations, time_taken);
    printf("Performance: %.2f million samples/second\n", iterations / (time_taken * 1000000.0));
    printf("✓ 2D noise performance test completed\n\n");
    
    // Test 2: 3D noise performance
    printf("Test 2: 3D noise performance...\n");
    start = clock();
    for (int i = 0; i < iterations; i++) {
        double x = (double)i * 0.01;
        double y = (double)i * 0.02;
        double z = (double)i * 0.03;
        simplex_noise_3d(x, y, z);
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Generated %d 3D noise samples in %.6f seconds\n", iterations, time_taken);
    printf("Performance: %.2f million samples/second\n", iterations / (time_taken * 1000000.0));
    printf("✓ 3D noise performance test completed\n\n");
    
    // Test 3: Fractal noise performance
    printf("Test 3: Fractal noise performance...\n");
    start = clock();
    for (int i = 0; i < iterations / 10; i++) { // Fewer iterations for fractal
        double x = (double)i * 0.01;
        double y = (double)i * 0.02;
        simplex_fractal_2d(x, y, 4, 0.5, 2.0);
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Generated %d fractal noise samples in %.6f seconds\n", iterations / 10, time_taken);
    printf("Performance: %.2f thousand samples/second\n", (iterations / 10) / (time_taken * 1000.0));
    printf("✓ Fractal noise performance test completed\n\n");
    
    // Test 4: Array generation performance
    printf("Test 4: Array generation performance...\n");
    const int width = 100, height = 100;
    double *noise_array = malloc(width * height * sizeof(double));
    
    start = clock();
    simplex_noise_array_2d(0.0, 0.0, width, height, 0.01, noise_array);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Generated %dx%d noise array in %.6f seconds\n", width, height, time_taken);
    printf("Performance: %.2f million samples/second\n", (width * height) / (time_taken * 1000000.0));
    printf("✓ Array generation performance test completed\n\n");
    
    // Test 5: Performance statistics
    printf("Test 5: Performance statistics...\n");
    simplex_perf_stats_t stats;
    if (simplex_get_performance_stats(&stats) == 0) {
        printf("Function calls: %zu\n", simplex_get_function_call_count());
        printf("Cache hits: %d, Cache misses: %d\n", 
               simplex_get_cache_hits(), simplex_get_cache_misses());
        printf("✓ Performance statistics retrieved\n\n");
    } else {
        printf("✗ Failed to get performance statistics\n\n");
    }
    
    free(noise_array);
    simplex_cleanup();
    
    printf("All performance tests completed! ✓\n");
    return 0;
}
