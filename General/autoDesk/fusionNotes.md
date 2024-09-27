# Fusion 360

Fusion 360 is a parametric CAD program by Autodesk, which will be used quite a bit during the program. Most of the notes in this document can be found at this link:

* https://gixlabs.github.io/how_to/fusion.html

The following sections explore the bare minimum to get started with the program.

### General Hotkeys

Here are some general hotkeys you can use in Fusion:

* `S` - Search for a command

## Sketches

Sketches are the underlying geometry that support the creation of 3D solid, surface, and T-Spline bodies in a design.

To access the sketch environment, first create a sketch using the first button in the `CREATE` group of the `SOLID` menu up top. This will open up a 3D plane. Once you select one of the faces, it will open the 2D view, and the Sketch menu will open up top.

The `Sketch` menu is organized by a typical workflow, going from `CREATE` to `MODIFY` to `CONSTRAINTS` etc.

### CREATE

The create group is where you can find a bunch of basic shapes such as lines, circles, and text.

When you create a solid, the initial point will have a constraint included as a default. If you click the start point, the relative constraint symbol appears. You can click it to to see some details and delete it if needed.

Tips for creating:

* Hovering over an endpoint will allow you to start the next solid there. Note that an auto-constraint is created when you do this
* To close an open shape, click two open vertexes and it will close the shape

Here are some examples and their hotkeys:

* Line (`L`)
* Rectangle (`R`)
* Circle (`C`)
* Dimension (`D`)
    * If you click another dimension when prompted to enter a value, you will link the new dimension to the previously created one
    * This allows you to create formulas for certain dimensions, which will be discussed further later

---

#### Construction Lines

You may want to create construction lines when sketching. One way to do this is to make lines on the x and y axes below your 2D sketch.

Once they are created, you will see they divide up your current model. Now, select these lines, and change the linetype to "construction" on the `SKETCH PALETTE` (or press `x`). Now, these lines can freely move and they do not affect the extrusion of your model.

Now, let's add a MidPoint constraint by right-clicking each endpoint to the edges of the sketch. You can now use this to constrain your design!

### MODIFY

The modify group has tools such as Fillet, Trim, or Offset.

Here are some of these tools and how to use them:

* Fillet - On 2D, rounds the corners

### CONTRAINTS

The constraints group allows you to control the design and avoid unwanted errors. 

Make sure that Dimensions and Constraints is checked to see these on the drawing.

Here are some of the constraints and what you can do:

* Horizontal/Vertical (ruler icon): Locks the line in a horizontal or vertical fashion
* Coincident: Select an endpoint, and select a line
* Tangent (circle with tan line icon): Locks two objects to be tangent to each other


### Sketch Palette

The Sketch Palette has a bunch of different options to help with your workflow.

### Extrude

You can extrude a part of a sketch using the extrude tool in the `Create` tab.

When extruding, there are two types; a normal extrusion that makes a solid, and a `thin` extrusion that is hollow / has a wall around the outer border.

You can change a lot of things while extruding, such as the taper angle, 

## Components

Let's say you want to design a camera case, but need to design the camera itself first. This is where you would use a component!

To create a component, right click the top-level node (top left area, likely says `Unsaved`) and select `New Component`.

This opens a dialogue box with a few options that you can toggle:

* Sketch Grid:
* Snap:
* Slice:
* Profile:
* Points:
* Dimensions:
* Constraints:
* Projected Geometries:
* Construction Geometries:
* 3D Sketch:
