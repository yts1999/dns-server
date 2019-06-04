default:
	$(MAKE) full

full: main.cc buffer.cc message.cc parser.cc record.cc sockope.cc table.cc utils.cc
	g++ main.cc buffer.cc message.cc parser.cc record.cc sockope.cc table.cc utils.cc -o dnsrelay --std=c++14 

run:
	$(MAKE) full
	./dnsrelay

clean:
	$(RM) *.o dnsrelay

count:
	@echo "Total code lines: `ls include/*.h *.cc | xargs cat | wc -l`"
