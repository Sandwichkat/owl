#include "compat.h"

#if defined(_WIN32)

#include <stdio.h>

const char *ether_ntoa(const struct ether_addr *addr) {
  static char buf[18];
  if (!addr)
    return "00:00:00:00:00:00";
  snprintf(buf, sizeof(buf), "%x:%x:%x:%x:%x:%x",
           addr->ether_addr_octet[0], addr->ether_addr_octet[1], addr->ether_addr_octet[2],
           addr->ether_addr_octet[3], addr->ether_addr_octet[4], addr->ether_addr_octet[5]);
  return buf;
}

#endif
