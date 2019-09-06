BIN_NAME := track-helper
VERSION_MAJOR=1
VERSION_MINOR=0

SRCDIR   = ../src
HEADERDIR= ../inc
OBJDIR   = ../obj
BINDIR   = ../bin
PKGDIR   = ../pkg

OUTPUT_FILENAME := $(BIN_NAME)

export OUTPUT_FILENAME
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) )

#GNU_PREFIX := arm-none-eabi-

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO :=
else
NO_ECHO := @
endif

# Toolchain commands
CC       := "$(GNU_PREFIX)gcc"
AS       := "$(GNU_PREFIX)as"
AR       := "$(GNU_PREFIX)ar" -r
LD       := "$(GNU_PREFIX)ld"
NM       := "$(GNU_PREFIX)nm"
OBJDUMP  := "$(GNU_PREFIX)objdump"
OBJCOPY  := "$(GNU_PREFIX)objcopy"
SIZE     := "$(GNU_PREFIX)size"

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#sources project
SOURCES := $(wildcard $(SRCDIR)/*.c)

#includes common to all targets

INC_PATHS += -I$(HEADERDIR)

LISTING_DIRECTORY = $(OBJDIR)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJDIR) $(BINDIR) $(LISTING_DIRECTORY) )


CFLAGS += --std=gnu99
CFLAGS += -Os -g -Wall -Werror

#Link Library
LIBS += -lncurses -lrt
#default target - first one defined
default:  $(BIN_NAME)

#building all targets
all:
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e $(PROJECT_NAME)

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	debug release


C_SOURCE_FILE_NAMES = $(notdir $(SOURCES))
C_PATHS = $(call remduplicates, $(dir $(SOURCES) ) )
C_OBJECTS = $(addprefix $(OBJDIR)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

vpath %.c $(C_PATHS)
#vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

CFLAGS += -DDEBUG
CFLAGS += -ggdb3 -O0
ASMFLAGS += -DDEBUG -ggdb3 -O0
LDFLAGS += -ggdb3 -O0

$(BIN_NAME): $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo LD $(OUTPUT_FILENAME)
	@$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(BINDIR)/$(OUTPUT_FILENAME)

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

# Create objects from C SRC files
$(OBJDIR)/%.o: %.c
	@echo CC $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Link
$(BINDIR)/$(OUTPUT_FILENAME): $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME)
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(BINDIR)/$(OUTPUT_FILENAME)

clean:
	$(RM) $(OBJDIR) $(BINDIR)/$(OUTPUT_FILENAME)
	
.PHONY: clean
