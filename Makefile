OUT_DIR			= _out/bin
INSTALL_DIR		= _install/bin

EXESUFFIX		= .exe
DEBUG			= devenv -debugexe $(1) $(2)
#EXESUFFIX		=
#DEBUG			= gdb --args $(1) $(2)


all:
	fbuild all

run: all
	@cd $(OUT_DIR) && ./game$(EXESUFFIX)

run-tests: all
	@cd $(OUT_DIR) && ./tests$(EXESUFFIX) --startup-only

run-game: all
	@cd $(OUT_DIR) && ./game$(EXESUFFIX)

debug-tests: all
	@cd $(OUT_DIR) && $(call DEBUG,tests$(EXESUFFIX),--startup-only) &

debug-game: all
	@cd $(OUT_DIR) && $(call DEBUG,game$(EXESUFFIX),) &


.PHONY: proper
proper:
	@rm -rf _build _out _install _vs

.PHONY: docs
docs:
	@doxygen .doxyconf

.PHONY: cloc
cloc:
	cloc --exclude-dir=ext,GL src include

