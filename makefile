CXX=g++

SDIR=./src
ODIR=./obj
BDIR=./obj/build
IDIR=./src/include

TARGET=$(BDIR)/RISC-Emu

vpath %.c $(SDIR)
vpath %.o $(ODIR)
vpath %.h $(IDIR)

DIRS=hw
SOURCEDIRS=$(SDIR) $(foreach dir, $(DIRS), $(addprefix $(SDIR)/, $(dir)))
TARGETDIRS=$(ODIR) $(foreach dir, $(DIRS), $(addprefix $(ODIR)/, $(dir)))
DEPSDIRS=$(IDIR) $(foreach dir, $(DIRS), $(addprefix $(IDIR)/, $(dir)))

SOURCES=$(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.cpp))
OBJS=$(subst $(SDIR), $(ODIR), $(SOURCES:.cpp=.o))
DEPS=$(foreach dir, $(DEPSDIRS), $(wildcard $(dir)/*.h))

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

clean:
	find $(ODIR) -type f -name '*.o' -delete
	rm $(TARGET)
	@echo Done cleaning

debug:
	@echo $(SOURCEDIRS)
	@echo $(TARGETDIRS)
	@echo $(SOURCES)
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(BUILD)