import cymaze
m = cymaze.Maze((63, 126))
m.generate()
data = m.maze
m.save_bmp('/tmp/test.bmp', (512, 1024))
import os
os.system('gnome-open /tmp/test.bmp')
