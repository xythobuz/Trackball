# Trackball

Custom 3D-printed PC trackball.

See the `hardware` directory for OpenSCAD design files.

The `firmware` directory contains the required Raspberry Pi Pico source code.

Further directions can be found in the READMEs in these directories.

## License

The `firmware` itself is licensed as GPLv3.
It uses the [Pi Pico SDK](https://github.com/raspberrypi/pico-sdk), licensed as BSD 3-clause, and therefore also [TinyUSB](https://github.com/hathach/tinyusb), licensed under the MIT license.
Some code is adapted from the TinyUSB examples.
And the project uses the [FatFS library](https://github.com/abbrev/fatfs), licensed as BSD 1-clause.

The `hardware` design is also licensed as GPLv3.
It uses [this Cherry MX switch model](https://www.thingiverse.com/thing:421524) licensed as CC-BY-SA, and a [Pi Pico case model](https://www.printables.com/model/210898-raspberry-pi-pico-case) licensed as CC-BY-NC-SA.
But both are only used for visualization purposes and don't influence the 3D model at all.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <http://www.gnu.org/licenses/>.
