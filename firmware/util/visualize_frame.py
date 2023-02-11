#!/usr/bin/env python
#
# Copyright (c) 2022 - 2023 Thomas Buck (thomas@xythobuz.de)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# See <http://www.gnu.org/licenses/>.

import matplotlib.pyplot as plt
import numpy as np
import sys
import math
import os

if len(sys.argv) < 2:
    print("Usage:")
    print("    " + sys.argv[0] + " filename [...]")
    sys.exit(0)

fig, axes = plt.subplots(1, len(sys.argv) - 1, figsize=(15, 5))

# support one or multiple files
if not isinstance(axes, np.ndarray):
    axes = [axes]

for n in range(0, len(sys.argv) - 1):
    print("reading " + sys.argv[n + 1])

    frame = []
    if os.path.getsize(sys.argv[n + 1]) == 1296:
        print("binary file format detected. parsing.")
        with open(sys.argv[n + 1], "rb") as f:
            while (byte := f.read(1)):
                frame.append(int.from_bytes(byte, "big"))
    else:
        print("text file format detected. parsing.")
        with open(sys.argv[n + 1]) as f:
            lines = f.readlines()
            for line in lines:
                nums = line.split()
                for r in nums:
                    frame.append(int(r, 16))

    print("frame length: " + str(len(frame)))
    row_len = math.sqrt(len(frame))
    print("row length: " + str(row_len))
    row_len = int(row_len)

    frame2d = []
    for i in range(0, row_len):
        row = []
        for j in range(0, row_len):
            row.append(frame[i * row_len + j])
        frame2d.append(row)

    im = axes[n].imshow(frame2d, vmin=0, vmax=0xFF, cmap='plasma')
    axes[n].set_title(sys.argv[n + 1], fontsize=18)

fig.colorbar(im, ax=axes, label='Value Range')
plt.suptitle('Pan and Zoom on colorbar to adjust', fontsize=10)
plt.show()
