##### computing tasks #####
doQtaSearch="1"
doModelSelection="0"
doPrediction="0"

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
store="1"

##### parameters #####
data_path="$( cd "$( dirname "$0" )" && pwd )"
praat="tools/qtaf0/praat"
script="tools/qtaf0/_qtaf0.praat"
search="\"Search for optimal qTA parameters\""
resynth="\"F0 resynthesis with qTA parameters\""

##### qta #####
function qtaSearch {
  START_TIME=$SECONDS

  # search optimal qta parameters
  #$praat --run $script $search $fmin $fmax $smean $shift $order $data_path/qta/corpus/ $data_path/qta/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/qta/;

  # resynthesis with optimal qta parameters
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/qta/corpus/ $data_path/qta/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/qta/;

  # calculate statistics
  bin/qtatools -s --in $data_path/qta/corpus.target --out $data_path/qta/corpus.stat --dir $data_path/qta/

  # create training data
  bin/mlasampling --in $data_path/corpus.sampa $data_path/qta/corpus.target --out $data_path/corpus.sample;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

##### lrr #####

function lrrPredict {
  START_TIME=$SECONDS

  # predict targets using linear ridge regression
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/lrr/lrr.algorithm;

  # resynthesis with predicted lrr targets (training and test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/lrr/corpus/ $data_path/lrr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/lrr/training/;
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/lrr/corpus/ $data_path/lrr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/lrr/test/;

  # calculate statistics (training and test)
  bin/qtatools -s --in $data_path/lrr/training.target --out $data_path/lrr/training.stat --dir $data_path/lrr/training/;
  bin/qtatools -s --in $data_path/lrr/test.target --out $data_path/lrr/test.stat --dir $data_path/lrr/test/;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

##### krr #####

function krrPredict {
  START_TIME=$SECONDS

  # predict targets using kernel ridge regression
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/krr/krr.algorithm;

  # resynthesis with predicted krr targets (training and test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/krr/corpus/ $data_path/krr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/krr/training/;
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/krr/corpus/ $data_path/krr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/krr/test/;

  # calculate statistics (training and test)
  bin/qtatools -s --in $data_path/krr/training.target --out $data_path/krr/training.stat --dir $data_path/krr/training/;
  bin/qtatools -s --in $data_path/krr/test.target --out $data_path/krr/test.stat --dir $data_path/krr/test/;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

##### svr #####
function svrModelSelection {
  START_TIME=$SECONDS

  # model selection for support vector regression
  bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

function svrPredict {
  START_TIME=$SECONDS

  # predict targets using support vector regression
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;

  # resynthesis with predicted svr targets (training and test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/svr/corpus/ $data_path/svr/training.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/svr/training/;
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/svr/corpus/ $data_path/svr/test.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/svr/test/;

  # calculate statistics (training and test)
  bin/qtatools -s --in $data_path/svr/training.target --out $data_path/svr/training.stat --dir $data_path/svr/training/;
  bin/qtatools -s --in $data_path/svr/test.target --out $data_path/svr/test.stat --dir $data_path/svr/test/;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

##### mlp #####
function mlpModelSelection {
  START_TIME=$SECONDS

  # model selection for multi layer perceptron
  bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/mlp/mlp.algorithm;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

function mlpPredict {
  START_TIME=$SECONDS

  # predict targets using multi layer perceptron
  bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/mlp/mlp.algorithm;

  # resynthesis with predicted mlp targets (training and test)
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/mlp/corpus/ $data_path/mlp/training.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/mlp/training/;
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/mlp/corpus/ $data_path/mlp/test.target $mmin $mmax $bmin $bmax $lmin $lmax $store $data_path/mlp/test/;

  # calculate statistics (training and test)
  bin/qtatools -s --in $data_path/mlp/training.target --out $data_path/mlp/training.stat --dir $data_path/mlp/training/;
  bin/qtatools -s --in $data_path/mlp/test.target --out $data_path/mlp/test.stat --dir $data_path/mlp/test/;

  ELAPSED_TIME=$(($SECONDS - $START_TIME))
  echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
}

#################################################################################################################################
########################################################## main script ##########################################################
#################################################################################################################################

START_TIME_ALL=$SECONDS

##### print parameters
echo "[bash] used parameters:"
echo "    f0 analysis [$fmin,$fmax] Hz"
echo "    speaker onset mean: $smean Hz"
echo "    slope [$mmin,$mmax] st/s"
echo "    offset [$bmin,$bmax] st"
echo "    strength [$lmin,$lmax] 1/s"
echo "    timeshift: $shift s"
echo "    order: $order"
echo ""

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

##### qta search
if [ $doQtaSearch = 1 ]
then
  qtaSearch;
  echo "";
fi

##### model selection
if [ $doModelSelection = 1 ]
then
  svrModelSelection;
  echo "";
  mlpModelSelection;
  echo "";
fi

##### prediction
if [ $doPrediction = 1 ]
then
  lrrPredict &
  krrPredict &
  svrPredict &
  mlpPredict;
  wait;
  echo "";
fi

ELAPSED_TIME_ALL=$(($SECONDS - $START_TIME_ALL))
echo ">>>>>> $(($ELAPSED_TIME_ALL/60)) min $(($ELAPSED_TIME_ALL%60)) sec <<<<<<"

