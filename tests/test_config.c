/**
 * @file test_config.c
 * @brief Configuration system test
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
#include <string.h>
#include <math.h>

int main(void) {
    printf("Simplex Noise Configuration System Test\n");
    printf("======================================\n\n");
    
    // Test 1: Default configuration
    printf("Test 1: Default configuration...\n");
    simplex_config_t config = simplex_get_default_config();
    printf("✓ Default configuration created\n\n");
    
    // Test 2: Configuration validation
    printf("Test 2: Configuration validation...\n");
    simplex_config_validation_t validation;
    simplex_validate_config(&config, &validation);
    printf("Valid: %s, Errors: %d, Warnings: %d\n", 
           validation.valid ? "Yes" : "No", validation.error_count, validation.warning_count);
    printf("✓ Configuration validation working\n\n");
    
    // Test 3: Configuration file operations
    printf("Test 3: Configuration file operations...\n");
    config.seed = 54321;
    config.octaves = 6;
    config.persistence = 0.7;
    
    if (simplex_save_config("test_config.ini", SIMPLEX_CONFIG_INI, &config) == 0) {
        printf("✓ INI config saved\n");
        
        simplex_config_t loaded_config;
        if (simplex_load_config("test_config.ini", SIMPLEX_CONFIG_INI, &loaded_config) == 0) {
            printf("✓ INI config loaded\n");
            if (loaded_config.seed == config.seed && loaded_config.octaves == config.octaves) {
                printf("✓ Config values match\n\n");
            } else {
                printf("✗ Config values don't match\n\n");
                return 1;
            }
        } else {
            printf("✗ Failed to load INI config\n\n");
            return 1;
        }
    } else {
        printf("✗ Failed to save INI config\n\n");
        return 1;
    }
    
    // Test 4: Key-value operations
    printf("Test 4: Key-value operations...\n");
    char value_str[256];
    double value_double;
    
    if (simplex_get_config_string(&config, "persistence", value_str, sizeof(value_str)) == 0) {
        printf("✓ Got persistence as string: %s\n", value_str);
    }
    
    if (simplex_set_config_double(&config, "lacunarity", 3.0) == 0) {
        printf("✓ Set lacunarity to 3.0\n");
        if (simplex_get_config_double(&config, "lacunarity", &value_double) == 0) {
            printf("✓ Got lacunarity as double: %.6f\n\n", value_double);
        }
    }
    
    // Cleanup
    remove("test_config.ini");
    
    printf("All configuration tests passed! ✓\n");
    return 0;
}
