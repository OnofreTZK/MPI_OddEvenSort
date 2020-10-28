import sys # argv
import numpy as np

# Reading -----------------------------------------------
try:
    time_file = open( sys.argv[1], "r" )

except IOError:
    print("Arquivo inválido!\n")


time_list = time_file.readlines()

time_file.close()


# Getting means -----------------------------------------

def arithmetic_mean(numbers): # }}}
    return float( sum(numbers) ) / max(len(numbers), 1 )
#{{{

temp_mean = [] # temporaty list to calc the mean.

final_means = [] # final list with all 12 means.

for time in time_list:
    
    temp_mean.append( float(time) )
    
    if len(temp_mean) == 5 :
        final_means.append( arithmetic_mean( temp_mean ) )
        temp_mean.clear()





# Writing -----------------------------------------------

if '/' in sys.argv[1]:
    final_file = open( "mean_" + sys.argv[1].split('/')[-1], "w+" )
else:
    final_file = open( "mean_" + sys.argv[1], "w+" )


for i in range( len(final_means) ):
    final_file.write( "{0:.3f}\n".format(final_means[i]) )

final_file.close()



