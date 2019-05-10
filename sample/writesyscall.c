extern void write_char(const char* c);

void write_string(const char* s) {
    do {
        char c = *s;
        if (c == 0) return;
        write_char(s);
        s++;
    } while(1);
}

void _start() {
    write_string("super secret message. I love Travis Scott!\n");
}
