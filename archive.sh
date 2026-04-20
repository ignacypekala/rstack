mkdir rstack
cp Makefile-release rstack/Makefile
cp rstack.c rstack.h rstack_container.c rstack_container.h \
    rstack_delete.c rstack_delete.h  rstack_read.c types.h \
    rstack/

cd rstack
tar -czvf 'rstack.tgz' *
mv rstack.tgz ..
cd -
