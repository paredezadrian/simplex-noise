#!/bin/bash
# Script to serve MkDocs documentation locally

echo "Starting MkDocs development server..."
echo "Documentation will be available at: http://127.0.0.1:8000"
echo "Press Ctrl+C to stop the server"
echo ""

source mkdocs-env/bin/activate
# Install dependencies if not already installed
pip install -r requirements.txt > /dev/null 2>&1
mkdocs serve --dev-addr=127.0.0.1:8000
