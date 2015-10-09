ifndef RES_BITDEPTH
  export RES_BITDEPTH=1
endif

ifndef RES_LANGUAGE
  export RES_LANGUAGE=en
endif

.SUFFIXES:
.SUFFIXES: .brc .bar

all: brewser.bar

clean:
	rm -f brewser.bar brewser_res.h

brewser.bar: $(RES_LANGUAGE)/brewser.brc $(RES_LANGUAGE)/*.html $(RES_BITDEPTH)bit/*
	../tools/barc -o $@ -h brewser_res.h -i $(RES_BITDEPTH)bit -i $(RES_LANGUAGE) $<
