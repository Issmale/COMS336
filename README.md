Ray Tracer – Issmale Bekri
Overview

This project implements a ray tracer all required features and addtional features for the required A. The renderer outputs images in PPM format.

Build Instructions

The project uses CMake.

cd build
cmake ..
make -j

How to Run

Run the executable and redirect output to a PPM file:

./raytracer > raytracer.ppm


To preview the result on macOS:

open raytracer.ppm

Notes

Rendering is fully path-traced and may require 1–2 hours or longer depending on scene complexity, sample count, and resolution.

Output is always written to standard output in PPM format.