# Reactor Module

## Overview

This `reactor` module aims to add advanced meshing capabilities to MOOSE so that users can create complex-geometry meshes that are related to reactors without turning to external meshing software.

This module consists of a series of new mesh generators to enable meshing of a hexagonal-geometry reactor core. The functionalities of these mesh generators cover:

- Creating unit hexagon meshes as basic components for the core (other polygon meshes can also be created);
- Stitching the unit hexagon meshes and adding appropriate peripheral regions to form hexagon assembly regions;
- Creating unit hexagon meshes with adaptive external boundaries that are stitch-able with assembly meshes;
- Stitching the assembly meshes and adaptive-boundary unit hexagon meshes to form a core mesh.

## Objects and Syntax

!syntax complete groups=ReactorApp level=3
