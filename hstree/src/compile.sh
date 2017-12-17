g++ -O3 -c -DNDEBUG -o free.o free.cc
g++ -O3 -c -DNDEBUG -o io.o io.cc
g++ -O3 -c -DNDEBUG -o main.o main.cc
g++ -O3 -c -DNDEBUG -o prepare.o prepare.cc
g++ -O3 -c -DNDEBUG -o hstopk.o hstopk.cc
g++ -O3 -c -DNDEBUG -o hsimSearch.o hsimSearch.cc
g++ -O3 -o hstree free.o io.o main.o prepare.o hstopk.o hsimSearch.o
rm *.o
