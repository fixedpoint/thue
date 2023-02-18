.PHONY: all check clean

all: thue

check: thue
	install -d tmp
	./thue -h > /dev/null
	./thue --help > /dev/null
	./thue test/hello.t > tmp/hello
	diff -u test/hello.txt tmp/hello
	./thue -s 15000 test/seq.t > tmp/seq
	diff -u test/seq.txt tmp/seq
	./thue test/io.t < test/io.t > tmp/io
	diff -u test/io.txt tmp/io
	./thue -l test/io.t < test/io.t > tmp/io-l
	diff -u test/io.txt tmp/io-l
	./thue -r test/io.t < test/io.t > tmp/io-r
	diff -u test/io.txt tmp/io-r
	echo 0 | ./thue test/incr.t > tmp/incr
	echo 1 | ./thue test/incr.t >> tmp/incr
	echo 10 | ./thue test/incr.t >> tmp/incr
	echo 11 | ./thue test/incr.t >> tmp/incr
	diff -u test/incr.txt tmp/incr
	./thue -s 42 test/yes.t > tmp/yes
	diff -u test/yes.txt tmp/yes
	./thue test/quine.t > tmp/quine
	diff -u test/quine.t tmp/quine
	./thue test/quine2.t > tmp/quine2
	diff -u test/quine2.t tmp/quine2
	./thue test/quine3.t > tmp/quine3
	diff -u test/quine3.t tmp/quine3

clean:
	-rm -rf tmp
	-rm thue

thue: thue.cc
	$(CXX) -Wall -W -O3 -std=c++17 -o $@ $<
