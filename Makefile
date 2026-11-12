CC      = gcc
CFLAGS  = -Wall -Wextra -Wvla -std=c11 -g -Iinclude
# To catch memory errors: make test SAN=1
ifdef SAN
CFLAGS += -fsanitize=address,undefined
endif

BUILD = build

# Every folder named like 01_xxx, 02_xxx, ... is a step.
# Adding a new step folder is enough: no need to edit this file.
STEPS := $(sort $(wildcard [0-9][0-9]_*))
NUMS  := $(foreach s,$(STEPS),$(firstword $(subst _, ,$(s))))

.PHONY: all test clean $(addprefix test-,$(NUMS))

all: $(addprefix $(BUILD)/test_,$(STEPS))

$(BUILD):
	mkdir -p $(BUILD)

# Rules for one step: build every .c in the folder into build/test_<folder>
define STEP_RULES
$(BUILD)/test_$(1): $(wildcard $(1)/*.c) $(wildcard $(1)/*.h) $(wildcard include/*.h) | $(BUILD)
	$(CC) $(CFLAGS) -I$(1) -o $$@ $(wildcard $(1)/*.c)

test-$(firstword $(subst _, ,$(1))): $(BUILD)/test_$(1)
	./$(BUILD)/test_$(1)
endef
$(foreach s,$(STEPS),$(eval $(call STEP_RULES,$(s))))

test: $(addprefix test-,$(NUMS))

clean:
	rm -rf $(BUILD)
