#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Nov 11 21:56:23 2021

@author: fabian
"""
#Example for how you could use PIPS with Linopy

import sys
import numpy as np
from linopy import Model
import pandas as pd
import xarray as xr
import matplotlib.pyplot as plt
import pypsa
import subprocess


import logging


logging.basicConfig(level=logging.INFO)

#n = pypsa.examples.ac_dc_meshed() #ac_dc_meshed(); storage_hvdc(); scigrid_de() for examples
n = pypsa.Network("/home/ken/Documents/networks/networks/elec_s_37_ec.nc")
#n.snapshots = n.snapshots[:240]
m = n.optimize.create_model()

N = 60
number_of_cores = 1
Filepath = '/tmp/pypsa-model'

blocks = np.repeat(np.arange(1, N + 1), len(n.snapshots) / N)
m.blocks = xr.DataArray(blocks, [n.snapshots])

m.to_block_files(Filepath)

#m.solve(solver_name='gurobi')

#result = subprocess.run(["mpirun", "-np", str(number_of_cores), "../../../build/pipsipmLinopyCallback", str(N), Filepath])

#original_stdout = sys.stdout
#Filepath = '/tmp/pypsa-model/Python_Solution.sol'
#with open(Filepath, 'w') as f:
#    sys.stdout = f 
#    print(m.solution.variables)
#    sys.stdout = original_stdout 
