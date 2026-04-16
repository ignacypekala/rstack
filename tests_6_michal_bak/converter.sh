for file in ./output/test*_*.model.out; do
    filename=${file#./output/}
    testname=${filename%_*.model.out}
    casename=${filename#${testname}_}
    casename=${casename%.model.out}
    mkdir $testname
    mv "$file" "./$testname/${testname}_${casename}.fout"
done
