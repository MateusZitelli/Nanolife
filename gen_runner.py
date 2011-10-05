#Nanolife - Simple artificial life simulator

#Copyright 2011 Mateus Zitelli <zitellimateus@gmail.com>

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#MA 02110-1301, USA.
from random import choice
prog = [int(i) for i in open('creat', 'r').read().split(", ")[:-1]]
times = [0 for i in range(len(prog))]
ptr = 0
pos = 0
memory = [0 for i in range(len(prog))]
loops = []
position = [250,250]
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
	elif v == 8:
		print "SEX_REP;"

for i in range(1000):
	if ptr >= len(prog) - 1 or pos >= len(prog) - 1: break
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
		memory[ptr] = choice([0,1])
		if(memory[ptr]): print "Compatible bot in front at", ptr
		else: print "Imcompatible or null cell at", ptr
	elif prog[pos] == 7:
		memory[ptr] = rotation
		print 'Rotation =', rotation, 'at', ptr
	elif prog[pos] == 8:
		do(memory[ptr])
		#print 'Do();'
	times[pos] += 1
	pos += 1
	#memory[ptr] %= 9
	#print memor
print prog
print times

