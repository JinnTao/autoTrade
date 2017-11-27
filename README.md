# CodeLib #
## 前期准备 ##
* 1 mongoDb:下载地址：https://www.mongodb.com/download-center
* 2 visual studio 2017 64
* 3 mongoDriver 和 boost 当然如果你有心也可以自己编译
* 4 依赖项目-marketDataCollector： https://github.com/drinkmystery/ctpmarketdatacollector.git  该项目作为行情收集
## 运行 ##
* 1 marketDataCollector ： 一般来说不需要管理，主要实时记录行情（频率-1min） 主力合约数据记录如：rb0000
* 2 codelib:内置几个简单的策略。1，2分开主要保证，运行期间数据收集不会出现断档。
* 3 回测：目前采用vnpy进行回测。参考地址：https://github.com/vnpy/vnpy/tree/master
