SUBDIRS=Cypher App

# default target.  Execute 'make' in each subdirectory
all:
	@for dir in $(SUBDIRS); do \
		make -C $$dir ; \
	done

# clean
clean:
	rm -f *~
	@for dir in $(SUBDIRS); do \
		make -C $$dir clean ; \
	done

