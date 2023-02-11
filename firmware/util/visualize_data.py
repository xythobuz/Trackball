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

if len(sys.argv) < 2:
    print("Usage:")
    print("    " + sys.argv[0] + " filename")
    sys.exit(0)

fig, axes = plt.subplots(6, 1, figsize=(5, 15))

data = []
lines_heading = []
with open(sys.argv[1]) as f:
    lines = f.readlines()
    lines_heading = lines[0].split(',')
    lines_data = lines[1:]
    for line in lines_data:
        nums = line.split(',')
        line_data = []
        for r in nums:
            line_data.append(int(r))
        data.append(line_data)

start_time = data[0][0]
for i in range(0, len(data)):
    data[i][0] = (data[i][0] - start_time) / 1000.0 / 1000.0

print("samples: " + str(len(data)))

x = [ r[0] for r in data ]

for n in range(3, 9):
    y = [ r[n] for r in data ]
    im = axes[n - 3].plot(x, y)
    axes[n - 3].set_title(lines_heading[n])

plt.suptitle(sys.argv[1], fontsize=18)
plt.tight_layout()
plt.show()
