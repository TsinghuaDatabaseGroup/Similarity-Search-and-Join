g++ -O3 -c -DNDEBUG -o storage.o storage.cc
g++ -O3 -c -DNDEBUG -o main.o main.cc
g++ -O3 -c -DNDEBUG -o index.o index.cc
g++ -O3 -c -DNDEBUG -o search.o search.cc
g++ -O3 -c -DNDEBUG -o compact.o compact.cc
g++ -O3 -o hstree-disk storage.o main.o index.o search.o compact.o
rm *.o
