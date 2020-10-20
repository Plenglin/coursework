GITZIP := ./gitzip.sh

define generate-target-zip
$(DIR)/astrid_yu_$1.zip: 
	$(GITZIP) $(DIR)/astrid_yu_$1.zip $(DIR)/$1 
all:: $(DIR)/astrid_yu_$1.zip
CLEANFILES += $(DIR)/astrid_yu_$1.zip
endef

define generate-class-directory
DIR := $1
include $1/Makefile
endef

.PHONY: all clean
CLASS_DIRS = 2020-06_CPE357 2020-09_CSC572
$(foreach class,$(CLASS_DIRS),$(eval $(call generate-class-directory,$(class))))

clean:
	rm -f $(CLEANFILES)

