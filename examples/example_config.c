/**
 * @file example_config.c
 * @brief Configuration system example
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

int main(void) {
    printf("Simplex Noise Configuration Example\n");
    printf("===================================\n\n");
    
    // Example 1: Create and save configuration
    printf("Example 1: Creating and saving configuration...\n");
    simplex_config_t config = simplex_get_default_config();
    
    // Customize configuration
    config.seed = 12345;
    config.octaves = 8;
    config.persistence = 0.7;
    config.lacunarity = 2.5;
    config.enable_caching = 1;
    config.enable_profiling = 1;
    strcpy(config.output_file, "noise_output.png");
    config.verbose_mode = 1;
    
    // Save to different formats
    if (simplex_save_config("my_config.ini", SIMPLEX_CONFIG_INI, &config) == 0) {
        printf("✓ Configuration saved to INI format\n");
    }
    
    if (simplex_save_config("my_config.json", SIMPLEX_CONFIG_JSON, &config) == 0) {
        printf("✓ Configuration saved to JSON format\n");
    }
    
    // Example 2: Load and validate configuration
    printf("\nExample 2: Loading and validating configuration...\n");
    simplex_config_t loaded_config;
    
    if (simplex_load_config("my_config.ini", SIMPLEX_CONFIG_INI, &loaded_config) == 0) {
        printf("✓ Configuration loaded from INI file\n");
        
        // Validate configuration
        simplex_config_validation_t validation;
        simplex_validate_config(&loaded_config, &validation);
        
        printf("Validation results:\n");
        printf("  Valid: %s\n", validation.valid ? "Yes" : "No");
        printf("  Errors: %d\n", validation.error_count);
        printf("  Warnings: %d\n", validation.warning_count);
        
        if (validation.warning_count > 0) {
            printf("Warnings:\n");
            for (int i = 0; i < validation.warning_count; i++) {
                printf("  %d: %s\n", i+1, validation.warnings[i]);
            }
        }
    }
    
    // Example 3: Runtime configuration modification
    printf("\nExample 3: Runtime configuration modification...\n");
    
    // Get current values
    char value_str[256];
    double value_double;
    int value_int;
    
    simplex_get_config_string(&loaded_config, "persistence", value_str, sizeof(value_str));
    printf("Current persistence: %s\n", value_str);
    
    // Modify values
    simplex_set_config_double(&loaded_config, "persistence", 0.8);
    simplex_set_config_int(&loaded_config, "octaves", 10);
    simplex_set_config_string(&loaded_config, "output_file", "modified_output.png");
    
    // Verify changes
    simplex_get_config_double(&loaded_config, "persistence", &value_double);
    simplex_get_config_int(&loaded_config, "octaves", &value_int);
    simplex_get_config_string(&loaded_config, "output_file", value_str, sizeof(value_str));
    
    printf("Modified persistence: %.6f\n", value_double);
    printf("Modified octaves: %d\n", value_int);
    printf("Modified output file: %s\n", value_str);
    
    // Example 4: Configuration merging
    printf("\nExample 4: Configuration merging...\n");
    simplex_config_t base_config = simplex_get_default_config();
    simplex_config_t override_config = simplex_get_default_config();
    simplex_config_t merged_config;
    
    // Set up base configuration
    base_config.seed = 1000;
    base_config.octaves = 4;
    strcpy(base_config.config_file, "base.ini");
    
    // Set up override configuration
    override_config.seed = 2000;
    override_config.octaves = 6;
    override_config.persistence = 0.9;
    strcpy(override_config.output_file, "override.png");
    
    // Merge configurations
    if (simplex_merge_config(&base_config, &override_config, &merged_config) == 0) {
        printf("✓ Configuration merge successful\n");
        printf("Merged seed: %u (from override)\n", merged_config.seed);
        printf("Merged octaves: %d (from override)\n", merged_config.octaves);
        printf("Merged persistence: %.6f (from override)\n", merged_config.persistence);
        printf("Merged config_file: %s (from base)\n", merged_config.config_file);
        printf("Merged output_file: %s (from override)\n", merged_config.output_file);
    }
    
    // Example 5: Print configuration in different formats
    printf("\nExample 5: Configuration display formats...\n");
    
    printf("\nCompact format:\n");
    simplex_print_config(&merged_config, 0);
    
    printf("\nVerbose format:\n");
    simplex_print_config(&merged_config, 1);
    
    // Example 6: Use configuration for noise generation
    printf("\nExample 6: Using configuration for noise generation...\n");
    simplex_noise_init_advanced(&merged_config);
    
    double noise_value = simplex_noise_2d(1.0, 2.0);
    printf("Noise value with merged config: %.6f\n", noise_value);
    
    // Cleanup
    remove("my_config.ini");
    remove("my_config.json");
    simplex_cleanup();
    
    printf("\nConfiguration example completed successfully!\n");
    return 0;
}
