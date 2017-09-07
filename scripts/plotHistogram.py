#script arguments, path
import sys
import os

#import data
import pandas as pd

#numpy for array, matrix, ...
import numpy as np

#For plotting results
import matplotlib.pyplot as plt 
import matplotlib as mpl
from matplotlib.ticker import FormatStrFormatter
mpl.rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
mpl.rc('text', usetex=True)

#local lib
import mplio

'''

'''
class Dat(object):
  filename=""
  df=[]
  dat={}
  configName=''

  def __init__(self, filename):
    self.filename=filename
    self.name=os.path.splitext(os.path.basename(filename))[0]
    self.df=pd.read_csv(self.filename, sep=',',header=None)

#open dataset
datas=[]
depths=np.array(0,float)
for el in sys.argv[1::]:
  datas.append(Dat(el))

#Print simple histogram for datas[0]
dat=datas[0].df.values[1:,1].astype(np.int)
nbins=dat.ptp()
#np.unique(x).size
fig, ax = plt.subplots()
counts, bins, patches = plt.hist(dat, nbins, normed=1, facecolor='g', alpha=0.75)

plt.xlabel('Number of endpoints')
plt.ylabel('Relative amout(in \%)')
plt.title('Histogram of SSSP problem size')
plt.grid(True)

# Set the ticks to be at the edges of the bins.
ax.set_xticks(bins)
# Set the xaxis's tick labels to be formatted with 1 decimal place...
ax.xaxis.set_major_formatter(FormatStrFormatter('%0.1f'))

# Label the raw counts and the percentages below the x-axis...
bin_centers = 0.5 * np.diff(bins) + bins[:-1]
for count, x in zip(counts, bin_centers):
    # Label the raw counts
    #ax.annotate(str(count), xy=(x, 0), xycoords=('data', 'axes fraction'),
    #    xytext=(0, -18), textcoords='offset points', va='top', ha='center')

    # Label the percentages
    percent = (100 * float(count) / counts.sum())
    ax.annotate('{:.1f}'.format(percent), xy=(x, 0), xycoords=('data', 'axes fraction'),
        xytext=(0, -32), textcoords='offset points', va='top', ha='center')


# Give ourselves some more room at the bottom of the plot
plt.subplots_adjust(bottom=0.15)
plt.show()
