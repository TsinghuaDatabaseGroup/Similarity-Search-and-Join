include ../common/flags

all: allpair-token ppjoinplus-token ppjoin-token

allpair-token:
	$(CXX) $(CXXFLAGS) *.cpp -o $@

ppjoinplus-token:
	$(CXX) $(CXXFLAGS) *.cpp -o $@ -DPPJOIN -DPLUS

ppjoin-token:
	$(CXX) $(CXXFLAGS) *.cpp -o $@ -DPPJOIN

clean:
	rm -f *.d *.o
	rm -f allpair-token
	rm -f ppjoinplus-token
	rm -f ppjoin-token

.PHONY: all allpair-token ppjoinplus-token ppjoin-token clean

