import io
import time

import matplotlib.pyplot as plt
from mpl_toolkits.axisartist import Subplot

from math import pi as PI
from math import tau as TAU
from math import e as E

import numpy as np

fig : plt.figure = plt.figure(dpi = 80)
ax = Subplot(fig, 111)

fig.add_subplot(ax)

for direction in ["right", "top"]:
    # hides borders
    ax.axis[direction].set_visible(False)

ax.set_xlabel("Time (s)")

# X axis
xmayorticks : np.ndarray = np.linspace(0.0, 2.0 * PI, 6)
ax.set_xticks(ticks = xmayorticks, labels = [])

# Y axis
ymayorticks : np.ndarray = np.linspace(-1, 1, 3)
ymayorlabels : list[str] = [f'{int(y)}' for y in ymayorticks]
ax.set_yticks(ymayorticks, labels = ymayorlabels)

# X plot
x : np.ndarray = np.linspace(0.0, 2.0 * PI, 100)
line, = ax.plot([], [])

# Limit
ax.set_xlim(-0.25 * PI, 2.25 * PI)
ax.set_ylim(-1.25, 1.25)

def get_png() -> bytes:
    t : float = time.time()

    line.set_data(x, np.cos(x + t))
    
    buffer : io.BytesIO = io.BytesIO()

    fig.savefig(buffer, format = "png")
    buffer.seek(0)

    return buffer.getvalue()
