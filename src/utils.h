#define COUNT_OF(x)                                                            \
  ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

#define debug_print(...)                                                       \
  do {                                                                         \
    if (DEBUG)                                                                 \
      os_printf(__VA_ARGS__);                                                  \
  } while (0)
