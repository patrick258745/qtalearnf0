##### computing tasks #####
doQtaSearch="1"
doSvrModelSelection="0"
doSvrPrediction="0"
doMlpModelSelection="0"
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
program_path="/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0"
praat="$program_path/tools/qtaf0/praat"
script="$program_path/tools/qtaf0/_qtaf0.praat"
search="\"Search for optimal qTA parameters\""
resynth="\"F0 resynthesis with qTA parameters\""

START_TIME=$SECONDS

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

if [ $doSvrModelSelection = 1 ]
then
  # model selection for support vector regression
  $program_path/bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;
fi

if [ $doSvrPrediction = 1 ]
then
  # predict targets using support vector regression
  $program_path/bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;

  # resynthesis with predicted svr targets
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/svr/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/svr/;

  if [ "$(uchardet $data_path/svr/corpus.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/svr/corpus.measures > $data_path/svr/corpus.measures.tmp
    mv $data_path/svr/corpus.measures.tmp $data_path/svr/corpus.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/svr/corpus.target --out $data_path/svr/corpus.stat --dir $data_path/svr/
fi

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
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/mlp/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/mlp/;

  if [ "$(uchardet $data_path/mlp/corpus.measures)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/mlp/corpus.measures > $data_path/mlp/corpus.measures.tmp
    mv $data_path/mlp/corpus.measures.tmp $data_path/mlp/corpus.measures
  fi

  # calculate statistics
  $program_path/bin/qtatools -s --in $data_path/mlp/corpus.target --out $data_path/mlp/corpus.stat --dir $data_path/mlp/
fi

ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
