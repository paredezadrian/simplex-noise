#!/bin/bash
# Fix braces around single-line if statements

echo "Fixing braces around single-line if statements..."

# Fix common patterns
sed -i 's/if (\([^)]*\)) {$/if (\1) {/' src/simplex_noise.c
sed -i 's/if (\([^)]*\)) $/if (\1) {/' src/simplex_noise.c

# Add braces to single-line if statements that don't have them
sed -i 's/if (\([^)]*\))\n        \([^{].*\)$/if (\1) {\n        \2\n    }/' src/simplex_noise.c

echo "Braces fix complete!"
