START_TIME=$SECONDS

tools/qtaf0/praat --run tools/qtaf0/_qtaf0.praat \"Search for optimal qTA parameters\" 100 600 0 5 ../../test/corpus/ ../../test/corpus.target -50 50 80 110 1 80 ../../test/qta/;

tools/qtaf0/praat --run tools/qtaf0/_qtaf0.praat \"F0 resynthesis with qTA parameters\" 100 600 0 5 ../../test/corpus/ ../../test/corpus.target -50 50 80 110 1 80 ../../test/qta/;

bin/mlasampling --in test/corpus.sampa test/corpus.target --out test/corpus.sample

ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
