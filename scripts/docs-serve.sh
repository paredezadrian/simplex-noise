#!/bin/bash
# Script to serve MkDocs documentation locally

echo "Starting MkDocs development server..."
echo "Documentation will be available at: http://127.0.0.1:8000"
echo "Press Ctrl+C to stop the server"
echo ""

source mkdocs-env/bin/activate
mkdocs serve --dev-addr=127.0.0.1:8000
