#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
# IC Spread analysis
# file_name1 = 'ic1905_1min.csv'
# file_name2 = 'ic1906_1min.csv'
#
# cols_1 = 'CFFEX.IC1905.close'
# cols_2 = 'CFFEX.IC1906.close'

'''
    I-rb Spread analysis
'''
file_name1 = 'i_1min.csv'
file_name2 = 'rb_1min.csv'

cols_1 = 'KQ.m@DCE.i.close'
cols_2 = 'KQ.m@SHFE.rb.close'


df_leg1 = pd.read_csv(file_name1)
df_leg2 = pd.read_csv(file_name2)

df_leg1.set_index('datetime',inplace=True)
df_leg2.set_index('datetime',inplace=True)

figure = plt.figure()
#print (df_leg1.columns)
spread_prc = df_leg1.loc[:,cols_1] - df_leg2.loc[:,cols_2]
spread_prc.plot()
plt.show()