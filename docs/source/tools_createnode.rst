Create Node Tools
=================

.. figure:: images/tools_shelf_icons_creation.png
    :alt: Node Creation Shelf Icons
    :align: center
    :width: 40%

Creating various nodes and objects with mmSolver uses the tools below.

.. _create-marker-ref:

Create Marker
-------------

Create a default `Marker` node under the active viewport's `Camera`.

Usage:

1) Click in a Maya 3D viewport, to activate the `Camera`.

2) Run the `Create Marker` tool, using the shelf, menu or marking menu.

   - A `Marker` will be created in the center of the viewport.

.. note::
    Default Maya cameras (such as ``persp``, ``top``, ``front``, etc)
    are not supported to create Markers.

.. note:: To create Markers, with 2D data from external 3D software
    use the :ref:`Load Markers <load-markers-ref>` tool.

Run this Python command:

.. code:: python

    import mmSolver.tools.createmarker.tool as tool
    tool.main()

.. _create-bundle-ref:

Create Bundle
-------------

Create a default `Bundle` node.

If `Markers` are selected, the `Bundle` will attempt to attach to it,
while adhering to the rule; *a bundle can only have one marker
representation for each camera.*

Run this Python command:

.. code:: python

    import mmSolver.tools.createbundle.tool as tool
    tool.main()

.. _create-line-ref:

Create Line
-----------

Create a 2D `Line` node, made up of `Markers` and `Bundles`. The `Line`
defines a straight line between all `Markers` on the `Line`.

Adjust the Marker positions to move the `Line`.

.. note:: To display a straight line select the ``mmLineShape`` shape
   node and enable `Draw Middle`.

Run this Python command:

.. code:: python

    import mmSolver.tools.createline.tool as tool
    tool.main()

.. _create-camera-ref:

Create Camera
-------------

Create a default Camera.

Run this Python command:

.. code:: python

    import mmSolver.tools.createcamera.tool as tool
    tool.main()

.. _create-lens-ref:

Create Lens
-----------

Create a `Lens` (distortion) node.

The `Lens` node is used to distort all `Markers` attached to the
`Camera` similar to how imperfections in physical lenses distorts
light (mostly at the edges) of an image.

Usage:

1) Activate 3D viewport or select camera node(s).

2) Run the `Create Lens` tool, using the shelf or menu.

3) Adjust lens node attributes as needed, or load a lens with the
   :ref:`Load Markers in Maya <load-markers-in-maya-ref>`.

Run this Python command:

.. code:: python

    import mmSolver.tools.createlens.tool as tool
    tool.main()

.. _create-imageplane-ref:

Create ImagePlane
-----------------

Create a MM Solver ImagePlane node, with the chosen image file
(sequence).

.. note:: The image plane supports any image format supported by
   Maya's ``file`` node, but can be buggy when reading image
   sequences.

.. figure:: images/tools_create_mm_image_plane.png
    :alt: MM Image Plane
    :align: center
    :width: 90%

Usage:

1) Activate 3D viewport or select camera node(s).

2) Run the `Create ImagePlane` tool, using the shelf or menu.

3) Browse to image file.

   - If the image is named as an image sequence (such as
     ``file.#.ext``), it will be detected and the full image sequence
     will be loaded.

Run this Python command:

.. code:: python

    import mmSolver.tools.createimageplane.tool as tool
    tool.main()
