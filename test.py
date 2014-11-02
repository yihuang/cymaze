import cymaze
m = cymaze.Maze(63)
m.generate()
data = m.maze
m.save_bmp('/tmp/test.bmp', 512)
import os
os.system('open /tmp/test.bmp')
