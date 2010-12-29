CPP  = g++.exe
LINKOBJ  = tinylog.o $(RES)
BIN  = tinylog.a
CXXFLAGS =
RM = rm -rf

.PHONY: all clean

all: $(BIN)

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

$(BIN): $(LINKOBJ)
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)

%.o: %.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)
