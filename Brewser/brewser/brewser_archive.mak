#
# BREW browser makefile for creating a zip file.

#----------------------------------------------------------------------------
# Make file prerequisites
#----------------------------------------------------------------------------

# ARM: Location of ARM portion of package (.bar and .mod)
# WIN: Location of WIN portion of package (.bar and .dll)
# BITDEPTH: Location of WIN portion of package (.bar and .dll)
# Note: A DOC directory is assumed to be local to this makefile 

#----------------------------------------------------------------------------
# Macro definition section
#----------------------------------------------------------------------------

# Application Specific Definitions
APPNAME  = brewser
APPSPECEXT = pdf

# Application Output Names
PKGDIR=$(APPNAME)_$(RES_BITDEPTH)_$(RES_LANGUAGE)
APPDIR=$(PKGDIR)/$(APPNAME)
PKGDIRWIN=$(APPDIR)
PKGDIRARM=$(APPDIR)/ARM
PKGDIRDOC=$(APPDIR)/DOC
ifdef VERSION
 COPYDIR=$(VERSION)
 PKGNAME=$(APPNAME)_$(RES_BITDEPTH)_$(RES_LANGUAGE)_$(VERSION)
else
 PKGNAME=$(APPNAME)_$(RES_BITDEPTH)_$(RES_LANGUAGE)
endif

#----------------------------------------------------------------------------
# Rules section
#----------------------------------------------------------------------------

# Simplified Pseudo Targets
.PHONY: all ARMFILES WINFILES DOCFILES

ifdef VERSION
all: $(COPYDIR) $(COPYDIR)/$(PKGNAME).zip
else
all: $(PKGDIR)/$(PKGNAME).zip
endif

clean:
	@if test -d $(APPDIR); then rm -rf $(APPDIR); fi
	@if test -d $(PKGDIR); then rm -rf $(PKGDIR); fi

allclean:
	@if test -d $(COPYDIR); then rm -rf $(COPYDIR); fi

# Output directories
$(COPYDIR):
	@if test ! -d $(COPYDIR); then mkdir $(COPYDIR); fi

$(PKGDIR):
	@if test ! -d $(PKGDIR); then mkdir $(PKGDIR); fi

$(APPDIR): $(PKGDIR)
	@if test ! -d $(APPDIR); then mkdir $(APPDIR); fi

# Primary Target (.zip)
$(COPYDIR)/$(PKGNAME).zip: $(PKGDIR)/$(PKGNAME).zip
	cp $< $@

$(PKGDIR)/$(PKGNAME).zip: ARMFILES WINFILES DOCFILES
	# ZIP it up, ARM, WIN, and DOC are static names for all apps
	cd $(PKGDIR);zip -r $(PKGNAME).zip $(APPNAME).mif $(APPNAME)

# ARM Files (Device)
ARMFILES: $(PKGDIRARM) $(PKGDIRARM)/$(APPNAME).mod $(PKGDIRARM)/$(APPNAME).bar $(PKGDIRARM)/$(APPNAME).mif

$(PKGDIRARM): $(APPDIR)
	@if test ! -d $(PKGDIRARM); then mkdir $(PKGDIRARM); fi

$(PKGDIRARM)/$(APPNAME).mod: ./$(APPNAME).mod
	cp $< $@

$(PKGDIRARM)/$(APPNAME).bar: ./$(APPNAME).bar
	cp $< $@

$(PKGDIRARM)/$(APPNAME).mif: ../$(APPNAME).mif
	cp $< $@

# Windows Files (Emulator)
WINFILES: $(APPDIR) $(PKGDIRWIN)/$(APPNAME).dll $(PKGDIRWIN)/$(APPNAME).bar $(PKGDIR)/$(APPNAME).mif

$(PKGDIRWIN)/$(APPNAME).dll: ./$(APPNAME).dll
	cp $< $@

$(PKGDIRWIN)/$(APPNAME).bar: ./$(APPNAME).bar
	cp $< $@

$(PKGDIR)/$(APPNAME).mif: ../$(APPNAME).mif
	cp $< $@

# Doc Files
DOCFILES: $(PKGDIRDOC) $(PKGDIRDOC)/$(APPNAME)_spec.$(APPSPECEXT)

$(PKGDIRDOC): $(APPDIR)
	@if test ! -d $(PKGDIRDOC); then mkdir $(PKGDIRDOC); fi

$(PKGDIRDOC)/$(APPNAME)_spec.$(APPSPECEXT): DOC/$(APPNAME)_spec.$(APPSPECEXT)
	cp $< $@

