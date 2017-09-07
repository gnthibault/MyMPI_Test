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

plt.xlabel('Number of links in path')
plt.ylabel('Relative amount')
plt.title('Histogram of SSSP path size')
plt.grid(True)

# Set the ticks to be at the edges of the bins.
ax.set_xticks(bins)
ax.xaxis.set_ticks(np.arange(bins.min(),bins.max(),50))#bins.ptp()/25))
ax.yaxis.set_ticks(np.arange(0, 1, 0.05))
# Set the xaxis's tick labels to be formatted with 1 decimal place...
ax.xaxis.set_major_formatter(FormatStrFormatter('%0.1f'))

# Label the raw counts and the percentages below the x-axis...
#bin_centers = 0.5 * np.diff(bins) + bins[:-1]
perc = (counts.astype(float) / counts.sum())
perc = perc[::-1].cumsum()[::-1]
ax2 = ax.twiny()
ax2.set_xticklabels([])
ax2.plot(perc, 'C1', label="Cumulative sum")
#ax2.tick_params('Relative amount (cumulative)', colors='C1')
#for count, x, percent in zip(counts, bin_centers, perc):
    # Label the raw counts
    #ax.annotate(str(count), xy=(x, 0), xycoords=('data', 'axes fraction'),
    #    xytext=(0, -18), textcoords='offset points', va='top', ha='center')

    # Label the percentages
#    ax.annotate('{:.1f}\%'.format(percent), xy=(x, 0), xycoords=('data', 'axes fraction'),
#        xytext=(0, -32), textcoords='offset points', va='top', ha='center')


# Give ourselves some more room at the bottom of the plot
#plt.subplots_adjust(bottom=0.15)
plt.legend()
plt.show()
