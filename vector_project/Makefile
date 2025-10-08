CXX = g++
CXXFLAGS = -std=c++11 -fPIC -Wall
LDFLAGS = -ldl

# Цели
TARGET = main
STATIC_LIB = libnumber.a
DYNAMIC_LIB = libvector.so

# Исходные файлы
NUMBER_SRC = number.cpp
NUMBER_OBJ = number.o
VECTOR_SRC = vector.cpp
VECTOR_OBJ = vector.o
MAIN_SRC = main.cpp

.PHONY: all clean

all: $(STATIC_LIB) $(DYNAMIC_LIB) $(TARGET)

# Статическая библиотека Number
$(STATIC_LIB): $(NUMBER_OBJ)
	ar rcs $@ $^

$(NUMBER_OBJ): $(NUMBER_SRC) number.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Динамическая библиотека Vector
$(DYNAMIC_LIB): $(VECTOR_OBJ) $(STATIC_LIB)
	$(CXX) -shared -o $@ $(VECTOR_OBJ) -L. -lnumber

$(VECTOR_OBJ): $(VECTOR_SRC) vector.h number.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Исполняемый файл
$(TARGET): $(MAIN_SRC) $(STATIC_LIB) $(DYNAMIC_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_SRC) -L. -lnumber -lvector -Wl,-rpath,.

clean:
	rm -f $(TARGET) $(STATIC_LIB) $(DYNAMIC_LIB) *.o

test: all
	./$(TARGET)
