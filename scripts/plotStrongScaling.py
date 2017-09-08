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
mpl.rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
mpl.rc('text', usetex=True)

#local lib
import mplio

#plot configuration
figureDir='figures'
closePlot=True
plotIndex=0
errBarTransp=0.1


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
    self.dat=dict(list(zip(self.df.values[0,:].T,
      self.df.values[1:,:].T)))
    for confname in ['daint','local']:
      if confname in self.filename:
        self.configName=confname

#open dataset
datas=[]
depths=np.array(0,float)
for el in sys.argv[1::]:
  datas.append(Dat(el))

colors=[]
for i in range(max(len(sys.argv)-1,depths.size)):
  colors.append('C'+str(i))

'''
  Interesting stuff
'''
fig = plt.figure(plotIndex)
ax = fig.add_subplot(111)
for dat,col in zip(datas,colors):
  #get data from csv
  nodes=dat.dat["number-node"].astype(int)
  mint_n=dat.dat["min"].astype(float)
  maxt_n=dat.dat["max"].astype(float)
  medt_n=dat.dat["median"].astype(float)
  #settng up real metric related to weak scaling
  idxSingle=(nodes==1)
  mint_1=mint_n[idxSingle]
  medt_1=medt_n[idxSingle]
  maxt_1=maxt_n[idxSingle]
  miny=(mint_1/(maxt_n*nodes))*100.
  medy=(medt_1/(medt_n*nodes))*100.
  maxy=(maxt_1/(mint_n*nodes))*100.

  x=nodes
  x, miny, maxy, medy = map(np.array,zip(*sorted(zip(x, miny, maxy, medy))))

  #plot
  ax.plot( x, medy, label=dat.name, c=col)
  errBar=plt.fill_between(x, miny, maxy, facecolor=col, interpolate=True)
  errBar.set_alpha(errBarTransp)
  ax.add_collection(errBar)
  
#Bla blah stuff
ax.legend(loc=2,prop={'size':8})
ax.xaxis.set_ticks(np.arange(1, nodes.max()+1, 1))
ax.set_xlabel('Number of cores/nodes')
ax.set_ylabel('Parallel efficiency')
ax.set_title('Strong scaling experiments for the Dijkstra algorithm')
ax.grid(True)
mplio.save("DijkstraStrongScaling", ext="png", close=False)
plt.show()
plotIndex+=1
