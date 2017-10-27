C_FILE=conf_reader.c daemon.c log.c  pidfile.c main.c
H_FILE=conf_reader.h daemon.h log.h pidfile.h list.h common.h
main:$(C_FILE) $(H_FILE)
	gcc -o $@ $(C_FILE)

clean:
	rm -rf main
