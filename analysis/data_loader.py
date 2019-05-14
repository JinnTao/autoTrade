#!/home/tao/venv/bin/python python
#  -*- coding: utf-8 -*-


from tqsdk import TqApi, TqSim
from tqsdk.tools import DataDownloader
from datetime import datetime, date
from contextlib import closing

api = TqApi(TqSim())

download_tasks = {}

# 下载从 2017-01-01 到 2018-09-01 的 rb主连 5分钟线数据
download_tasks["rb_1min"] = DataDownloader(api, symbol_list="KQ.m@SHFE.rb", dur_sec=60,
                    start_dt=date(2019, 1, 1), end_dt=date(2019, 5, 13), csv_file_name="rb_1min.csv")

download_tasks["i_1min"] = DataDownloader(api, symbol_list="KQ.m@DCE.i", dur_sec=60,
                    start_dt=date(2019, 1, 1), end_dt=date(2019, 5, 13), csv_file_name="i_1min.csv")

with closing(api):
    while not all([v.is_finished() for v in download_tasks.values()]):
        api.wait_update()
        print("progress: ", {k: ("%.2f%%" % v.get_progress()) for k, v in download_tasks.items()})
