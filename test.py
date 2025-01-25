import matplotlib.pyplot as plot
import numpy as np
import os

for file in ['x.test', 'y.test', 'z.test']:

	if not os.path.exists(file):
		continue

	data = np.fromfile(file)
	plot.plot(data, label=file)

plot.legend()
plot.tight_layout()
plot.show()
