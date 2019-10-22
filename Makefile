all: projects makebin 

projects:
	@$(MAKE) -C cjrShow
	@$(MAKE) -C cjrShowFull
#:w	@$(MAKE) -C cjrShowNMRPipe
	@$(MAKE) -C cjrShowNMRView
	@$(MAKE) -C cjrShowProcpar
	@$(MAKE) -C cjrShowSparky
	@$(MAKE) -C cjrShowVarian
	@$(MAKE) -C xml_parser

makebin:
	mkdir -p bin
	cp */dist/Debug/GNU-Linux/* bin

clean:
	rm -fr */dist/ */build bin/
