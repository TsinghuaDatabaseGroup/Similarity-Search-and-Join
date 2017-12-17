# Author : jnwang
# Email  : jnwangir@gmail.com
# Date   : 2011-02-06
# Project: FastJoin


include ../makefile.inc

FastJoinCmd.o: FastJoinCmd.cpp FastJoin.o \
	TokenSigScheme/TokenSigSchemeFactory.o TokenSigScheme/TokenSigScheme.o \
	TokenSetSigScheme/JaccardTokenSetSigScheme.o TokenSetSigScheme/DiceTokenSetSigScheme.o TokenSetSigScheme/CosinTokenSetSigScheme.o\
	Tokenizer/StandardTokenizer.o
	$(CC) -c -o $@ $(CPPFLAGS) $<

#TestFastJoin.o: TestFastJoin.cpp FastJoin.o \
#	TokenSigScheme/TokenSigSchemeFactory.o TokenSigScheme/TokenSigScheme.o \
#	TokenSetSigScheme/JaccardTokenSetSigScheme.o TokenSetSigScheme/DiceTokenSetSigScheme.o TokenSetSigScheme/CosinTokenSetSigScheme.o\
#	Tokenizer/StandardTokenizer.o
#	$(CC) -c -o $@ $(CPPFLAGS) $<

FastJoin.o: FastJoin.cpp FastJoin.h SignatureIndex.o\
	TokenSetSigScheme/TokenSetSigScheme.o \
	Tokenizer/Tokenizer.o \
	Utility/GlobFunc.o
	$(CC) -c -o $@ $(CPPFLAGS) $<
	
SignatureIndex.o: SignatureIndex.cpp SignatureIndex.h Utility/TypeDefs.o
	$(CC) -c -o $@ $(CPPFLAGS) $<
	
.PHONY:	clean
clean:
	-rm *.o
