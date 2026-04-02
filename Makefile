# compiler e flag
CXX = g++
CXXFLAGS = -g -Wall -std=c++11
DEPFLAGS = -MMD -MP # -MMD crea i file .d per le dependency. -MP aggiunge target phony per gli header

# target e file
TARGET = main.exe
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

# obbiettivo di default. crea l'eseguibile e la documentazione
build: $(TARGET)

# per costruire l'eseguibile e chiamare valgrind in un solo comando
valgrind: $(TARGET)
	valgrind ./$(TARGET)

# cancella la cartella build e i suoi contenuti
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
	rm -fr html

# crea l'eseguibile
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# crea gli object file e genera le dipendenze
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# include i file .d
-include $(DEPS)

# obbiettivi dichiarati phony
.PHONY: build valgrind clean
