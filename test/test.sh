##### computing tasks #####
doQtaSearch="1"
doSvrModelSelection="1"
doMlpModelSelection="1"

doLrrPrediction="1"
doKrrPrediction="1"
doSvrPrediction="1"
doMlpPrediction="1"

##### user data #####
fmin="100"
fmax="500"
smean="232.86"
mmin="-50"
mmax="50"
bmin="80"
bmax="110"
lmin="10"
lmax="80"
order="5"
shift="0"

##### parameters #####
data_path="$( cd "$( dirname "$0" )" && pwd )"
program_path="/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0"
praat="$program_path/tools/qtaf0/praat"
script="$program_path/tools/qtaf0/_qtaf0.praat"
search="\"Search for optimal qTA parameters\""
resynth="\"F0 resynthesis with qTA parameters\""

START_TIME=$SECONDS

#################### QTA ####################
if [ $doQtaSearch = 1 ]
then
  # search optimal qta parameters
  $praat --run $script $search $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/qta/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/qta/;

  # resynthesis with optimal qta parameters
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/qta/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/qta/;

  if [ "$(uchardet $data_path/qta/corpus.target)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/qta/corpus.target > $data_path/qta/corpus.target.tmp
    mv $data_path/qta/corpus.target.tmp $data_path/qta/corpus.target
  fi

  if [ "$(uchardet $data_path/qta/corpus.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/qta/corpus.measures > $data_path/qta/corpus.measures.tmp
    mv $data_path/qta/corpus.measures.tmp $data_path/qta/corpus.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/qta/corpus.target --out $data_path/qta/corpus.stat --dir $data_path/qta/

  # create training data
  $program_path/bin/mlasampling --in $data_path/corpus.sampa $data_path/qta/corpus.target --out $data_path/corpus.sample;
fi

#################### LRR ####################

if [ $doLrrPrediction = 1 ]
then
  # predict targets using linear ridge regression
  $program_path/bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/lrr/lrr.algorithm;

  # resynthesis with predicted lrr targets (test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/lrr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/lrr/test/;

  # resynthesis with predicted lrr targets (training)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/lrr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/lrr/training/;

  if [ "$(uchardet $data_path/lrr/test.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/lrr/test.measures > $data_path/lrr/test.measures.tmp
    mv $data_path/lrr/test.measures.tmp $data_path/lrr/test.measures
  fi

  if [ "$(uchardet $data_path/lrr/training.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/lrr/training.measures > $data_path/lrr/training.measures.tmp
    mv $data_path/lrr/training.measures.tmp $data_path/lrr/training.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/lrr/training.target --out $data_path/lrr/training.stat --dir $data_path/lrr/training/
  $program_path/bin/qtatools -s --in $data_path/lrr/test.target --out $data_path/lrr/test.stat --dir $data_path/lrr/test/
fi

#################### KRR ####################

if [ $doKrrPrediction = 1 ]
then
  # predict targets using linear ridge regression
  $program_path/bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/krr/krr.algorithm;

  # resynthesis with predicted krr targets (test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/krr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/krr/test/;

  # resynthesis with predicted krr targets (training)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/krr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/krr/training/;

  if [ "$(uchardet $data_path/krr/test.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/krr/test.measures > $data_path/krr/test.measures.tmp
    mv $data_path/krr/test.measures.tmp $data_path/krr/test.measures
  fi

  if [ "$(uchardet $data_path/krr/training.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/krr/training.measures > $data_path/krr/training.measures.tmp
    mv $data_path/krr/training.measures.tmp $data_path/krr/training.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/krr/training.target --out $data_path/krr/training.stat --dir $data_path/krr/training/
  $program_path/bin/qtatools -s --in $data_path/krr/test.target --out $data_path/krr/test.stat --dir $data_path/krr/test/
fi

#################### SVR ####################
if [ $doSvrModelSelection = 1 ]
then
  # model selection for support vector regression
  $program_path/bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;
fi

if [ $doSvrPrediction = 1 ]
then
  # predict targets using support vector regression
  $program_path/bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;

  # resynthesis with predicted svr targets (test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/svr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/svr/test/;

  # resynthesis with predicted svr targets (training)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/svr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/svr/training/;

  if [ "$(uchardet $data_path/svr/test.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/svr/test.measures > $data_path/svr/test.measures.tmp
    mv $data_path/svr/test.measures.tmp $data_path/svr/test.measures
  fi

  if [ "$(uchardet $data_path/svr/training.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/svr/training.measures > $data_path/svr/training.measures.tmp
    mv $data_path/svr/training.measures.tmp $data_path/svr/training.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/svr/training.target --out $data_path/svr/training.stat --dir $data_path/svr/training/
  $program_path/bin/qtatools -s --in $data_path/svr/test.target --out $data_path/svr/test.stat --dir $data_path/svr/test/
fi

#################### MLP ####################
if [ $doMlpModelSelection = 1 ]
then
  # model selection for multi layer perceptron
  $program_path/bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/mlp/mlp.algorithm;
fi

if [ $doMlpPrediction = 1 ]
then
  # predict targets using multi layer perceptron
  $program_path/bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/mlp/mlp.algorithm;

  # resynthesis with predicted mlp targets
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/mlp/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/mlp/test/;

  # resynthesis with predicted mlp targets
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/mlp/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/mlp/training/;

  if [ "$(uchardet $data_path/mlp/test.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/mlp/test.measures > $data_path/mlp/test.measures.tmp
    mv $data_path/mlp/test.measures.tmp $data_path/mlp/test.measures
  fi

  if [ "$(uchardet $data_path/mlp/training.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/mlp/training.measures > $data_path/mlp/training.measures.tmp
    mv $data_path/mlp/training.measures.tmp $data_path/mlp/training.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/mlp/training.target --out $data_path/mlp/training.stat --dir $data_path/mlp/training/
  $program_path/bin/qtatools -s --in $data_path/mlp/test.target --out $data_path/mlp/test.stat --dir $data_path/mlp/test/
fi

ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
