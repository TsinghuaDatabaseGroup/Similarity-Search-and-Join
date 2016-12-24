include ../../common/flags

all: listmerger-ed listmerger-token

listmerger-ed: merger-ed.cc
	$(CXX) $(CXXFLAGS) merger-ed.cc ../common/*.cc ../util/*.cc -o $@

listmerger-token: merger-token.cc
	$(CXX) $(CXXFLAGS) merger-token.cc ../common/*.cc ../util/*.cc -o $@

clean:
	rm -f *.d *.o
	rm -f listmerger-ed listmerger-token


