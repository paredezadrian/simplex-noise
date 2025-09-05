/**
 * @file example_fractal.c
 * @brief Fractal noise example
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
#include <math.h>

int main(void) {
    printf("Fractal Noise Example\n");
    printf("=====================\n\n");
    
    // Initialize with custom configuration
    simplex_config_t config = simplex_get_default_config();
    config.seed = 456;
    config.octaves = 6;
    config.persistence = 0.6;
    config.lacunarity = 2.0;
    simplex_noise_init_advanced(&config);
    
    // Generate fractal noise with different parameters
    const int size = 20;
    const double scale = 0.1;
    
    printf("Generating %dx%d fractal noise patterns...\n", size, size);
    printf("Scale: %.2f, Octaves: %d, Persistence: %.2f, Lacunarity: %.2f\n\n", 
           scale, config.octaves, config.persistence, config.lacunarity);
    
    // Generate different fractal patterns
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            double noise_x = x * scale;
            double noise_y = y * scale;
            
            // Generate fractal noise
            double fractal = simplex_fractal_2d(noise_x, noise_y, config.octaves, 
                                              config.persistence, config.lacunarity);
            
            // Convert to character representation
            char char_fractal = fractal > 0.5 ? '#' : fractal > 0 ? '+' : 
                               fractal > -0.5 ? '.' : '-';
            
            printf("%c", char_fractal);
        }
        printf("  "); // Separator
        
        for (int x = 0; x < size; x++) {
            double noise_x = x * scale;
            double noise_y = y * scale;
            double fbm = simplex_fbm_2d(noise_x, noise_y, config.octaves, 
                                      config.persistence, config.lacunarity);
            char char_fbm = fbm > 0.5 ? '#' : fbm > 0 ? '+' : 
                           fbm > -0.5 ? '.' : '-';
            printf("%c", char_fbm);
        }
        printf("  "); // Separator
        
        for (int x = 0; x < size; x++) {
            double noise_x = x * scale;
            double noise_y = y * scale;
            double hybrid = simplex_hybrid_multifractal_2d(noise_x, noise_y, config.octaves, 
                                                         config.persistence, config.lacunarity, 0.3);
            char char_hybrid = hybrid > 0.5 ? '#' : hybrid > 0 ? '+' : 
                              hybrid > -0.5 ? '.' : '-';
            printf("%c", char_hybrid);
        }
        printf("\n");
    }
    
    printf("\nLegend:\n");
    printf("Left:   Standard Fractal Noise\n");
    printf("Middle: Fractional Brownian Motion (fBm)\n");
    printf("Right:  Hybrid Multi-Fractal\n");
    printf("Symbols: # = high, + = medium-high, . = medium, - = low\n\n");
    
    // Demonstrate different fractal parameters
    printf("Fractal Noise with Different Parameters:\n");
    double x = 10 * scale, y = 10 * scale;
    
    printf("Standard Fractal (octaves=6, persistence=0.6): %.6f\n", 
           simplex_fractal_2d(x, y, 6, 0.6, 2.0));
    printf("High Detail (octaves=8, persistence=0.8): %.6f\n", 
           simplex_fractal_2d(x, y, 8, 0.8, 2.0));
    printf("Low Detail (octaves=3, persistence=0.3): %.6f\n", 
           simplex_fractal_2d(x, y, 3, 0.3, 2.0));
    printf("Smooth (octaves=4, persistence=0.4): %.6f\n", 
           simplex_fractal_2d(x, y, 4, 0.4, 2.0));
    
    printf("\nFractional Brownian Motion:\n");
    printf("fBm (octaves=6, persistence=0.6): %.6f\n", 
           simplex_fbm_2d(x, y, 6, 0.6, 2.0));
    printf("fBm (octaves=8, persistence=0.8): %.6f\n", 
           simplex_fbm_2d(x, y, 8, 0.8, 2.0));
    
    printf("\nHybrid Multi-Fractal:\n");
    printf("Hybrid (offset=0.3): %.6f\n", 
           simplex_hybrid_multifractal_2d(x, y, 6, 0.6, 2.0, 0.3));
    printf("Hybrid (offset=0.1): %.6f\n", 
           simplex_hybrid_multifractal_2d(x, y, 6, 0.6, 2.0, 0.1));
    printf("Hybrid (offset=0.5): %.6f\n", 
           simplex_hybrid_multifractal_2d(x, y, 6, 0.6, 2.0, 0.5));
    
    simplex_cleanup();
    printf("\nFractal example completed successfully!\n");
    return 0;
}
