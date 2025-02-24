CXX = g++
CXXFLAGS = -std=c++17 \
           -I/opt/homebrew/opt/openssl@3/include \
           -I/opt/homebrew/opt/nlohmann-json/include \
           -I/usr/local/opt/nlohmann-json/include \
           -Isrc
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto -lcurl

SRCS = src/main.cpp src/api.cpp src/order_manager.cpp src/config/config.cpp src/SMA_strategy.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = bot

# Add backtest sources and target
BACKTEST_SRCS = tests/backtest_C/backtest.cpp \
                tests/backtest_C/backtester.cpp \
                src/enhanced_strategy.cpp \
                src/order_manager.cpp \
                src/api.cpp \
                src/config/config.cpp
BACKTEST_OBJS = $(BACKTEST_SRCS:.cpp=.o)
BACKTEST_TARGET = backtest

all: $(TARGET) $(BACKTEST_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BACKTEST_TARGET): $(BACKTEST_OBJS)
	$(CXX) $(BACKTEST_OBJS) -o $(BACKTEST_TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BACKTEST_OBJS) $(TARGET) $(BACKTEST_TARGET)

.PHONY: all clean
