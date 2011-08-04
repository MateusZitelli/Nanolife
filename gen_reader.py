tex = [int(i) for i in open('creat', 'r').read().split(", ")[:-1]]

for i in tex:
	if i == 0:
		print 'ptr++;'
	elif i == 1:
		print 'ptr--;'
	elif i == 2:
		print 'memory[ptr]++;'
	elif i == 3:
		print 'memory[ptr]--;'
	elif i == 4:
		print 'while(memory[ptr]){'
	elif i == 5:
		print '}'
	elif i == 6:
		print 'Do();'
