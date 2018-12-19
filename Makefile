include nemu/Makefile.git

default:
	@echo "Please run 'make' under any subprojects to compile."
 
clean:
	-$(MAKE) -C nemu clean
	-$(MAKE) -C nexus-am clean
	-$(MAKE) -C nanos-lite clean
	-$(MAKE) -C navy-apps clean

submit: clean
	git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash submit.sh

setup:
	STUID=$(STUID) STUNAME=$(STUNAME) bash setup.sh

count:
	find ./nemu -name "*.[ch]" | xargs cat | grep -v "^\s$$" | wc -l

.PHONY: default clean submit

