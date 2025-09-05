/**
 * @file test_basic.c
 * @brief Basic functionality test for simplex noise
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
    printf("Simplex Noise Basic Functionality Test\n");
    printf("=====================================\n\n");
    
    // Test 1: Basic initialization
    printf("Test 1: Basic initialization...\n");
    simplex_noise_init(12345);
    printf("✓ Initialization successful\n\n");
    
    // Test 2: 1D noise generation
    printf("Test 2: 1D noise generation...\n");
    double noise_1d = simplex_noise_1d(1.0);
    printf("1D noise at x=1.0: %.6f\n", noise_1d);
    if (noise_1d >= -1.0 && noise_1d <= 1.0) {
        printf("✓ 1D noise in valid range\n\n");
    } else {
        printf("✗ 1D noise out of range\n\n");
        return 1;
    }
    
    // Test 3: 2D noise generation
    printf("Test 3: 2D noise generation...\n");
    double noise_2d = simplex_noise_2d(1.0, 2.0);
    printf("2D noise at (1.0, 2.0): %.6f\n", noise_2d);
    if (noise_2d >= -1.0 && noise_2d <= 1.0) {
        printf("✓ 2D noise in valid range\n\n");
    } else {
        printf("✗ 2D noise out of range\n\n");
        return 1;
    }
    
    // Test 4: 3D noise generation
    printf("Test 4: 3D noise generation...\n");
    double noise_3d = simplex_noise_3d(1.0, 2.0, 3.0);
    printf("3D noise at (1.0, 2.0, 3.0): %.6f\n", noise_3d);
    if (noise_3d >= -1.0 && noise_3d <= 1.0) {
        printf("✓ 3D noise in valid range\n\n");
    } else {
        printf("✗ 3D noise out of range\n\n");
        return 1;
    }
    
    // Test 5: 4D noise generation
    printf("Test 5: 4D noise generation...\n");
    double noise_4d = simplex_noise_4d(1.0, 2.0, 3.0, 4.0);
    printf("4D noise at (1.0, 2.0, 3.0, 4.0): %.6f\n", noise_4d);
    if (noise_4d >= -1.0 && noise_4d <= 1.0) {
        printf("✓ 4D noise in valid range\n\n");
    } else {
        printf("✗ 4D noise out of range\n\n");
        return 1;
    }
    
    // Test 6: Fractal noise
    printf("Test 6: Fractal noise generation...\n");
    double fractal_2d = simplex_fractal_2d(1.0, 2.0, 4, 0.5, 2.0);
    printf("2D fractal noise: %.6f\n", fractal_2d);
    if (fractal_2d >= -1.0 && fractal_2d <= 1.0) {
        printf("✓ Fractal noise in valid range\n\n");
    } else {
        printf("✗ Fractal noise out of range\n\n");
        return 1;
    }
    
    // Test 7: Advanced noise variants
    printf("Test 7: Advanced noise variants...\n");
    double ridged = simplex_ridged_2d(1.0, 2.0);
    double billowy = simplex_billowy_2d(1.0, 2.0);
    double fbm = simplex_fbm_2d(1.0, 2.0, 4, 0.5, 2.0);
    
    printf("Ridged noise: %.6f\n", ridged);
    printf("Billowy noise: %.6f\n", billowy);
    printf("fBm noise: %.6f\n", fbm);
    
    if (ridged >= 0.0 && ridged <= 1.0 && 
        billowy >= 0.0 && billowy <= 1.0 &&
        fbm >= -1.0 && fbm <= 1.0) {
        printf("✓ Advanced variants in valid ranges\n\n");
    } else {
        printf("✗ Advanced variants out of range\n\n");
        return 1;
    }
    
    printf("All basic functionality tests passed! ✓\n");
    return 0;
}
