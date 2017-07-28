##### computing tasks #####
doQtaSearch="0"
doSvrPrediction="1"
doSvrModelSelection="0"

##### user data #####
fmin="100"
fmax="600"
smean="232.86"
mmin="-10"
mmax="10"
bmin="85"
bmax="105"
lmin="30"
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
  $praat --run $script $search $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/qta/;

  if [ "$(uchardet $data_path/corpus.target)" = "UTF-16" ]
  then
    iconv -f UTF-16 -t UTF-8 $data_path/corpus.target > $data_path/corpus.target.tmp
    mv $data_path/corpus.target.tmp $data_path/corpus.target
  fi

  # resynthesis with optimal qta parameters
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/corpus.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/qta/;

  # create training data
  $program_path/bin/mlasampling --in $data_path/corpus.sampa $data_path/corpus.target --out $data_path/corpus.sample;
fi

if [ $doSvrPrediction = 1 ]
then
  # predict targets using support vector regression
  $program_path/bin/mlatraining -p --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;

  # resynthesis with predicted svr targets
  $praat --run $script $resynth $fmin $fmax $smean $shift $order $data_path/corpus/ $data_path/svr/prediction.target $mmin $mmax $bmin $bmax $lmin $lmax $data_path/svr/;
fi

if [ $doSvrModelSelection = 1 ]
then
  # predict targets using support vector regression
  $program_path/bin/mlatraining -m --in $data_path/corpus.sample --alg $data_path/svr/svr.algorithm;
fi

ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo ">>> $(($ELAPSED_TIME/60)) min $(($ELAPSED_TIME%60)) sec <<<"
