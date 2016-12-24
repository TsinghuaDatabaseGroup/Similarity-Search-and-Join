include ../../common/flags

all: mtree-ed

mtree-ed:
	$(CXX) $(CXXFLAGS) *.cpp ../GiST/*.cpp -o $@ -I../GiST -DUNIX -DPRINTING_OBJECTS

clean:
	rm -f *.d *.o
	rm -f mtree-ed

.PHONY: all mtree-ed clean

