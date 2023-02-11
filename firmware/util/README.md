# RP2040 Trackball Firmware Utilities

Some utilities to help with this project.

## Frame Capture and Visualization

The PMW3360 mouse sensor can do full captures of the raw 2D image seen by the chip.
These can be grabbed from the firmware by opening the debug USB serial port and running the `pmwf` command.
The output should be pasted into a textfile and can then be visualized using the included `visualize_frame.py` script.

## Data Capture and Visualization

The firmware can also grab a number of data samples from the sensor, by running the `pmwd` command.
The output can then be visualized with `visualize_data.py`.

## License

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <http://www.gnu.org/licenses/>.
