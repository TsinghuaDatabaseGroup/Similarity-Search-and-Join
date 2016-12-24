all: ed token

ed: adaptjoin allpair-ed bitriejoin-ed edjoin-ed fastss-ed partenum-ed passjoin-ed triejoin-ed qchunk-indexchunk-ed qchunk-indexgram-ed listmerger mtree-ed | bin

token: adaptjoin partenum-token passjoin-token ppjoin-token listmerger | bin

bin:
	rm -rf bin
	mkdir bin

adaptjoin:
	+make -C ./adaptjoin
	+cp ./adaptjoin/adaptjoin ./bin

allpair-ed:
	+make -C ./allpair-ed
	+cp ./allpair-ed/allpair-ed ./bin

bitriejoin-ed:
	+make -C ./bitriejoin-ed
	+cp ./bitriejoin-ed/bitriejoin-ed ./bin

edjoin-ed:
	+make -C ./edjoin-ed
	+cp ./edjoin-ed/edjoin-ed ./bin

fastss-ed:
	+make -C ./fastss-ed
	+cp ./fastss-ed/fastss-ed ./bin

partenum-ed:
	+make -C ./partenum-ed
	+cp ./partenum-ed/partenum-ed ./bin

partenum-token:
	+make -C ./partenum-token
	+cp ./partenum-token/partenum-token ./bin

passjoin-ed:
	+make -C ./passjoin-ed
	+cp ./passjoin-ed/passjoin-ed ./bin

passjoin-token:
	+make -C ./passjoin-token
	+cp ./passjoin-token/passjoin-token ./bin

triejoin-ed:
	+make -C ./triejoin-ed
	+cp ./triejoin-ed/triejoin-ed ./bin

qchunk-indexchunk-ed:
	+make -C ./qchunk-indexchunk-ed
	+cp ./qchunk-indexchunk-ed/qchunk-indexchunk-ed ./bin

qchunk-indexgram-ed:
	+make -C ./qchunk-indexgram-ed
	+cp ./qchunk-indexgram-ed/qchunk-indexgram-ed ./bin

ppjoin-token:
	+make -C ./ppjoin-token
	+cp ./ppjoin-token/allpair-token ./bin
	+cp ./ppjoin-token/ppjoinplus-token ./bin
	+cp ./ppjoin-token/ppjoin-token ./bin

listmerger:
	+make -C ./flamingo/listmerger
	+cp ./flamingo/listmerger/listmerger-ed ./bin
	+cp ./flamingo/listmerger/listmerger-token ./bin

mtree-ed:
	+make -C ./mtree-ed/M3
	+cp ./mtree-ed/M3/mtree-ed ./bin

clean:
	rm -rf ./bin
	+make -C ./adaptjoin clean
	+make -C ./allpair-ed clean
	+make -C ./bitriejoin-ed clean
	+make -C ./edjoin-ed clean
	+make -C ./fastss-ed clean
	+make -C ./partenum-ed clean
	+make -C ./partenum-token clean
	+make -C ./passjoin-ed clean
	+make -C ./passjoin-token clean
	+make -C ./triejoin-ed clean
	+make -C ./qchunk-indexchunk-ed clean
	+make -C ./qchunk-indexgram-ed clean
	+make -C ./ppjoin-token clean
	+make -C ./flamingo/listmerger clean
	+make -C ./mtree-ed/M3 clean

.PHONY: all ed token clean adaptjoin allpair-ed bitriejoin-ed edjoin-ed fastss-ed partenum-ed partenum-token passjoin-ed passjoin-token triejoin-ed qchunk-indexchunk-ed qchunk-indexgram-ed ppjoin-token listmerger mtree-ed

