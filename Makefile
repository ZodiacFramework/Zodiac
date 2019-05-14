DEST=/usr/local/bin/Zodiac

.PHONY: release
release:
	cd Hydra; make
	cd Hydra/alpha; make

.PHONY: debug
	cd Hydra; make debug

.PHONY: install
install:
	mkdir $(DEST)
	cp -a LICENSE $(DEST)
	cp -a -r Hydra $(DEST)

.PHONY: uninstall
uninstall:
	rm -rf /usr/local/bin/Zodiac
