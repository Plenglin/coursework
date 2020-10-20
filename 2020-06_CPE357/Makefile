labs = 1 2 3 4 5 6 7 8
progs = 1 2 3 4 5

$(foreach i, $(labs), $(eval $(call generate-target-zip,$(i),lab)))
$(foreach i, $(progs), $(eval $(call generate-target-zip,$(i),prog)))

