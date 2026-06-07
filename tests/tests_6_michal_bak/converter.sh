for file in ./output/test*_*.model.out; do
    filename=${file#./output/}
    testname=${filename%_*.model.out}
    casename=${filename#${testname}_}
    casename=${casename%.model.out}
    mkdir $testname 2>/dev/null
    mv "$file" "./$testname/${testname}_${casename}.fout" 
done

rmdir output
rm tester.c
rm rstack.h

for cfile in ./test*.c; do
    nvim -s macro.txt $cfile
done

