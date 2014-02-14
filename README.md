petulant-beard-archer
=====================

Construct 3d meshes from a series of images using 3d constraints

Based heavily on http://insight3d.sourceforge.net/

### insight3d

Advantages:

- Very usable GUI
- Open source
- Automatically detects correspondence on the fly
- Can build own 3d polygons in 2d images
- Readable file format
- Works

Disadvantages:

- Uses OpenCV legacy highgui
- Lots of pointers leads to a few bugs and crashes
- Originally designed to work with MSVC
- Hard to compile

Some features I'd like:

- Ability to manually edit correspondences
- Ability to constrain triangulation
- Know if a calculation is underconstrained or badly conditioned and show user how.
- Probabilistic triangulation
- Probabilistic calibration
- Lots of abstraction and TMP

### Constrained triangulation algorithm

We'll find out after this is built, but I think I figured out how to triangulate with certain linear constraints.

### Contributing

Send me a pull request :)
