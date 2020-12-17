CXX=g++

SDIR=./src
ODIR=./obj
BDIR=./obj/build
IDIR=./src/include
TDIR=./tests/emu-tests

TARGET=$(BDIR)/RISC-Emu

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

SOURCES=$(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.cpp))
OBJS=$(subst $(SDIR), $(ODIR), $(SOURCES:.cpp=.o))
DEPS=$(foreach dir, $(DEPSDIRS), $(wildcard $(dir)/*.h))
TESTSOURCES=$(foreach dir, $(TESTDIRS), $(wildcard $(dir)/*.h))
TESTOBJ=$(TESTSOURCES:.h=.test)

DEBUG=yes

ifeq ($(DEBUG),yes)
	BUILD=-g
else
	BUILD=
endif

CFLAGS=-I$(IDIR) $(BUILD)

.PHONY: all clean directories

all: directories $(TARGET)

directories:
	mkdir -p $(BDIR)
	mkdir -p $(TARGETDIRS)

$(TARGET): $(OBJS)
	@echo Linking $@
	$(CXX) -o $(TARGET) $(OBJS) $(BUILD)

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
	$(CXX) -o $$@.test $$@.cpp $(subst $(TDIR), $(ODIR), $(1))/$$*.o $(TESTBUILDFLAGS)
	$$@.test
endef

$(foreach targetdir, $(TESTDIRS), $(eval $(call compiletest, $(targetdir))))

# tests/emu-tests/instructions/AbstractInstruction_Tests: tests/emu-tests/instructions/AbstractInstruction_Tests.h
# 	@echo Making test $@
# 	$(TESTBIN) --error-printer -o $@.cpp $<
# 	$(CXX) -o $@.test $@.cpp $(TESTBUILDFLAGS)
# 	$@.test

clean: testclean
	find $(ODIR) -type f -name '*.o' -delete
	rm $(TARGET)
	@echo Done cleaning

testclean:
	find $(TDIR) -type f -name '*.cpp' -delete
	find $(TDIR) -type f -name '*.test' -delete
	find $(TDIR) -type f -name '*.o' -delete

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