CXX = g++
CXXFLAGS = -std=c++17 \
           -I/opt/homebrew/opt/openssl@3/include \
           -I/opt/homebrew/opt/nlohmann-json/include \
           -I/usr/local/opt/nlohmann-json/include
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto -lcurl

SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/api.cpp $(SRCDIR)/order_manager.cpp $(SRCDIR)/config/config.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = bot

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
