/**
 * @file example_3d.c
 * @brief 3D simplex noise example
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 *
 * @author Adrian Paredez
 * @version 1.0
 * @date 9/5/2025
 */

#include <math.h>
#include <simplex_noise.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("3D Simplex Noise Example\n");
    printf("========================\n\n");

    // Initialize with custom configuration
    simplex_config_t config = simplex_get_default_config();
    config.seed = 123;
    config.octaves = 4;
    config.persistence = 0.5;
    config.lacunarity = 2.0;
    simplex_noise_init_advanced(&config);

    // Generate 3D noise slices
    const int size = 16;
    const double scale = 0.2;

    printf("Generating %dx%d 3D noise slices...\n", size, size);
    printf("Scale: %.2f, Octaves: %d, Persistence: %.2f\n\n", scale, config.octaves,
           config.persistence);

    // Generate slices at different Z levels
    for (int z = 0; z < 4; z++) {
        printf("Z slice %d:\n", z);
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                double noise_x = x * scale;
                double noise_y = y * scale;
                double noise_z = z * scale;

                // Generate 3D noise
                double noise_3d = simplex_noise_3d(noise_x, noise_y, noise_z);

                // Convert to character representation
                char char_3d = noise_3d > 0.5    ? '#'
                               : noise_3d > 0    ? '+'
                               : noise_3d > -0.5 ? '.'
                                                 : '-';

                printf("%c", char_3d);
            }
            printf("  ");  // Separator

            for (int x = 0; x < size; x++) {
                double noise_x = x * scale;
                double noise_y = y * scale;
                double noise_z = z * scale;
                double fractal_3d = simplex_fractal_3d(noise_x, noise_y, noise_z, config.octaves,
                                                       config.persistence, config.lacunarity);
                char char_fractal = fractal_3d > 0.5    ? '#'
                                    : fractal_3d > 0    ? '+'
                                    : fractal_3d > -0.5 ? '.'
                                                        : '-';
                printf("%c", char_fractal);
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("Legend:\n");
    printf("Left:   Classic 3D Simplex Noise\n");
    printf("Right:  3D Fractal Noise\n");
    printf("Symbols: # = high, + = medium-high, . = medium, - = low\n\n");

    // Demonstrate different 3D noise variants
    printf("3D Noise Variants at position (8, 8, 2):\n");
    double x = 8 * scale, y = 8 * scale, z = 2 * scale;
    printf("  3D Classic:  %.6f\n", simplex_noise_3d(x, y, z));
    printf("  3D Fractal:  %.6f\n",
           simplex_fractal_3d(x, y, z, config.octaves, config.persistence, config.lacunarity));
    printf("  3D Ridged:   %.6f\n", simplex_ridged_3d(x, y, z));
    printf("  3D Billowy:  %.6f\n", simplex_billowy_3d(x, y, z));

    simplex_cleanup();
    printf("\n3D example completed successfully!\n");
    return 0;
}
