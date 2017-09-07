import sys
import shutil
import numpy as np
import pandas as pd
import csv

if (len(sys.argv)<2):
  print("Usage is ./collator filename")
  print("All columns but the last one will be used as indices")
  print("Datafile must contain a header (otherwise first row ignored)")
  sys.exit()

nbIndexingCol = sys.argv
tmpinputfile = sys.argv[1]
inputfile=tmpinputfile+".sav"
shutil.copyfile(tmpinputfile,inputfile)
outputfile = tmpinputfile

# Reading output
aggregatedData = {}
header = list(pd.read_csv(inputfile, nrows=1).columns)
dataFrame=pd.read_csv(inputfile, header=1)

for row in dataFrame.itertuples():
  key=",".join(list(map(str,list(row[1:-1]))))
  val=row[-1]
  if key in aggregatedData:
    aggregatedData[key].append(val)
  else:
    aggregatedData[key]=[val]

# Writing output
with open(outputfile,"wt") as out:
  header = header[0:-1]
  header.extend(["avg", "max", "median", "min", "standard deviation"])
  writer = csv.writer(out)
  writer.writerow(header)
  for key,data in aggregatedData.items():
    min_gflops = np.round(np.min(data),3)
    max_gflops = np.round(np.max(data),3)
    median_gflops = np.round(np.median(data),3)
    avg_gflops = np.round(np.average(data),3)
    std_dev = np.round(np.std(data),3)
    data2=[avg_gflops, max_gflops, median_gflops, min_gflops, std_dev]
    writer.writerow(key.split(",")+data2)
