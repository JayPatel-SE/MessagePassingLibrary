SOURCES += $(wildcard src/*.c) \
		   $(wildcard src/internal/*.c) \

INCLUDES += -Iinc

BUILD_DIR = build

LDFLAGS = -pthread -lm -l cmocka

WARNINGS += -Wall

EXE = msg_main

OBJECTS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(SOURCES))))

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc $(WARNINGS) -c $(INCLUDES) $< -o $@

$(EXE): $(OBJECTS)
	gcc $(OBJECTS) -o $@ $(LDFLAGS)

all: $(EXE)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(EXE)