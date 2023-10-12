TARGETS = build test stylecheck formatcheck all noskiptest grade clean test_boardcolors test_dictionary test_errormessage test_gamestatus test_guessedwords

.PHONY: $(TARGETS)

tools/output:
	@mkdir -p tools/output

$(TARGETS): tools/output
	@$(MAKE) -C tools/cppaudit $(MAKECMDGOALS)
