#!/usr/bin/env python3
"""
Test Suite for Simplex Noise Python Wrapper

This module contains comprehensive tests for the simplex noise library.
"""

import unittest
import numpy as np
import tempfile
import os
from simplex_noise import SimplexNoise, noise_2d, fractal_2d, generate_terrain


class TestSimplexNoise(unittest.TestCase):
    """Test cases for the SimplexNoise class."""

    def setUp(self):
        """Set up test fixtures."""
        self.noise = SimplexNoise(seed=42)

    def test_initialization(self):
        """Test noise generator initialization."""
        self.assertEqual(self.noise.seed, 42)
        self.assertTrue(self.noise._initialized)

    def test_seed_property(self):
        """Test seed property getter and setter."""
        # Test getter
        self.assertEqual(self.noise.seed, 42)

        # Test setter
        self.noise.seed = 123
        self.assertEqual(self.noise.seed, 123)
        self.assertTrue(self.noise._initialized)

    def test_noise_2d_single(self):
        """Test 2D noise generation for single values."""
        value = self.noise.noise_2d(1.0, 2.0)
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, -1.0)
        self.assertLessEqual(value, 1.0)

    def test_noise_3d_single(self):
        """Test 3D noise generation for single values."""
        value = self.noise.noise_3d(1.0, 2.0, 3.0)
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, -1.0)
        self.assertLessEqual(value, 1.0)

    def test_noise_4d_single(self):
        """Test 4D noise generation for single values."""
        value = self.noise.noise_4d(1.0, 2.0, 3.0, 4.0)
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, -1.0)
        self.assertLessEqual(value, 1.0)

    def test_noise_2d_array(self):
        """Test 2D noise generation for arrays."""
        x = np.array([1.0, 2.0, 3.0])
        y = np.array([1.0, 2.0, 3.0])

        result = self.noise.noise_2d(x, y)
        self.assertIsInstance(result, np.ndarray)
        self.assertEqual(result.shape, x.shape)
        self.assertTrue(np.all(result >= -1.0))
        self.assertTrue(np.all(result <= 1.0))

    def test_noise_3d_array(self):
        """Test 3D noise generation for arrays."""
        x = np.array([1.0, 2.0])
        y = np.array([1.0, 2.0])
        z = np.array([1.0, 2.0])

        result = self.noise.noise_3d(x, y, z)
        self.assertIsInstance(result, np.ndarray)
        self.assertEqual(result.shape, x.shape)
        self.assertTrue(np.all(result >= -1.0))
        self.assertTrue(np.all(result <= 1.0))

    def test_noise_4d_array(self):
        """Test 4D noise generation for arrays."""
        x = np.array([1.0, 2.0])
        y = np.array([1.0, 2.0])
        z = np.array([1.0, 2.0])
        w = np.array([1.0, 2.0])

        result = self.noise.noise_4d(x, y, z, w)
        self.assertIsInstance(result, np.ndarray)
        self.assertEqual(result.shape, x.shape)
        self.assertTrue(np.all(result >= -1.0))
        self.assertTrue(np.all(result <= 1.0))

    def test_fractal_2d_single(self):
        """Test 2D fractal noise generation for single values."""
        value = self.noise.fractal_2d(
            1.0, 2.0, octaves=4, persistence=0.5, lacunarity=2.0
        )
        self.assertIsInstance(value, float)
        # Fractal noise can have a wider range than single octave
        self.assertGreaterEqual(value, -2.0)
        self.assertLessEqual(value, 2.0)

    def test_fractal_3d_single(self):
        """Test 3D fractal noise generation for single values."""
        value = self.noise.fractal_3d(
            1.0, 2.0, 3.0, octaves=4, persistence=0.5, lacunarity=2.0
        )
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, -2.0)
        self.assertLessEqual(value, 2.0)

    def test_fractal_2d_array(self):
        """Test 2D fractal noise generation for arrays."""
        x = np.array([1.0, 2.0])
        y = np.array([1.0, 2.0])

        result = self.noise.fractal_2d(x, y, octaves=4, persistence=0.5, lacunarity=2.0)
        self.assertIsInstance(result, np.ndarray)
        self.assertEqual(result.shape, x.shape)
        self.assertTrue(np.all(result >= -2.0))
        self.assertTrue(np.all(result <= 2.0))

    def test_ridged_2d(self):
        """Test ridged noise generation."""
        value = self.noise.ridged_2d(1.0, 2.0, offset=1.0)
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, 0.0)  # Ridged noise is always positive

    def test_billowy_2d(self):
        """Test billowy noise generation."""
        value = self.noise.billowy_2d(1.0, 2.0, offset=1.0)
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, 0.0)  # Billowy noise is always positive

    def test_fbm_2d(self):
        """Test FBM noise generation."""
        value = self.noise.fbm_2d(1.0, 2.0, octaves=4, persistence=0.5, lacunarity=2.0)
        self.assertIsInstance(value, float)
        # FBM can have a wider range
        self.assertGreaterEqual(value, -2.0)
        self.assertLessEqual(value, 2.0)

    def test_deterministic_output(self):
        """Test that the same input produces the same output."""
        # Test with same seed
        noise1 = SimplexNoise(seed=42)
        noise2 = SimplexNoise(seed=42)

        value1 = noise1.noise_2d(1.0, 2.0)
        value2 = noise2.noise_2d(1.0, 2.0)

        self.assertEqual(value1, value2)

    def test_different_seeds(self):
        """Test that different seeds produce different outputs."""
        noise1 = SimplexNoise(seed=42)
        noise2 = SimplexNoise(seed=123)

        value1 = noise1.noise_2d(1.0, 2.0)
        value2 = noise2.noise_2d(1.0, 2.0)

        self.assertNotEqual(value1, value2)

    def test_array_shape_validation(self):
        """Test that array shape validation works correctly."""
        x = np.array([1.0, 2.0, 3.0])
        y = np.array([1.0, 2.0])  # Different shape

        with self.assertRaises(ValueError):
            self.noise.noise_2d(x, y)

    def test_uninitialized_error(self):
        """Test that uninitialized noise generator raises error."""
        noise = SimplexNoise(seed=42)
        noise._initialized = False  # Simulate uninitialized state

        with self.assertRaises(RuntimeError):
            noise.noise_2d(1.0, 2.0)


class TestConvenienceFunctions(unittest.TestCase):
    """Test cases for convenience functions."""

    def test_noise_2d_function(self):
        """Test noise_2d convenience function."""
        value = noise_2d(1.0, 2.0, seed=42)
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, -1.0)
        self.assertLessEqual(value, 1.0)

    def test_fractal_2d_function(self):
        """Test fractal_2d convenience function."""
        value = fractal_2d(
            1.0, 2.0, octaves=4, persistence=0.5, lacunarity=2.0, seed=42
        )
        self.assertIsInstance(value, float)
        self.assertGreaterEqual(value, -2.0)
        self.assertLessEqual(value, 2.0)

    def test_generate_terrain_function(self):
        """Test generate_terrain convenience function."""
        terrain = generate_terrain(64, 64, seed=42, octaves=4)
        self.assertIsInstance(terrain, np.ndarray)
        self.assertEqual(terrain.shape, (64, 64))
        self.assertTrue(np.all(terrain >= -2.0))
        self.assertTrue(np.all(terrain <= 2.0))


class TestImageGeneration(unittest.TestCase):
    """Test cases for image generation functionality."""

    def setUp(self):
        """Set up test fixtures."""
        self.noise = SimplexNoise(seed=42)

    def test_generate_image_grayscale(self):
        """Test grayscale image generation."""
        with tempfile.NamedTemporaryFile(suffix=".png", delete=False) as tmp:
            filename = tmp.name

        try:
            self.noise.generate_image(
                filename, width=64, height=64, color_mode="grayscale"
            )
            self.assertTrue(os.path.exists(filename))
            self.assertGreater(os.path.getsize(filename), 0)
        finally:
            if os.path.exists(filename):
                os.unlink(filename)

    def test_generate_image_rgb(self):
        """Test RGB image generation."""
        with tempfile.NamedTemporaryFile(suffix=".png", delete=False) as tmp:
            filename = tmp.name

        try:
            self.noise.generate_image(filename, width=64, height=64, color_mode="rgb")
            self.assertTrue(os.path.exists(filename))
            self.assertGreater(os.path.getsize(filename), 0)
        finally:
            if os.path.exists(filename):
                os.unlink(filename)

    def test_generate_image_heightmap(self):
        """Test heightmap image generation."""
        with tempfile.NamedTemporaryFile(suffix=".png", delete=False) as tmp:
            filename = tmp.name

        try:
            self.noise.generate_image(
                filename, width=64, height=64, color_mode="heightmap"
            )
            self.assertTrue(os.path.exists(filename))
            self.assertGreater(os.path.getsize(filename), 0)
        finally:
            if os.path.exists(filename):
                os.unlink(filename)

    def test_generate_image_invalid_color_mode(self):
        """Test that invalid color mode raises error."""
        with tempfile.NamedTemporaryFile(suffix=".png", delete=False) as tmp:
            filename = tmp.name

        try:
            with self.assertRaises(ValueError):
                self.noise.generate_image(
                    filename, width=64, height=64, color_mode="invalid"
                )
        finally:
            if os.path.exists(filename):
                os.unlink(filename)


class TestPerformance(unittest.TestCase):
    """Test cases for performance characteristics."""

    def test_large_array_performance(self):
        """Test performance with large arrays."""
        noise = SimplexNoise(seed=42)

        # Generate large array
        x = np.linspace(0, 10, 1000)
        y = np.linspace(0, 10, 1000)
        X, Y = np.meshgrid(x, y)

        # This should complete without errors
        result = noise.noise_2d(X, Y)
        self.assertEqual(result.shape, (1000, 1000))
        self.assertTrue(np.all(result >= -1.0))
        self.assertTrue(np.all(result <= 1.0))

    def test_fractal_performance(self):
        """Test performance with fractal noise."""
        noise = SimplexNoise(seed=42)

        x = np.linspace(0, 10, 500)
        y = np.linspace(0, 10, 500)
        X, Y = np.meshgrid(x, y)

        # This should complete without errors
        result = noise.fractal_2d(X, Y, octaves=6, persistence=0.5, lacunarity=2.0)
        self.assertEqual(result.shape, (500, 500))
        self.assertTrue(np.all(result >= -2.0))
        self.assertTrue(np.all(result <= 2.0))


if __name__ == "__main__":
    # Run the tests
    unittest.main(verbosity=2)
