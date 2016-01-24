vsetcol(0, 0, 0, 255)
y = 0

repeat
	x = 0
	repeat
		vplot(x, y)
		x = x + 4
	until x > 100
	y = y + 4
until y > 100