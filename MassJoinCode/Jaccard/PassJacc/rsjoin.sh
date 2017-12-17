#!/bin/bash

export CLASSPATH=.:$HADOOP_HOME/hadoop-core-1.0.4.jar

ROOT_PATH=`dirname $0`
CURRENT_PATH=`pwd`

cd $ROOT_PATH
rm -rf bin
rm -rf target
mkdir bin
mkdir target

cd src
find . -name *.java | xargs javac -d ../bin 

cd ..
jar cvf target/passjoin.jar -C bin/ . > /dev/null 

hadoop fs -rmr group
hadoop fs -rmr record1
hadoop fs -rmr record2
hadoop fs -rmr segment
hadoop fs -rmr substring
hadoop fs -rmr bloomfilter
hadoop fs -rmr length1
hadoop fs -rmr length2

hadoop fs -rmr unused
hadoop fs -rmr candidates
hadoop fs -rmr middle
hadoop fs -rmr result

hadoop jar target/passjoin.jar hadoop.PassJoin

cd $CURRENT_PATH
