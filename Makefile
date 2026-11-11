CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude
# To catch memory errors: make test SAN=1
ifdef SAN
CFLAGS += -fsanitize=address,undefined
endif

BUILD = build

.PHONY: all test test-01 test-02 clean

all: $(BUILD)/test_caesar $(BUILD)/test_vigenere

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/test_caesar: 01_caesar/test_caesar.c 01_caesar/caesar.c 01_caesar/caesar.h | $(BUILD)
	$(CC) $(CFLAGS) -I01_caesar -o $@ 01_caesar/test_caesar.c 01_caesar/caesar.c

$(BUILD)/test_vigenere: 02_vigenere/test_vigenere.c 02_vigenere/vigenere.c 02_vigenere/vigenere.h | $(BUILD)
	$(CC) $(CFLAGS) -I02_vigenere -o $@ 02_vigenere/test_vigenere.c 02_vigenere/vigenere.c

test-01: $(BUILD)/test_caesar
	./$(BUILD)/test_caesar

test-02: $(BUILD)/test_vigenere
	./$(BUILD)/test_vigenere

test: test-01 test-02

clean:
	rm -rf $(BUILD)
