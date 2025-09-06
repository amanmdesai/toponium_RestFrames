import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.axes_grid1 import make_axes_locatable

plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['font.size'] = 20
plt.rcParams['axes.titlesize'] = 20
plt.rcParams['axes.titleweight'] = 'bold'

# Data
maximum = np.array([
    [0.026, 0.042, 0.033],
    [0.044, 0.102, 0.042],
    [0.054, 0.165, 0.054]
])


data = np.array([
    [0.6599883803020188, 0.6530838964932151, 0.6534428660764171],
    [1.4202205999807342, 2.2987327773726776, 1.4078827038848876],
    [1.736308974035721,  4.111014282162971,  1.7286368956779798]
])

xlabels = ["[-6,-2]", "[-2,2]", "[2,6]"]
ylabels = ["[-1,-0.4]", "[-0.4,0.4]", "[0.4,1]"]

fig, ax = plt.subplots(figsize=(12,12))
cax = ax.matshow(data, cmap="Blues")

# Add text labels (bold, readable)
for (i, j), val in np.ndenumerate(data):
    color = 'black' if val < data.max() * 0.6 else 'white'
    ax.text(j, i, f"{val:.2f}", ha="center", va="center",
            color=color, fontweight='bold')

ax.set_xticks(np.arange(len(xlabels)))
ax.set_yticks(np.arange(len(ylabels)))
ax.set_xticklabels(xlabels, fontsize=28)
ax.set_yticklabels(ylabels, fontsize=28)

ax.set_xlabel(r"$\Delta\phi(t\bar{t})$", fontsize=32)
ax.set_ylabel(r"$N_{\rm chel}$", fontsize=32)

ax.set_title(r"RestFrames $M_{\rm top}^{a} = M_{\rm top}^{b}$", fontsize=32, pad=20)


# Create colorbar with EXACT same height as plot
divider = make_axes_locatable(ax)
cax_cb = divider.append_axes("right", size="5%", pad=0.1)
cbar = fig.colorbar(cax, cax=cax_cb)
cbar.ax.tick_params(labelsize=24)
cbar.set_label(r"Significance", fontsize=28)
#cbar.set_label(r"Ratio of Toponium$/t\bar{t}$", fontsize=28)

plt.tight_layout()
plt.savefig('grid.png', bbox_inches='tight', pad_inches=0)
plt.show()
