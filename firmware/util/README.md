# RP2040 Trackball Firmware Utilities

Some utilities to help with this project.

## Frame Capture and Visualization

The PMW3360 mouse sensor can do full captures of the raw 2D image seen by the chip.
These can be grabbed from the firmware by opening the debug USB serial port and running the `pmwf` command.
The output should be pasted into a textfile and can then be visualized using the included `visualize_frame.py` script.

## Data Capture and Visualization

The firmware can also grab a number of data samples from the sensor, by running the `pmwd` command.
The output can then be visualized with `visualize_data.py`.
