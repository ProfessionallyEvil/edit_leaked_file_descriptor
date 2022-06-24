CC = gcc
CFLAGS = -O3 -s
SRCFILES=src/edit_leaked_file_descriptor.c src/proc.c src/prompts.c
OUTFILE=edit_leaked_file_descriptor

edit_leaked_file_descriptor:
	$(CC) $(CFLAGS) $(SRCFILES) -o $(OUTFILE)

clean:
	rm -f edit_leaked_file_descriptor

help:
	@echo "Default Target:"
	@echo "\t edit_leaked_file_descriptor => Builds the edit_leaked_file_descriptor application"
	@echo "\nMaintance Targets:"
	@echo "\t clean => delete the binaries."
	@echo "\t help => Show this help screen."
