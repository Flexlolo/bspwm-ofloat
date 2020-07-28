CXXFLAGS ?= -O2

PREFIX ?= /usr/local

headers := bspwm-ofloat.h XUtils.h
sources := bspwm-ofloat.cpp XUtils.cpp
objects := $(sources:.cpp=.o)
program := bspwm-ofloat

all: $(program)

$(objects): %.o: %.cpp $(headers)
	$(CXX) $(CXXFLAGS) -Wall -c -o $@ $<

$(program): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -lX11 -ldocopt

clean:
	rm -f $(program) $(objects)

install: $(program)
	install -D -t $(DESTDIR)$(PREFIX)/bin $(program)