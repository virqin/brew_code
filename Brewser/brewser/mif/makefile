ifndef RES_BITDEPTH
  export RES_BITDEPTH=1
endif

ifndef RES_LANGUAGE
  export RES_LANGUAGE=en
endif

.SUFFIXES:
.SUFFIXES: .brc .mif

all: brewser.mif

clean:
	rm -f brewser.mif

brewser.mif: $(RES_LANGUAGE)/brewser_mif.brc $(RES_BITDEPTH)bit/*
	../tools/barc -o $@ -j -i $(RES_BITDEPTH)bit -i $(RES_LANGUAGE) $<
