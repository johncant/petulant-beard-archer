petulant-beard-archer
=====================

Real name yet to be decided

Based heavily on http://insight3d.sourceforge.net/

Construct 3d meshes from a series of images using 3d constraints

Features WIP so far
-------------------

*   GUI in Gtk (Gtkmm), using Glade and imported with Gtk::Builder
*   Insight3d parser
    built using boost::spirit
    Expects no polygons
*   2d image point editor. The image gets locally zoomed around the cursor, so the user never needs to pan. The ROI is always at the chosen zoom, and you never have to pan the picture. This means that the left and right mouse buttons are free for actual editing.
*   Correspondence editor. This is required to refine correspondences detected by automatic methods.

Features ready so far
---------------------

*   none yet really :)

Motivation for rewriting insight3d rather than extending it
-----------------------------------------------------------

Advantages of insight3d:

- Very usable GUI
- Open source
- Automatically detects correspondence on the fly
- Can build own 3d polygons in 2d images
- Readable file format
- Works

Disadvantages of insight3d:

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

Constrained triangulation algorithm
-----------------------------------

We'll find out after this is built, but I think I figured out how to triangulate with certain linear constraints. This will hopefully be in v1 :)

#### Other algorithmic goals

*   Use constraints to find projection matrices more accurately
*   Allow more complicated constraints
*   Fuzzy reconstruction
*   Infer textures and material properties from images of the lit object

Contributing
------------

Send me a pull request :)
