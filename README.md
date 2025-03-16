# Image Morphing Tool

## Overview

This project is an **image morphing application** that generates intermediate frames between a source and a target image using feature-based morphing techniques. It includes:

- A **GUI-based feature selection tool** (`select_features.cpp`) for defining corresponding points or lines between images.
- A **morphing engine** (`morph_images.cpp`, `morph.cpp`) that computes the transformation and generates the in-between frames.
- Support for **OpenGL and GLUT-based visualization** of the morphing process.

The project was developed as part of an **M.Eng project at the University of Victoria**.

---

## Features

- **Graphical Feature Selection**: Define feature points using an interactive GUI.
- **Feature-Based Morphing**: Morph images based on user-defined feature points.
- **Frame Generation**: Output a series of frames for smooth transitions.
- **Animation Display**: View the morphing sequence using OpenGL.
- **Configurable Parameters**: Adjust transformation parameters (a, b, p) for fine control over the morphing effect.

---

## Dependencies

The project requires the following libraries:

- **OpenGL & GLUT** (for rendering and GUI)
- **CGAL** (for geometric computations)
- **SPL** (custom array utilities)
- **Standard C++ Libraries** (math, iostream, vector, string, etc.)

Make sure these dependencies are installed before compiling the project.

---

## Installation & Compilation

### 1. Install Dependencies (Ubuntu example)
```sh
sudo apt-get install libglut-dev freeglut3-dev
sudo apt-get install libcgal-dev
