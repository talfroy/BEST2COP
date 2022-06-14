SUBDIRS := B2COP-2D B2COP-LCA2 B2COP-LCAB B2COP-SRG



vertclair=\033[1;32m
neutre=\033[0;m



all: $(SUBDIRS)
$(SUBDIRS):
	@echo "${vertclair} ------------- BUILDING FOLDER $@ ${neutre}"
	$(MAKE) -C $@

clean: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f Makefile clean; \
	done

.PHONY: all $(SUBDIRS) clean