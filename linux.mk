include Makefile

PREFIX=/usr
BIN_PREFIX=$(PREFIX)/bin
DOC_PREFIX=$(PREFIX)/share/doc/$(TARGET)
MAN_PREFIX=$(PREFIX)/share/man/man1
ICONS_PREFIX=$(PREFIX)/share/icons
APPS_PREFIX=$(PREFIX)/share/applications
DOCS=doc/ChangeLog doc/README doc/qnapi-download.desktop doc/qnapi-download.schemas

install: all
	strip $(TARGET)
	$(INSTALL_PROGRAM) $(TARGET) $(BIN_PREFIX)/$(TARGET)
	$(MKDIR) $(DOC_PREFIX)
	$(INSTALL_FILE) $(DOCS) $(DOC_PREFIX)/
	$(MOVE) $(DOC_PREFIX)/ChangeLog $(DOC_PREFIX)/changelog
	$(COMPRESS) $(DOC_PREFIX)/changelog
	$(INSTALL_FILE) doc/manpage $(MAN_PREFIX)/$(TARGET).1
	$(COMPRESS) $(MAN_PREFIX)/$(TARGET).1
	$(INSTALL_FILE) src/qnapi.png $(ICONS_PREFIX)/
	$(INSTALL_FILE) src/qnapi_44x44.png $(ICONS_PREFIX)/
	$(INSTALL_FILE) doc/$(TARGET).desktop $(APPS_PREFIX)/

uninstall:
	$(DEL_FILE) $(BIN_PREFIX)/$(TARGET)
	$(DEL_FILE) -r $(DOC_PREFIX)
	$(DEL_FILE) $(MAN_PREFIX)/$(TARGET).1.gz
	$(DEL_FILE) $(ICONS_PREFIX)/qnapi.png
	$(DEL_FILE) $(ICONS_PREFIX)/qnapi_44x44.png
	$(DEL_FILE) $(APPS_PREFIX)/$(TARGET).desktop
