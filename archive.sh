mkdir rstack
cp Makefile-release rstack/
cp rstack.c rstack_container.c rstack_container.h \
    rstack_delete.c rstack_delete.h  rstack_read.c types.h \
    rstack/

tar -czvf 'rstack.tgz' rstack
