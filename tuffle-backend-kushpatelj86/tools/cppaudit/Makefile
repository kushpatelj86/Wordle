include ../settings/config.mk

OS_NAME 		:= $(shell uname -s | tr A-Z a-z)
SHELL         		:= bash
OUTPUT_PATH		:= ../output
SETTINGS_PATH		:= ../settings
ROOT_PATH		:= $(shell printf "%q" "$(PWD)")
CPP_AUDIT_PATH		:= $(shell printf "%q" "$$(pwd)")
CPPAUDIT_FROM_ROOT 	:= $(shell realpath --relative-to=$(ROOT_PATH) $(CPP_AUDIT_PATH))
REL_ROOT_PATH		:= $(shell realpath --relative-to=$(CPP_AUDIT_PATH) $(ROOT_PATH))
OUTPUT_FROM_ROOT 	:= $(shell realpath --relative-to=$(ROOT_PATH) $(OUTPUT_PATH))
FILES         		:= $(DRIVER) $(IMPLEMS) $(HEADERS)
HAS_CLANGTDY  		:= $(shell command -v clang-tidy 2> /dev/null)
HAS_CLANGFMT  		:= $(shell command -v clang-format 2> /dev/null)
HAS_GTEST         	:= $(shell echo -e "int main() { }" >> test.cc ; clang++ test.cc -o test -lgtest 2> /dev/null; echo $$?; rm -rf test.cc test;)

ifeq ($(OS_NAME), darwin)
	COMPILE_FLAGS	:= $(MAC_COMPILE_FLAGS)
	UT_COMPILE_FLAGS	:= $(MAC_UT_COMPILE_FLAGS)
	HAS_BREW	:= $(shell command -v brew 2> /dev/null)
endif

ifeq ($(UTNAME),)
  UTNAME = unittest.cpp
endif

.PHONY: build test stylecheck formatcheck all clean noskiptest install_gtest test_boardcolors test_dictionary test_errormessage test_gamestatus test_guessedwords

$(OUTPUT_PATH):
	@mkdir -p $(OUTPUT_PATH)

$(OUTPUT_PATH)/unittest: $(OUTPUT_PATH) $(SETTINGS_PATH)/$(UTNAME) $(addprefix $(REL_ROOT_PATH)/, $(DRIVER) $(IMPLEMS) $(HEADERS))
	@clang++ -std=c++17 -fsanitize=address $(addprefix $(REL_ROOT_PATH)/, $(IMPLEMS) $(OTHER_IMPLEMS)) $(SETTINGS_PATH)/$(UTNAME) -o $(OUTPUT_PATH)/unittest -pthread -lgtest $(UT_COMPILE_FLAGS)

$(OUTPUT_PATH)/unittest_boardcolors: $(OUTPUT_PATH) $(SETTINGS_PATH)/$(UTNAME_BOARDCOLORS) $(addprefix $(REL_ROOT_PATH)/, $(DRIVER) $(IMPLEMS) $(HEADERS))
	@clang++ -std=c++17 -fsanitize=address $(addprefix $(REL_ROOT_PATH)/, $(IMPLEMS) $(OTHER_IMPLEMS)) $(SETTINGS_PATH)/$(UTNAME_BOARDCOLORS) -o $(OUTPUT_PATH)/unittest_boardcolors -pthread -lgtest $(UT_COMPILE_FLAGS)

$(OUTPUT_PATH)/unittest_dictionary: $(OUTPUT_PATH) $(SETTINGS_PATH)/$(UTNAME_DICTIONARY) $(addprefix $(REL_ROOT_PATH)/, $(DRIVER) $(IMPLEMS) $(HEADERS))
	@clang++ -std=c++17 -fsanitize=address $(addprefix $(REL_ROOT_PATH)/, $(IMPLEMS) $(OTHER_IMPLEMS)) $(SETTINGS_PATH)/$(UTNAME_DICTIONARY) -o $(OUTPUT_PATH)/unittest_dictionary -pthread -lgtest $(UT_COMPILE_FLAGS)

$(OUTPUT_PATH)/unittest_errormessage: $(OUTPUT_PATH) $(SETTINGS_PATH)/$(UTNAME_ERRORMESSAGE) $(addprefix $(REL_ROOT_PATH)/, $(DRIVER) $(IMPLEMS) $(HEADERS))
	@clang++ -std=c++17 -fsanitize=address $(addprefix $(REL_ROOT_PATH)/, $(IMPLEMS) $(OTHER_IMPLEMS)) $(SETTINGS_PATH)/$(UTNAME_ERRORMESSAGE) -o $(OUTPUT_PATH)/unittest_errormessage -pthread -lgtest $(UT_COMPILE_FLAGS)

$(OUTPUT_PATH)/unittest_gamestatus: $(OUTPUT_PATH) $(SETTINGS_PATH)/$(UTNAME_GAMESTATUS) $(addprefix $(REL_ROOT_PATH)/, $(DRIVER) $(IMPLEMS) $(HEADERS))
	@clang++ -std=c++17 -fsanitize=address $(addprefix $(REL_ROOT_PATH)/, $(IMPLEMS) $(OTHER_IMPLEMS)) $(SETTINGS_PATH)/$(UTNAME_GAMESTATUS) -o $(OUTPUT_PATH)/unittest_gamestatus -pthread -lgtest $(UT_COMPILE_FLAGS)

$(OUTPUT_PATH)/unittest_guessedwords: $(OUTPUT_PATH) $(SETTINGS_PATH)/$(UTNAME_GUESSEDWORDS) $(addprefix $(REL_ROOT_PATH)/, $(DRIVER) $(IMPLEMS) $(HEADERS))
	@clang++ -std=c++17 -fsanitize=address $(addprefix $(REL_ROOT_PATH)/, $(IMPLEMS) $(OTHER_IMPLEMS)) $(SETTINGS_PATH)/$(UTNAME_GUESSEDWORDS) -o $(OUTPUT_PATH)/unittest_guessedwords -pthread -lgtest $(UT_COMPILE_FLAGS)

install_gtest:
ifeq ($(HAS_GTEST),1)
	@echo -e "google test not installed\n"
ifeq ($(OS_NAME), darwin)
ifndef ($(HAS_BREW))
	@echo -e "Installing brew, please provide your password when asked.\n"
	@/bin/bash -c "$$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
	@brew install coreutils
	# Need to redefine the path directories otherwise this will fail the first time.
endif
	@echo -e "Installing cmake. Please provide the password when asked\n"
	@brew install cmake
	@echo -e "\nDownloading and installing googletest\n"
	@cd /tmp/; git clone https://github.com/google/googletest; cd googletest; mkdir build; cd build; cmake .. -DCMAKE_CXX_STANDARD=17; make; make install
	@echo -e "Finished installing google test\n"
else
ifneq ($(shell lsb_release -sr), 20.04)
	@cd /tmp/; git clone https://github.com/google/googletest.git; cd googletest; cmake CMakeLists.txt; make; sudo cp -r googletest/include/. /usr/include; sudo cp -r googlemock/include/. /usr/include; sudo cp lib/*.a /usr/lib
else
	@echo -e "Installing cmake. Please provide the password when asked\n"
	@sudo apt-get install cmake # install cmake
	@echo -e "\nDownloading and installing googletest\n"
	@sudo apt-get install libgtest-dev libgmock-dev
	@echo -e "Finished installing google test\n"
endif
endif
endif

build:
	@echo "Building the Tuffle Backend. This may take a minute..."
	@echo "If compilation on Replit fails, try refreshing the page."
	@cd $(ROOT_PATH)/ && g++ -g -Wno-cpp -O0 -o main dictionary.cc gamestate.cc main.cc tuffle.cc -lpthread -std=c++17
	@echo "Successfully compiled the Tuffle Backend!"

test: install_gtest $(OUTPUT_PATH)/unittest
	@echo -e "\n========================\nRunning unit test\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest.xml"
	@echo -e "\n========================\nUnit test complete\n========================\n"

test_boardcolors: install_gtest $(OUTPUT_PATH)/unittest_boardcolors
	@echo -e "\n========================\nRunning boardColors unit tests\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest_boardcolors --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest_boardcolors.xml"
	@echo -e "\n========================\nUnit tests complete\n========================\n"

test_dictionary: install_gtest $(OUTPUT_PATH)/unittest_dictionary
	@echo -e "\n========================\nRunning Dictionary unit tests\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest_dictionary --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest_dictionary.xml"
	@echo -e "\n========================\nUnit tests complete\n========================\n"

test_errormessage: install_gtest $(OUTPUT_PATH)/unittest_errormessage
	@echo -e "\n========================\nRunning errorMessage unit tests\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest_errormessage --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest_errormessage.xml"
	@echo -e "\n========================\nUnit tests complete\n========================\n"

test_gamestatus: install_gtest $(OUTPUT_PATH)/unittest_gamestatus
	@echo -e "\n========================\nRunning gameStatus unit tests\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest_gamestatus --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest_gamestatus.xml"
	@echo -e "\n========================\nUnit tests complete\n========================\n"

test_guessedwords: install_gtest $(OUTPUT_PATH)/unittest_guessedwords
	@echo -e "\n========================\nRunning guessedWords unit tests\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest_guessedwords --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest_guessedwords.xml"
	@echo -e "\n========================\nUnit tests complete\n========================\n"

noskiptest: install_gtest $(OUTPUT_PATH)/unittest
	@echo -e "\n========================\nRunning unit test\n========================\n"
	@cd $(REL_ROOT_PATH)/ && ./$(OUTPUT_FROM_ROOT)/unittest --noskip --gtest_output="xml:$(OUTPUT_FROM_ROOT)/unittest.xml"
	@echo -e "\n========================\nUnit test complete\n========================\n"

$(OUTPUT_PATH)/compile_commands.json : $(SETTINGS_PATH)/config.mk
	@cd $(ROOT_PATH)/ && bash $(CPP_AUDIT_PATH)/gen_ccjs.sh $(OUTPUT_FROM_ROOT) $(EXEC_FILE) $(DRIVER) $(IMPLEMS)

stylecheck: $(OUTPUT_PATH)/compile_commands.json
	@make clean
ifndef HAS_CLANGTDY
	@echo -e "clang-tidy not installed\n"
	@echo -e "Installing clang-tidy. Please provide password when asked\n"
ifeq ($(OS_NAME),darwin)
	@echo -e "WARNING: Installing llvm to provide stylecheck functionality will take about an hour."
	@brew install llvm
	@ln -s "$(brew --prefix llvm)/bin/clang-tidy" "/usr/local/bin/clang-tidy"
else
	@sudo apt-get -y install clang-tidy
	@echo -e "Finished installing clang-tidy\n"
endif
endif
	@echo -e "========================\nRunning style checker\n========================\n"
	@cd $(REL_ROOT_PATH)/ && clang-tidy -p=$(OUTPUT_FROM_ROOT) -quiet -header-filter=^crow_all.h  -export-fixes=$(OUTPUT_FROM_ROOT)/style.yaml $(IMPLEMS) $(DRIVER)
	@if [ -f "$(OUTPUT_PATH)/style.yaml" ]; then \
    echo -e "========================\nStyle checker failed\n========================\n"; \
    exit 1; \
  else \
    echo -e "========================\nStyle checker complete\n========================\n"; \
  fi

formatcheck:
ifndef HAS_CLANGFMT
ifeq ($(OS_NAME),darwin)
	@echo -e "clang-format not installed.\n"
	@echo -e "Installing clang-format. Please provide the password when asked\n"
	@brew install clang-format
	@echo -e "Finished installing clang-format\n"
else
	@echo -e "clang-format not installed.\n"
	@echo -e "Installing clang-format. Please provide the password when asked\n"
	@sudo apt-get -y install clang-format
	@echo -e "Finished installing clang-format\n"
endif
endif
	@echo -e "========================\nRunning format checker\n========================"
	@cd $(REL_ROOT_PATH)/ && bash $(CPPAUDIT_FROM_ROOT)/diff_format.sh $(CLANG_FORMAT_FLAGS) $(FILES)
	@cd $(REL_ROOT_PATH)/ && clang-format $(FILES) -output-replacements-xml > $(OUTPUT_FROM_ROOT)/format.xml
	@echo -e "========================\nFormat checking complete\n========================\n"

all:	test stylecheck formatcheck

clean:
	@rm -f $(OUTPUT_PATH)/*
