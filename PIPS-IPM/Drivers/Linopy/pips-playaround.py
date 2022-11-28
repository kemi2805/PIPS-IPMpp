#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Nov 11 21:56:23 2021

@author: fabian
"""
#Example for how you could use PIPS with Linopy
from linopy import Model
import numpy as np
import pandas as pd
import xarray as xr
import matplotlib.pyplot as plt
import pypsa
import subprocess
import sys

import logging

logging.basicConfig(level=logging.INFO)

n = pypsa.examples.ac_dc_meshed() #ac_dc_meshed(); storage_hvdc(); scigrid_de() for examples
m = n.optimize.create_model()

N = 10
number_of_cores = 4
blocks = np.repeat(np.arange(1, N + 1), len(n.snapshots) / N)
m.blocks = xr.DataArray(blocks, [n.snapshots])

Filepath = '/tmp/pypsa-model'
m.to_block_files(Filepath)

m.solve()
original_stdout = sys.stdout

result = subprocess.run(["mpirun", "-np", str(number_of_cores), "/home/ken/Desktop//PIPS-Git/PIPS-IPMpp/build/pipsipmLinopyCallback", str(N), Filepath])

Filepath = '/tmp/pypsa-model/Python_Solution.sol'
with open(Filepath, 'w') as f:
    sys.stdout = f 
    print(m.solution.variables)
    sys.stdout = original_stdout 
