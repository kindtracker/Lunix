void _start(void) {
  const char Message[] = "Meow\n";

  register int R0 asm("r0") = 1;
  register const char *R1 asm("r1") = Message;
  register int R2 asm("r2") = sizeof(Message) - 1;
  register int R7 asm("r7") = 4;
  asm volatile("svc #0"
               :
               : "r"(R0), "r"(R1), "r"(R2), "r"(R7)
               : "memory", "cc");

  R0 = 0;
  R7 = 1;
  asm volatile("svc #0" : : "r"(R0), "r"(R7) : "memory", "cc");

  for (;;) {
  }
}
