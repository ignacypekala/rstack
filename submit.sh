mkdir rstack

for src_file in src/*.[ch]; do
    submit_file=rstack/${src_file#src/}
    cp $src_file $submit_file
done
cp src/Makefile.submission rstack/Makefile

cd rstack
tar -czvf 'rstack.tgz' *
mv rstack.tgz ..
cd -
