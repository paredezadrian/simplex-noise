# Scripts Directory

This directory contains utility scripts for the Simplex Noise Library project.

## Available Scripts

### Documentation Scripts

- **`docs-serve.sh`** - Start local MkDocs development server

  ```bash
  ./scripts/docs-serve.sh
  ```

  Starts the documentation server at http://127.0.0.1:8000

- **`docs-deploy.sh`** - Deploy documentation to GitHub Pages
  ```bash
  ./scripts/docs-deploy.sh
  ```
  Builds and deploys the documentation to GitHub Pages

### Code Quality Scripts

- **`format-code.sh`** - Format code using clang-format

  ```bash
  ./scripts/format-code.sh
  ```

- **`fix-braces.sh`** - Fix brace formatting issues
  ```bash
  ./scripts/fix-braces.sh
  ```

## Usage

All scripts should be run from the project root directory:

```bash
# From project root
./scripts/docs-serve.sh
./scripts/docs-deploy.sh
./scripts/format-code.sh
./scripts/fix-braces.sh
```

## Requirements

- **Documentation scripts**: Python virtual environment with MkDocs
- **Code quality scripts**: clang-format, clang-tidy
