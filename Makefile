.phony: all
all:
	$(MAKE) -sC build all

.phony: %
%:
	$(MAKE) -sC build $*
