mkdir rstack
cp rstack.c Makefile rstack_container.c rstack_container.h \
    rstack_delete.c rstack_delete.h  rstack_read.c types.h \
    rstack/

tar -czvf 'rstack.tgz' rstack
