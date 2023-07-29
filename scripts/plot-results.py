#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv(sys.stdin)

df[["benchmark", "case", "x"]] = df["name"].str.split("/", expand=True)

ax = plt.axes()

for case_label, case_df in df.groupby("case"):
    case_df.plot(ax=ax, x="x", y="real_time", label=case_label)

plt.show()
