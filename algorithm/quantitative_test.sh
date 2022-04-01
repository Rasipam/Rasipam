#!/bin/bash  

cpp_module=$1
py_data_generator=$2

echo "[]" > insert_tactics.json

mkdir quantitative_test
cd quantitative_test

exp0_init_time=0
exp0_glob_time=0
exp0_count_time=0

for i in $(seq 1 10)
do
  echo "[New Module] exp0...$i time"
  python ../$py_data_generator -p5 -a3 -z10 --nrSequence=500 --nrOfSequence=10 --target=quantitative_base_0_$i.dat
  gtimeout 100s ../$cpp_module quantitative_base_0_$i.dat 1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp0_init_time=$exp0_init_time+$test_time
  gtimeout 100s ../$cpp_module quantitative_base_0_$i.dat 2,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp0_glob_time=$exp0_glob_time+$test_time
  exp0_count_time=`echo $exp0_count_time + 1 | bc`
done

exp0_init_time=`echo "scale=2 ; ($exp0_init_time) / $exp0_count_time" | bc`
exp0_glob_time=`echo "scale=2 ; ($exp0_glob_time) / $exp0_count_time" | bc`
echo "exp0: $exp0_init_time $exp0_glob_time" >> exp_results.txt


exp1_init_time=0
exp1_glob_time=0
exp1_count_time=0

for i in $(seq 1 10)  
do
  echo "[New Module] exp1...$i time"
  python ../$py_data_generator -p5 -a3 -z10 --nrSequence=700 --nrOfSequence=10 --target=quantitative_base_1_$i.dat
  gtimeout 100s ../$cpp_module quantitative_base_1_$i.dat 1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp1_init_time=$exp1_init_time+$test_time
  gtimeout 100s ../$cpp_module quantitative_base_1_$i.dat 2,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp1_glob_time=$exp1_glob_time+$test_time
  exp1_count_time=`echo $exp1_count_time + 1 | bc`
done

exp1_init_time=`echo "scale=2 ; ($exp1_init_time) / $exp1_count_time" | bc`
exp1_glob_time=`echo "scale=2 ; ($exp1_glob_time) / $exp1_count_time" | bc`
echo "exp1: $exp1_init_time $exp1_glob_time" >> exp_results.txt


exp2_init_time=0
exp2_glob_time=0
exp2_count_time=0

for i in $(seq 1 10)  
do
  echo "[New Module] exp2...$i time"
  python ../$py_data_generator -p5 -a3 -z10 --nrSequence=500 --nrOfSequence=20 --target=quantitative_base_2_$i.dat
  gtimeout 100s ../$cpp_module quantitative_base_2_$i.dat 1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp2_init_time=$exp2_init_time+$test_time
  gtimeout 100s ../$cpp_module quantitative_base_2_$i.dat 2,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp2_glob_time=$exp2_glob_time+$test_time
  exp2_count_time=`echo $exp2_count_time + 1 | bc`
done

exp2_init_time=`echo "scale=2 ; ($exp2_init_time) / $exp2_count_time" | bc`
exp2_glob_time=`echo "scale=2 ; ($exp2_glob_time) / $exp2_count_time" | bc`
echo "exp2: $exp2_init_time $exp2_glob_time" >> exp_results.txt


exp3_init_time=0
exp3_glob_time=0
exp3_count_time=0

for i in $(seq 1 10)  
do
  echo "[New Module] exp3...$i time"
  python ../$py_data_generator -p5 -a5 -z10 --nrSequence=500 --nrOfSequence=10 --target=quantitative_base_3_$i.dat
  gtimeout 100s ../$cpp_module quantitative_base_3_$i.dat 1,1,1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp3_init_time=$exp3_init_time+$test_time
  gtimeout 100s ../$cpp_module quantitative_base_3_$i.dat 2,1,1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp3_glob_time=$exp3_glob_time+$test_time
  exp3_count_time=`echo $exp3_count_time + 1 | bc`
done

exp3_init_time=`echo "scale=2 ; ($exp3_init_time) / $exp3_count_time" | bc`
exp3_glob_time=`echo "scale=2 ; ($exp3_glob_time) / $exp3_count_time" | bc`
echo "exp3: $exp3_init_time $exp3_glob_time" >> exp_results.txt


exp4_init_time=0
exp4_glob_time=0
exp4_count_time=0

for i in $(seq 1 10)  
do
  echo "[New Module] exp4...$i time"
  python ../$py_data_generator -p5 -a3 -z20 --nrSequence=500 --nrOfSequence=10 --target=quantitative_base_4_$i.dat
  gtimeout 100s ../$cpp_module quantitative_base_4_$i.dat 1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp4_init_time=$exp4_init_time+$test_time
  gtimeout 100s ../$cpp_module quantitative_base_4_$i.dat 2,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp4_glob_time=$exp4_glob_time+$test_time
  exp4_count_time=`echo $exp4_count_time + 1 | bc`
done

exp4_init_time=`echo "scale=2 ; ($exp4_init_time) / $exp4_count_time" | bc`
exp4_glob_time=`echo "scale=2 ; ($exp4_glob_time) / $exp4_count_time" | bc`
echo "exp4: $exp4_init_time $exp4_glob_time" >> exp_results.txt


exp5_init_time=0
exp5_glob_time=0
exp5_count_time=0

for i in $(seq 1 10) 
do
  echo "[New Module] exp5...$i time"
  python ../$py_data_generator -p10 -a3 -z10 --nrSequence=500 --nrOfSequence=10 --target=quantitative_base_5_$i.dat
  gtimeout 100s ../$cpp_module quantitative_base_5_$i.dat 1,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp5_init_time=$exp5_init_time+$test_time
  gtimeout 100s ../$cpp_module quantitative_base_5_$i.dat 2,1,1 0 -1 0 10000
  exit_status=$?
  if [[ $exit_status -eq 124 ]]; then
    continue
  fi
  test_time=$(<test_time.txt)
  exp5_glob_time=$exp5_glob_time+$test_time
  exp5_count_time=`echo $exp5_count_time + 1 | bc`
done

exp5_init_time=`echo "scale=2 ; ($exp5_init_time) / $exp5_count_time" | bc`
exp5_glob_time=`echo "scale=2 ; ($exp5_glob_time) / $exp5_count_time" | bc`
echo "exp5: $exp5_init_time $exp5_glob_time" >> exp_results.txt
