CXX=g++

SDIR=./src
ODIR=./obj
BDIR=./obj/build
IDIR=./src/include
TDIR=./tests/emu-tests

TARGET=$(BDIR)/RISC-Emu
TEST_TARGET=$(TDIR)/RISC-Emu.test

TEST=./tests
TESTROOT=$(TEST)/framework/cxxtest-4.4
TESTBIN=$(TESTROOT)/bin/cxxtestgen
TESTBUILDFLAGS=-I$(TESTROOT) -I$(IDIR) -g

vpath %.c $(SDIR)
vpath %.o $(ODIR)
vpath %.h $(IDIR)

DIRS=hw instructions units
SOURCEDIRS=$(SDIR) $(foreach dir, $(DIRS), $(addprefix $(SDIR)/, $(dir)))
TARGETDIRS=$(ODIR) $(foreach dir, $(DIRS), $(addprefix $(ODIR)/, $(dir)))
DEPSDIRS=$(IDIR) $(foreach dir, $(DIRS), $(addprefix $(IDIR)/, $(dir)))
TESTDIRS=$(TDIR) $(foreach dir, $(DIRS), $(addprefix $(TDIR)/, $(dir)))
TEST_RESULT_OUT=$(TEST)/results

SOURCES=$(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.cpp))
SOURCES_WO_MAIN=$(filter-out $(SDIR)/main.cpp, $(SOURCES))
OBJS=$(subst $(SDIR), $(ODIR), $(SOURCES:.cpp=.o))
DEPS=$(foreach dir, $(DEPSDIRS), $(wildcard $(dir)/*.h))
TESTSOURCES=$(foreach dir, $(TESTDIRS), $(wildcard $(dir)/*.h))
TESTOBJ=$(TESTSOURCES:.h=.test)
TESTPARTS=$(TESTSOURCES:.h=.part.cpp)
TEST_XML_RESULTS=$(TEST_RESULT_OUT)/$(shell date +%s).result.xml
MAKEDEPSFILE=makedeps

DEBUG=yes

ifeq ($(DEBUG),yes)
	BUILD=-g
else
	BUILD=
endif

CFLAGS=-I$(IDIR) $(BUILD)

.PHONY: all clean directories testall

all: directories $(TARGET)

directories:
	mkdir -p $(BDIR)
	mkdir -p $(TARGETDIRS)

$(TARGET): $(OBJS)
	@echo Linking $@
	$(CXX) -o $(TARGET) $(OBJS) $(BUILD) -pthread

define generateRules
$(1)/%.o: $(subst $(ODIR), $(SDIR), $(1))/%.cpp
	@echo Building $$@
	$(CXX) -c -o $$@ $$< $(CFLAGS)
endef

$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))

define compiletest
$(1)/%_Tests: $(1)/%_Tests.h $(subst $(TDIR), $(ODIR), $(1))/%.o
	@echo Making test $$@
	$(TESTBIN) --error-printer -o $$@.cpp $$<
	$(CXX) -c -o $$@.o $$@.cpp $(TESTBUILDFLAGS)
	$(eval NAME := $$@_Tests)
	@echo $(NAME)
	$(eval DEPS := $$(shell cat $(1)/$(MAKEDEPSFILE) | grep "$*_Tests" | cut -d':' -f2))
	@echo $(DEPS)
	$(CXX) -o $$@.test $$@.cpp $(subst $(TDIR), $(ODIR), $(1))/$$*.o $(TESTBUILDFLAGS)
	$$@.test
endef

$(foreach targetdir, $(TESTDIRS), $(eval $(call compiletest, $(targetdir))))

testall: testclean $(TEST_TARGET)

$(TEST_TARGET): $(SOURCES_WO_MAIN) $(TESTPARTS)
	@echo Bulding runner
	$(TESTBIN) --root --xunit-printer --xunit-file=$(TEST_XML_RESULTS) -o $(TDIR)/runner.cpp
	$(CXX) -o $@ -I$(TESTROOT) $(TDIR)/runner.cpp $(SOURCES_WO_MAIN) $(TESTPARTS)
	$@

define compiletestparts
$(1)/%.part.cpp: $(1)/%.h
	@echo Making part $$@
	$(TESTBIN) --part --xunit-printer -o $$@ $$<
endef

$(foreach targetdir, $(TESTDIRS), $(eval $(call compiletestparts, $(targetdir))))

clean: testclean
	find $(ODIR) -type f -name '*.o' -delete
	rm $(TARGET)
	@echo Done cleaning

testclean:
	find $(TDIR) -type f -name '*.cpp' -delete
	find $(TDIR) -type f -name '*.test' -delete
	find $(TDIR) -type f -name '*.o' -delete
	find $(TDIR) -type f -name '*.part.cpp' -delete

debug:
	@echo $(SOURCEDIRS)
	@echo $(TARGETDIRS)
	@echo $(TESTDIRS)
	@echo $(SOURCES)
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(BUILD)
	@echo $(TESTSOURCES)
	@echo $(TESTOBJ)