all:
	@$(MAKE) -C cjrShow
	@$(MAKE) -C cjrShowFull
	@$(MAKE) -C cjrShowNMRPipe
	@$(MAKE) -C cjrShowNMRView
	@$(MAKE) -C cjrShowProcpar
	@$(MAKE) -C cjrShowSparky
	@$(MAKE) -C cjrShowVarian
	@$(MAKE) -C xml_parser
