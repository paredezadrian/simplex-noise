/**
 * @file example_2d.c
 * @brief 2D simplex noise example
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
    printf("2D Simplex Noise Example\n");
    printf("========================\n\n");
    
    // Initialize with custom configuration
    simplex_config_t config = simplex_get_default_config();
    config.seed = 42;
    config.octaves = 6;
    config.persistence = 0.6;
    config.lacunarity = 2.0;
    simplex_noise_init_advanced(&config);
    
    // Generate 2D noise grid
    const int width = 20, height = 20;
    const double scale = 0.1;
    
    printf("Generating %dx%d 2D noise grid...\n", width, height);
    printf("Scale: %.2f, Octaves: %d, Persistence: %.2f\n\n", 
           scale, config.octaves, config.persistence);
    
    // Generate and display noise
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double noise_x = x * scale;
            double noise_y = y * scale;
            
            // Generate classic noise
            double classic = simplex_noise_2d(noise_x, noise_y);
            
            // Convert to character representation
            char classic_char = classic > 0.5 ? '#' : classic > 0 ? '+' : 
                               classic > -0.5 ? '.' : '-';
            
            printf("%c", classic_char);
        }
        printf("  "); // Separator
        
        for (int x = 0; x < width; x++) {
            double noise_x = x * scale;
            double noise_y = y * scale;
            double ridged = simplex_ridged_2d(noise_x, noise_y);
            char ridged_char = ridged > 0.7 ? '#' : ridged > 0.4 ? '+' : 
                              ridged > 0.1 ? '.' : ' ';
            printf("%c", ridged_char);
        }
        printf("  "); // Separator
        
        for (int x = 0; x < width; x++) {
            double noise_x = x * scale;
            double noise_y = y * scale;
            double fractal = simplex_fractal_2d(noise_x, noise_y, config.octaves, 
                                             config.persistence, config.lacunarity);
            char fractal_char = fractal > 0.5 ? '#' : fractal > 0 ? '+' : 
                               fractal > -0.5 ? '.' : '-';
            printf("%c", fractal_char);
        }
        printf("\n");
    }
    
    printf("\nLegend:\n");
    printf("Left:   Classic Simplex Noise\n");
    printf("Middle: Ridged Noise\n");
    printf("Right:  Fractal Noise\n");
    printf("Symbols: # = high, + = medium-high, . = medium, - = low, space = very low\n\n");
    
    // Demonstrate different noise variants
    printf("Noise Variants at position (10, 10):\n");
    double x = 10 * scale, y = 10 * scale;
    printf("  Classic:  %.6f\n", simplex_noise_2d(x, y));
    printf("  Ridged:   %.6f\n", simplex_ridged_2d(x, y));
    printf("  Billowy:  %.6f\n", simplex_billowy_2d(x, y));
    printf("  fBm:      %.6f\n", simplex_fbm_2d(x, y, 4, 0.5, 2.0));
    printf("  Hybrid:   %.6f\n", simplex_hybrid_multifractal_2d(x, y, 4, 0.5, 2.0, 0.3));
    printf("  Domain:   %.6f\n", simplex_domain_warp_2d(x, y, 0.5));
    
    simplex_cleanup();
    printf("\nExample completed successfully!\n");
    return 0;
}
