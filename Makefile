.PHONY: all check clean

all: thue

check: \
	tmp/no-such-file \
	tmp/help.log \
	tmp/hello.log \
	tmp/seq.log \
	tmp/io.log \
	tmp/io-l.log \
	tmp/io-r.log \
	tmp/incr.log \
	tmp/yes.log \
	tmp/quine.log \
	tmp/quine2.log \
	tmp/quine3.log \
	tmp/bin1.log \
	tmp/bin2.log

clean:
	-rm -rf tmp
	-rm thue

thue: thue.cc
	$(CXX) -Wall -W -O3 -std=c++17 -o $@ $<

tmp:
	install -d $@

tmp/h: thue | tmp
	./thue -h > $@

tmp/no-such-file: thue | tmp
	! ./thue no-such-file 2> $@

tmp/help: thue | tmp
	./thue --help > $@

tmp/help.log: tmp/h tmp/help
	diff -u $^ > $@

tmp/hello: test/hello.t thue | tmp
	./thue $< > $@

tmp/hello.log: test/hello.txt tmp/hello
	diff -u $^ > $@

tmp/seq: test/seq.t thue | tmp
	./thue -s 15000 test/seq.t > $@

tmp/seq.log: test/seq.txt tmp/seq
	diff -u $^ > $@

tmp/io: test/io.t thue | tmp
	./thue $< < $< > $@

tmp/io.log: test/io.txt tmp/io
	diff -u $^ > $@

tmp/io-l: test/io.t thue | tmp
	./thue -l $< < $< > $@

tmp/io-l.log: test/io.txt tmp/io-l
	diff -u $^ > $@

tmp/io-r: test/io.t thue | tmp
	./thue -r $< < $< > $@

tmp/io-r.log: test/io.txt tmp/io-r
	diff -u $^ > $@

tmp/incr: test/incr.t | tmp
	echo 0 | ./thue $< > tmp/incr
	echo 1 | ./thue $< >> tmp/incr
	echo 10 | ./thue $< >> tmp/incr
	echo 11 | ./thue $< >> tmp/incr

tmp/incr.log: test/incr.txt tmp/incr
	diff -u $^ > $@

tmp/yes: test/yes.t | tmp
	./thue -s 42 $< > $@

tmp/yes.log: test/yes.txt tmp/yes
	diff -u $^ > $@

tmp/quine: test/quine.t | tmp
	./thue $< > $@

tmp/quine.log: test/quine.t tmp/quine
	diff -u $^ > $@

tmp/quine2: test/quine2.t | tmp
	./thue $< > $@

tmp/quine2.log: test/quine2.t tmp/quine2
	diff -u $^ > $@

tmp/quine3: test/quine3.t | tmp
	./thue $< > $@

tmp/quine3.log: test/quine3.t tmp/quine3
	diff -u $^ > $@

tmp/bin1a: test/bin.t thue | tmp
	./thue -S 1 $< > $@

tmp/bin1b: test/bin.t thue | tmp
	./thue -S 1 $< > $@

tmp/bin2: test/bin.t thue | tmp
	./thue -S 2 $< > $@

tmp/bin1.log: tmp/bin1a tmp/bin1b
	diff -u $^ > $@

tmp/bin2.log: tmp/bin1a tmp/bin2
	! diff -u $^ > $@
