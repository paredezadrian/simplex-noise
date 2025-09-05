# Contributing Guide

Thank you for your interest in contributing to the Simplex Noise Library! This guide will help you get started with contributing to the project.

## Getting Started

### Prerequisites

- C99 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10 or later
- Git
- Basic understanding of C programming
- Familiarity with simplex noise algorithms (helpful but not required)

### Development Setup

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:

   ```bash
   git clone https://github.com/yourusername/simplex-noise.git
   cd simplex-noise
   ```

3. **Create a development branch**:

   ```bash
   git checkout -b feature/your-feature-name
   ```

4. **Set up the build environment**:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON ..
   make
   ```

## Contribution Areas

### Code Contributions

#### Bug Fixes

- Fix bugs in existing functionality
- Improve error handling
- Optimize performance
- Fix memory leaks

#### New Features

- Additional noise variants
- New interpolation methods
- Performance optimizations
- Platform-specific improvements

#### Documentation

- Improve existing documentation
- Add code comments
- Create tutorials
- Fix typos and grammar

### Testing

#### Unit Tests

- Add tests for new features
- Improve test coverage
- Fix failing tests

#### Integration Tests

- Test with different compilers
- Test on different platforms
- Performance benchmarks

## Development Workflow

### 1. Planning

Before starting work:

- Check existing issues and pull requests
- Discuss major changes in an issue first
- Plan your implementation approach
- Consider backward compatibility

### 2. Coding Standards

#### Code Style

- Follow the existing code style
- Use 4 spaces for indentation
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small

#### C Standards

- Use C99 standard features
- Avoid C++ specific features
- Use `const` where appropriate
- Handle errors gracefully

#### Example Code Style

```c
/**
 * @brief Generate 2D simplex noise with custom parameters
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param scale Noise scale factor
 * @return Noise value in range [-1, 1]
 */
double generate_custom_noise(double x, double y, double scale) {
    if (scale <= 0.0) {
        return 0.0;  // Invalid scale
    }

    return simplex_noise_2d(x * scale, y * scale);
}
```

### 3. Testing

#### Before Submitting

- Run all existing tests
- Add tests for new functionality
- Test on multiple platforms if possible
- Check for memory leaks
- Verify performance impact

#### Running Tests

```bash
# Build with tests
cmake -DENABLE_TESTS=ON ..
make

# Run tests
make test

# Run specific test
./tests/test_simplex_noise
```

#### Writing Tests

```c
#include <assert.h>
#include "simplex_noise.h"

void test_custom_noise() {
    simplex_noise_init(12345);

    // Test basic functionality
    double noise = generate_custom_noise(1.0, 2.0, 0.1);
    assert(noise >= -1.0 && noise <= 1.0);

    // Test edge cases
    double zero_noise = generate_custom_noise(1.0, 2.0, 0.0);
    assert(zero_noise == 0.0);

    printf("test_custom_noise: PASSED\n");
}
```

### 4. Documentation

#### Code Documentation

- Add Doxygen comments for new functions
- Update existing documentation
- Include usage examples
- Document any breaking changes

#### User Documentation

- Update README if needed
- Add examples for new features
- Update API reference
- Create tutorials for complex features

### 5. Submitting Changes

#### Commit Messages

Use clear, descriptive commit messages:

```
Add support for custom noise variants

- Implement custom_noise_variant_t enum
- Add simplex_set_custom_variant() function
- Update configuration system
- Add tests for new functionality
- Update documentation

Fixes #123
```

#### Pull Request Process

1. **Create a pull request** from your feature branch
2. **Fill out the PR template** completely
3. **Link related issues** using "Fixes #123" or "Closes #123"
4. **Request review** from maintainers
5. **Address feedback** promptly
6. **Keep PRs focused** - one feature per PR

#### PR Template

```markdown
## Description

Brief description of changes

## Type of Change

- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing

- [ ] Tests pass locally
- [ ] New tests added
- [ ] Performance tested

## Checklist

- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No breaking changes (or documented)
```

## Code Review Process

### Review Criteria

#### Functionality

- Does the code work as intended?
- Are edge cases handled properly?
- Is error handling appropriate?
- Are there any security issues?

#### Code Quality

- Is the code readable and maintainable?
- Are variable and function names clear?
- Is the code well-documented?
- Are there any code smells?

#### Performance

- Is the code efficient?
- Are there any performance regressions?
- Is memory usage appropriate?
- Are there any bottlenecks?

#### Testing

- Are there adequate tests?
- Do tests cover edge cases?
- Are tests reliable and fast?
- Is test coverage sufficient?

### Review Process

1. **Automated checks** must pass
2. **At least one reviewer** must approve
3. **All feedback** must be addressed
4. **Tests must pass** after changes
5. **Documentation** must be updated

## Issue Reporting

### Bug Reports

When reporting bugs, include:

- **Clear description** of the problem
- **Steps to reproduce** the issue
- **Expected behavior** vs actual behavior
- **Environment details** (OS, compiler, version)
- **Minimal code example** if possible

#### Bug Report Template

````markdown
## Bug Description

Brief description of the bug

## Steps to Reproduce

1. Step one
2. Step two
3. Step three

## Expected Behavior

What should happen

## Actual Behavior

What actually happens

## Environment

- OS: Ubuntu 20.04
- Compiler: GCC 9.3.0
- Library Version: 1.0.0

## Code Example

```c
// Minimal code that reproduces the issue
```
````

### Feature Requests

When requesting features:

- **Describe the use case** clearly
- **Explain why** it's needed
- **Provide examples** of how it would be used
- **Consider alternatives** and trade-offs

## Development Guidelines

### Git Workflow

#### Branch Naming

- `feature/description` - New features
- `bugfix/description` - Bug fixes
- `docs/description` - Documentation updates
- `refactor/description` - Code refactoring

#### Commit Guidelines

- Use present tense ("Add feature" not "Added feature")
- Keep first line under 50 characters
- Use body for detailed explanation
- Reference issues with "Fixes #123"

#### Example Commits

```
Add support for 4D noise generation

Implement simplex_noise_4d() function with proper
gradient handling and interpolation. Includes
comprehensive tests and documentation updates.

Fixes #45
```

### Code Organization

#### File Structure

- Keep related functions together
- Use clear file names
- Separate public and private APIs
- Group constants and enums logically

#### Function Design

- Single responsibility principle
- Clear input/output contracts
- Handle errors gracefully
- Use const where appropriate

#### Example Function Design

```c
/**
 * @brief Generate noise with custom parameters
 * @param x Input x coordinate
 * @param y Input y coordinate
 * @param params Noise parameters
 * @param result Output noise value
 * @return 0 on success, negative error code on failure
 */
int generate_noise_custom(double x, double y,
                         const noise_params_t* params,
                         double* result) {
    if (!params || !result) {
        return -1;  // Invalid parameters
    }

    if (params->scale <= 0.0) {
        return -2;  // Invalid scale
    }

    *result = simplex_noise_2d(x * params->scale, y * params->scale);
    return 0;
}
```

## Release Process

### Version Numbering

- **Major** (1.0.0): Breaking changes
- **Minor** (1.1.0): New features, backward compatible
- **Patch** (1.0.1): Bug fixes, backward compatible

### Release Checklist

- [ ] All tests pass
- [ ] Documentation updated
- [ ] Version numbers updated
- [ ] CHANGELOG.md updated
- [ ] Release notes prepared
- [ ] Tag created
- [ ] Package built and tested

## Community Guidelines

### Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow
- Follow the golden rule

### Communication

- Use clear, professional language
- Be specific in feedback
- Ask questions when unsure
- Share knowledge and experience

### Getting Help

- Check existing documentation first
- Search issues and discussions
- Ask questions in issues
- Join community discussions

## Recognition

### Contributors

- All contributors are recognized in CONTRIBUTORS.md
- Significant contributions get special recognition
- Contributors can become maintainers

### Types of Contributions

- Code contributions
- Documentation improvements
- Bug reports and fixes
- Testing and validation
- Community support

## Questions?

If you have questions about contributing:

- Check existing issues and discussions
- Ask in a new issue
- Contact maintainers directly
- Join community discussions

Thank you for contributing to the Simplex Noise Library!

---

_For more information about the project, see [Overview](overview.md)_
