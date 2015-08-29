# Cherenkov code for the MICE Analysis User Software (MAUS).
Each of these files were written by myself.

### The Cherenkov Detector

The Cherenkov (CKOV) Dectector in MICE is used for particle discrimination--it must separate muons and pions very well in a momentum range from 200 to 300 MeV/c. Two detectors, each with 4 photomultiplier tubes (PMTs) are located next to eachother along the beamline. Cherenkov light is produced in a 2.3 cm thick layer of aerogel, and reflected by 4 intersecting conical mirrors into four PMTs. Depending on the momentum of the particle, one, two or neither of the CKOV detectors will light up with varying intensity, thus letting the observer know whether a pion, electron or muon is present.

### Descriptions of the files:

fADCDataProcessor.cc - interprets the raw fADC outputs from the DAQ. The 8 PMTs used in the the Cherenkov detectors use the V1731 fADC. The height and pedestal of each signal is calculated and the peak is integrated using a simple window algorithm.  

MapPyCkov - takes the signals from each event in a spill and combines them to find the total number of photo electrons emitted

ReducePyCkov - combines the momentum information gathered from the Time of Flight (TOF) counters for each event for further analysis.

