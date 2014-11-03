import cymaze
m = cymaze.Maze((10,10))
m.generate()
m.save_bmp('/tmp/test.bmp', (800,600))

import os
os.system('gnome-open /tmp/test.bmp')
