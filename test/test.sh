##### computing tasks #####
doQtaSearch="0"
doSvrModelSelection="0"
doMlpModelSelection="0"

doLrrPrediction="1"
doKrrPrediction="0"
doSvrPrediction="0"
doMlpPrediction="0"

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
praat="tools/qtaf0/praat"
script="tools/qtaf0/_qtaf0.praat"
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

  # calculate statistics
  bin/qtatools -s --in $data_path/qta/corpus.target --out $data_path/qta/corpus.stat --dir $data_path/qta/

  # create training data
  bin/mlasampling --in $data_path/corpus.sampa $data_path/qta/corpus.target --out $data_path/corpus.sample;
fi

#################### LRR ####################

if [ $doLrrPrediction = 1 ]
then
  # predict targets using linear ridge regression
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/lrr/lrr.algorithm;

  # resynthesis with predicted lrr targets (test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/lrr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/lrr/test/;

  # resynthesis with predicted lrr targets (training)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/lrr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/lrr/training/;

  # calculate statistics
  bin/qtatools -s --in $data_path/lrr/training.target --out $data_path/lrr/training.stat --dir $data_path/lrr/training/
  bin/qtatools -s --in $data_path/lrr/test.target --out $data_path/lrr/test.stat --dir $data_path/lrr/test/
fi

#################### KRR ####################

if [ $doKrrPrediction = 1 ]
then
  # predict targets using linear ridge regression
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/krr/krr.algorithm;

  # resynthesis with predicted krr targets (test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/krr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/krr/test/;

  # resynthesis with predicted krr targets (training)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/krr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/krr/training/;

  # calculate statistics
  bin/qtatools -s --in $data_path/krr/training.target --out $data_path/krr/training.stat --dir $data_path/krr/training/
  bin/qtatools -s --in $data_path/krr/test.target --out $data_path/krr/test.stat --dir $data_path/krr/test/
fi

#################### SVR ####################
if [ $doSvrModelSelection = 1 ]
then
  # model selection for support vector regression
  bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;
fi

if [ $doSvrPrediction = 1 ]
then
  # predict targets using support vector regression
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;

  # resynthesis with predicted svr targets (test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/svr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/svr/test/;

  # resynthesis with predicted svr targets (training)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/svr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/svr/training/;

  # calculate statistics
  bin/qtatools -s --in $data_path/svr/training.target --out $data_path/svr/training.stat --dir $data_path/svr/training/
  bin/qtatools -s --in $data_path/svr/test.target --out $data_path/svr/test.stat --dir $data_path/svr/test/
fi

#################### MLP ####################
if [ $doMlpModelSelection = 1 ]
then
  # model selection for multi layer perceptron
  bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/mlp/mlp.algorithm;
fi

if [ $doMlpPrediction = 1 ]
then
  # predict targets using multi layer perceptron
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/mlp/mlp.algorithm;

  # resynthesis with predicted mlp targets
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/mlp/test.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/mlp/test/;

  # resynthesis with predicted mlp targets
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/mlp/training.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/mlp/training/;

  # calculate statistics
  bin/qtatools -s --in $data_path/mlp/training.target --out $data_path/mlp/training.stat --dir $data_path/mlp/training/
  bin/qtatools -s --in $data_path/mlp/test.target --out $data_path/mlp/test.stat --dir $data_path/mlp/test/
fi

ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
