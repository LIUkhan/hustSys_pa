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

<<<<<<< HEAD
recover:
	STUID=$(STUID) STUNAME=$(STUNAME) bash recover.sh

=======
count:
	find ./nemu -name "*.[ch]" | xargs cat | grep -v "^\s$$" | wc -l
>>>>>>> pa1

.PHONY: default clean submit

