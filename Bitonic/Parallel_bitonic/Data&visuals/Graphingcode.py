import matplotlib.pyplot as plt
import numpy as np
import csv
#we make 3 np float 2d arrays of dimension 7 by 10
size=np.zeros((7, 10),dtype=int)

cores_usertime=np.zeros((7, 10), dtype = float)
cores_systime=np.zeros((7, 10), dtype = float)



#open the csv file
with open('MyFile.csv','r') as csvfile:
    #extract all the lines in the csv file
    lines = csv.reader(csvfile, delimiter=',')
    i=0
    j=0
    #for each line, add the output into the appropriate index
    for row in lines:
        if(row[0]=='Num_procs'):
            continue
        elif(int(row[0])==4):
            size[0,j]=int(row[1])
            cores_usertime[0,j]=float(row[3])
            cores_systime[0,j]=float(row[4])

        elif(int(row[0])==8):

            size[1,j]=int(row[1])
            cores_usertime[1,j]=float(row[3])
            cores_systime[1,j]=float(row[4])


        elif(int(row[0])==16):
            size[2,j]=int(row[1])
            cores_usertime[2,j]=float(row[3])
            cores_systime[2,j]=float(row[4])

        elif(int(row[0])==32):
            size[3,j]=int(row[1])
            cores_usertime[3,j]=float(row[3])
            cores_systime[3,j]=float(row[4])

        elif(int(row[0])==64):
            size[4,j]=int(row[1])
            cores_usertime[4,j]=float(row[3])
            cores_systime[4,j]=float(row[4])

        elif(float(row[0])==128):
            size[5,j]=int(row[1])
            cores_usertime[5,j]=float(row[3])
            cores_systime[5,j]=float(row[4])

        elif(float(row[0])==256):
            size[6,j]=int(row[1])
            cores_usertime[6,j]=float(row[3])
            cores_systime[6,j]=float(row[4])

        else:
            print("nothing to do")
        #this allows proper indexing
        #each time we complete 7 rows(because we run each N/P value for 7 different processors), we increment the j index
        i=(i+1)%7
        if(i==0):
            j=j+1
        #print(i,j)


colors=['red','green','blue','yellow','orange','pink','purple',]
names =['4','8','16','32','64','128','256']

#print(cores_usertime[0])
#print(size[0])

#make subplots
fig, ax = plt.subplots()
#fig_systime, ax_systime = plt.subplots()
for i in range(7):
    print(size[i])
    ax.plot(np.log(size[0]),cores_systime[i],color=colors[i],label=names[i])
    #ax_systime.plot(size[i],cores_systime[i])
#plt.sizeticks(rotation = 25)
plt.ylabel('System Runtime')
plt.xlabel('Log(Size values)')
plt.legend(loc='upper left')
plt.title('Runtimes', fontsize = 20)
plt.grid()
plt.show()
