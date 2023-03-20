f = open("out.tr","r")
l = f.readlines()

sum=0
for i in l:
    i = i.split()
    if i[0]=='r' and i[4]=='tcp':
   	 sum += int(i[5])
print(str(sum*8/10000 ) + " kbps")
