from pylab import *
from random import choice
tex = [int(i) for i in open('creat', 'r').read().split("\n")[:-1]]
ptrs = []
poss = []
mema = []
loops = []
ptr = 0
pos = 0
mem = [0 for i in range(128)]
position = [0,0]
rotation = 0
loops_printados = []
def do(v):
	global rotation
	if v == 1:
		print "RC;"
	elif v == 2:
		print "RAC;"
	elif v == 3:
		print "MF;"
		if rotation == 0:
			position[0] += 1
		elif rotation == 1:
			position[1] += 1
		elif rotation == 2:
			position[0] -= 1
		elif rotation == 3:
			position[1] -= 1
		print position
	elif v == 4:
		print "MB;"
		if rotation == 0:
			position[0] -= 1
		elif rotation == 1:
			position[1] -= 1
		elif rotation == 2:
			position[0] += 1
		elif rotation == 3:
			position[1] += 1
	elif v == 5:
		print "REP;"
	elif v == 6:
		print "ATK;"
	elif v == 7:
		print "GIVE;"

for i in range(10000):
	if tex[pos] == 0:
		ptr += 1
		if not pos in loops_printados:
			print 'ptr++;'
			if len(loops):
				loops_printados.append(pos)
	elif tex[pos] == 1:
		ptr -= 1
		if not pos in loops_printados:
			print 'ptr--;'
			if len(loops):
				loops_printados.append(pos)
	elif tex[pos] == 2:
		mem[ptr] += 1
		if not pos in loops_printados:
			print 'memory[ptr]++;'
			if len(loops):
				loops_printados.append(pos)
	elif tex[pos] == 3:
		mem[ptr] -= 1
		if not pos in loops_printados:
			print 'memory[ptr]--;'
			if len(loops):
				loops_printados.append(pos)
	elif tex[pos] == 4:
		loops.append(pos)
		if not pos in loops_printados:
			print 'while(memory[', ptr, ']){'
			if len(loops):
				loops_printados.append(pos)
	elif tex[pos] == 5:
		if not pos in loops_printados and len(loops):
			print '}'
			if len(loops):
				loops_printados.append(pos)
		if(len(loops) and mem[loops[-1]] == 0):
			pos = loops[-1] + 1
		elif(len(loops) and mem[loops[-1]]):
			del loops[-1]
	elif tex[pos] == 6:
		mem[ptr] = choice([0,1])
		if not pos in loops_printados:
			print 'get();'
			if len(loops):
				loops_printados.append(pos)
	elif tex[pos] == 7:
		if not pos in loops_printados:
			print 'Do();'
			if len(loops):
				loops_printados.append(pos)
	ptr %= 128
	#print pos, ptr, mem[ptr], position
	ptrs.append(ptr)
	poss.append(pos)
	mema.append(mem[ptr])
	pos += 1
	pos %= 128
	mem[ptr] %= 8

plot(ptrs)
plot(poss)
plot(mema)
show()
