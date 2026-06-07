#include "../rstack.h"
#include "../macros.h"

int main(void) {
  rstack_t *rs0 = rstack_new();
  ASSERT(rs0);

  ASSERT(rstack_empty(rs0) == true);
  ASSERT_RESULT(rstack_front(rs0), false, 0);
  NO_ERROR(rstack_write(OUTPUT_FILE, rs0));
  rstack_delete(rs0);

  return PASS;
}
