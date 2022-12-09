class A:
	def __init__(self, value):
		self.value = value

def multiply(a, b):
	print("Will compute", a, "times", b)
	c = A(0)
	for i in range(0, a):
		c.value += b
	return c.value
