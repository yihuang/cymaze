import cymaze
m = cymaze.Maze(63)
for i in range(100):
    m.generate()
    m.save_bmp('/tmp/test%d.bmp'%i, 512)
