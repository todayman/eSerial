LIB_OBJECTS=eSerial/Parser.o eSerial/Writer.o eSerial/XMLParser.o eSerial/XMLWriter.o eSerial/b64.o
TEST_OBJECTS=test/Writer_tests.o test/XMLWriter_test.o test/b64_tests.o test/unittests.o test/XMLReader_test.o test/XML_tests_common.o
INSTALL_HEADERS=eSerial/Data.h eSerial/Parser.h eSerial/Writable.h eSerial/Writer.h eSerial/b64.h eSerial/hints.h

CC=clang
CXX=clang++
LD=clang++

CFLAGS=-Wall -Wextra -pedantic -std=c99
CXXFLAGS=-stdlib=libc++ -std=c++11 -I/usr/include/libxml2 -DGTEST_HAS_TR1_TUPLE=0

test: $(TEST_OBJECTS) Makefile
	$(LD) $(TEST_OBJECTS) -stdlib=libc++ -leSerial -lgtest -o test_serial
	./test_serial

eSerial: $(LIB_OBJECTS) Makefile
	$(LD) $(LIB_OBJECTS) -lxml2 -stdlib=libc++ -dynamiclib -o libeSerial.dylib
	mkdir -p include/eSerial
	cp $(INSTALL_HEADERS) include/eSerial/

clean:
	rm -f eSerial/*.o test/*.o

install: eSerial
	cp libeSerial.dylib /usr/local/lib/
	cp -R include/ /usr/local/include/