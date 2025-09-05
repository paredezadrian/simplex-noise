#!/bin/bash
# Script to deploy documentation to GitHub Pages

echo "Building and deploying documentation to GitHub Pages..."
echo ""

source mkdocs-env/bin/activate

# Build the site
echo "Building documentation..."
mkdocs build

# Deploy to GitHub Pages
echo "Deploying to GitHub Pages..."
mkdocs gh-deploy --force

echo ""
echo "Documentation deployed successfully!"
echo "Visit: https://paredezadrian.github.io/simplex-noise"
