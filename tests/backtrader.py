import backtrader as bt

class TestStrategy(bt.Strategy):
    def next(self):
        if self.data.close[0] > self.data.close[-1]:  # Example condition
            self.buy()
        elif self.data.close[0] < self.data.close[-1]:
            self.sell()

cerebro = bt.Cerebro()
data = bt.feeds.GenericCSVData(dataname="your_data.csv")  # Load data
cerebro.adddata(data)
cerebro.addstrategy(TestStrategy)
cerebro.run()
cerebro.plot()
