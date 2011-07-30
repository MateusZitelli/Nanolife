prog = [int(i) for i in open('creat', 'r').read().split("\n")[:-1]]
ptr = 0
pos = 0
memory = [0 for i in range(len(prog))]
loops = []
position = [0,0]
rotation = 0
def do(v):
	global rotation
	if v == 1:
		rotation = (rotation + 1) % 4
		print "RC", rotation ,";"
	elif v == 2:
		rotation = (rotation - 1) % 4
		print "RAC", rotation ,";"
	elif v == 3:
		print "MF",
		if rotation == 0:
			position[0] += 1
		elif rotation == 1:
			position[1] += 1
		elif rotation == 2:
			position[0] -= 1
		elif rotation == 3:
			position[1] -= 1
		print position, ';'
	elif v == 4:
		print "MB",
		if rotation == 0:
			position[0] -= 1
		elif rotation == 1:
			position[1] -= 1
		elif rotation == 2:
			position[0] += 1
		elif rotation == 3:
			position[1] += 1
		print position, ';'
	elif v == 5:
		print "REP;"
	elif v == 6:
		print "ATK;"
	elif v == 7:
		print "GIVE;"

for i in range(200):
	if ptr >= len(prog) or pos >= len(prog): break
	if prog[pos] == 0:
		ptr += 1
		#print 'ptr++;'
	elif prog[pos] == 1:
		ptr -= 1
		#print 'ptr--;'
	elif prog[pos] == 2:
		memory[ptr] += 1
		#print 'memory[ptr]++;'
	elif prog[pos] == 3:
		memory[ptr] -= 1
		#print 'memory[ptr]--;'
	elif prog[pos] == 4:
		if(memory[ptr]):
			loops.append((pos, ptr))
		#print 'while(memory[ptr]){'
	elif prog[pos] == 5:
		if(len(loops) and memory[loops[-1][1]]):
			pos = loops[-1][0]
		elif(len(loops)):
			del loops[-1]
		#print '}'
	elif prog[pos] == 6:
		do(memory[ptr])
		#print 'Do();'
	pos += 1
