/**
 * @file example_image.c
 * @brief Image generation example for simplex noise
 * 
 * @copyright Copyright (c) 2025. All rights reserved.
 * @license This code is provided under the MIT License.
 * 
 * @author Adrian Paredez
 * @version 1.0
 * @date 9/5/2025
 */

#include <simplex_image.h>
#include <simplex_noise.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Simplex Noise Image Generation Example\n");
    printf("=====================================\n\n");
    
    // Initialize simplex noise
    simplex_noise_init(12345);
    
    // Example 1: Basic 2D noise image
    printf("Generating basic 2D noise image...\n");
    simplex_image_config_t config = simplex_get_default_image_config();
    simplex_set_image_filename(&config, "basic_2d_noise.ppm");
    simplex_set_image_size(&config, 512, 512);
    simplex_set_noise_params(&config, 0.01, 1, 0.5, 2.0);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);
    
    if (simplex_generate_2d_image(&config) == 0) {
        printf("✓ Basic 2D noise image saved as 'basic_2d_noise.ppm'\n");
    } else {
        printf("✗ Failed to generate basic 2D noise image\n");
    }
    
    // Example 2: Fractal noise image
    printf("\nGenerating fractal noise image...\n");
    simplex_set_image_filename(&config, "fractal_noise.ppm");
    simplex_set_noise_params(&config, 0.005, 6, 0.6, 2.0);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_RGB);
    
    if (simplex_generate_fractal_image(&config) == 0) {
        printf("✓ Fractal noise image saved as 'fractal_noise.ppm'\n");
    } else {
        printf("✗ Failed to generate fractal noise image\n");
    }
    
    // Example 3: Heightmap
    printf("\nGenerating heightmap...\n");
    simplex_set_image_filename(&config, "heightmap.ppm");
    simplex_set_image_size(&config, 1024, 1024);
    simplex_set_noise_params(&config, 0.003, 8, 0.7, 2.0);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_HEIGHTMAP);
    
    if (simplex_generate_heightmap(&config) == 0) {
        printf("✓ Heightmap saved as 'heightmap.ppm'\n");
    } else {
        printf("✗ Failed to generate heightmap\n");
    }
    
    // Example 4: Terrain texture
    printf("\nGenerating terrain texture...\n");
    simplex_set_image_filename(&config, "terrain_texture.ppm");
    simplex_set_image_size(&config, 512, 512);
    simplex_set_noise_params(&config, 0.008, 4, 0.5, 2.0);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_TERRAIN);
    
    if (simplex_generate_texture(&config) == 0) {
        printf("✓ Terrain texture saved as 'terrain_texture.ppm'\n");
    } else {
        printf("✗ Failed to generate terrain texture\n");
    }
    
    // Example 5: 3D noise slice
    printf("\nGenerating 3D noise slice...\n");
    simplex_set_image_filename(&config, "3d_slice.ppm");
    simplex_set_image_size(&config, 512, 512);
    simplex_set_noise_params(&config, 0.01, 1, 0.5, 2.0);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_GRAYSCALE);
    
    if (simplex_generate_3d_image(&config, 0.5) == 0) {
        printf("✓ 3D noise slice saved as '3d_slice.ppm'\n");
    } else {
        printf("✗ Failed to generate 3D noise slice\n");
    }
    
    // Example 6: Image series
    printf("\nGenerating image series...\n");
    double scale_variations[] = {0.005, 0.01, 0.02, 0.04, 0.08};
    uint32_t seed_variations[] = {12345, 23456, 34567, 45678, 56789};
    
    if (simplex_generate_image_series(&config, 5, scale_variations, seed_variations) == 0) {
        printf("✓ Image series generated (simplex_series_0.ppm to simplex_series_4.ppm)\n");
    } else {
        printf("✗ Failed to generate image series\n");
    }
    
    // Example 7: Animation frames
    printf("\nGenerating animation frames...\n");
    simplex_set_image_filename(&config, "animation_frame.ppm");
    simplex_set_image_size(&config, 256, 256);
    simplex_set_noise_params(&config, 0.02, 3, 0.6, 2.0);
    simplex_set_color_mode(&config, SIMPLEX_COLOR_RGB);
    
    // Create animation directory
    if (system("mkdir -p animation_frames") != 0) {
        printf("Warning: Could not create animation_frames directory\n");
    }
    
    if (simplex_generate_animation(&config, 10, 0.1, "animation_frames") == 0) {
        printf("✓ Animation frames generated in 'animation_frames/' directory\n");
    } else {
        printf("✗ Failed to generate animation frames\n");
    }
    
    printf("\n=== IMAGE GENERATION COMPLETE ===\n");
    printf("Generated files:\n");
    printf("- basic_2d_noise.ppm (512x512 grayscale)\n");
    printf("- fractal_noise.ppm (512x512 RGB)\n");
    printf("- heightmap.ppm (1024x1024 heightmap colors)\n");
    printf("- terrain_texture.ppm (512x512 terrain colors)\n");
    printf("- 3d_slice.ppm (512x512 3D noise slice)\n");
    printf("- simplex_series_0.ppm to simplex_series_4.ppm (scale variations)\n");
    printf("- animation_frames/frame_0000.ppm to frame_0009.ppm (animation)\n");
    printf("\nYou can view these images with any PPM viewer or convert them to PNG:\n");
    printf("  convert basic_2d_noise.ppm basic_2d_noise.png\n");
    printf("  convert fractal_noise.ppm fractal_noise.png\n");
    printf("  convert heightmap.ppm heightmap.png\n");
    printf("  convert terrain_texture.ppm terrain_texture.png\n");
    
    return 0;
}
