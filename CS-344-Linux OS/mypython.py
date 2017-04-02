#mypython.py by Spencer Moran.
#class cs344
#date 3/4/2017
import random
random.seed()

#assignment file i/o

#file1 i/o operations
file1 = open("file1.txt", "w+")		#open file 1 write 10 random chars
for num in range(0,10):
	randnum = random.randint(97,122)
	file1.write(chr(randnum))

file1.write("\n")
file1.close()

file1 = open("file1.txt", "r")		#read it back
contents = file1.read(10)
print (contents)
file1.close()

#file2 i/o operations
file2 = open("file2.txt", "w+")		#same as above for both file 2 and file 3
for num in range(0,10):
	randnum = random.randint(97,122)
	file2.write(chr(randnum))

file2.write("\n")
file2.close()

file2 = open("file2.txt", "r")
contents = file2.read(10)
print (contents)
file2.close()

#file3 i/o operations
file3 = open("file3.txt", "w+")
for num in range(0,10):
	randnum = random.randint(97,122)
	file3.write(chr(randnum))

file3.write("\n")
file3.close()

file3 = open("file3.txt", "r")
contents = file3.read(10)
print (contents)
file3.close()

#numbers portion
randnum1 = random.randint(1,43)		#generate 2 randum numbers
randnum2 = random.randint(1,43)
print (randnum1)			#print the numbers and their product.
print (randnum2)
print (randnum1*randnum2)
