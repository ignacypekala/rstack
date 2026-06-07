#!/bin/bash

for batch in ./tests_*/; do
    batch_name=${batch#./tests_}
    batch_name=${batch_name%/}
    ./test-batch.sh $batch_name
    echo
done
