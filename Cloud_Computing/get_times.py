import os

for i in range(21, 33):
    header = "echo 'Time for D="+str(i) + "' >> times.txt"
    os.system(header)
    command = "{ time python generate_in_cloud.py -n 5 -d " +str(i) + " ; } 2>> times.txt"
    os.system(command)
