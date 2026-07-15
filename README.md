# gasket_measurer
Device for measuring gasket (and other uniform, grippy, and flexible materials)

Designed for use at Great Land Window for measuring window gasket. The device uses a raspberry pi pico with a rotary encoder, two-position rocker switch, 4-digit 7 segment display, and a push button all connected to a 3D-printed housing.

The software utilizes hardware interrupts to ensure accurate measuring where one of the pi pico's cores is dedicated to driving the display while the other handles the rest of the functions. The display supports both SI (meters) and USC (feet) units, both to +/- 0.1 precision. Due to limited space on the display, negative values will drop the negative sign when the number is three digits long (i.e. 4 digits total including the tenths place).

The 3D model is provided both as the raw FreeCAD file as well as a .step for 3D printing (you will have to split the .step into objects to orient them properly on the build plate; in Prusa slicer this would be right click > split > to objects).
