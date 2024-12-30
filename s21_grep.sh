#!/bin/bash

make clean
make

file_number=2
test_number=1
test_passed=0
test_failed=0
failed_tests=()

e="-e"
f="-f"
flags=("-i" "-v" "-c" "-l" "-n" "-h" "-s" "-o")
flag_combination=("-ii" "-iv" "-ic" "-il" "-in" "-ih" "-is" "-io" "-vv" "-vc" "-vl" "-vn" "-vh" "-vs" "-vo" "-cc" "-cl" "-cn" "-ch" "-cs" "-co" "-ll" "-ln" "-lh" "-ls" "-lo" "-nn" "-nh" "-ns" "-no" "-hh" "-hs" "-ho" "-ss" "-so" "-oo")

pattern="argument"
pattern_second="slow"

patternfile="pattern"

not_exist="not.txt"
directory="../common/"
restricted="restricted"
empty="empty"
file="../../materials/topic-list.md"

pattern_and_file=("$pattern" "$pattern_second" "$patternfile" "$not_exist" "$directory" "$restricted" "$empty" "$file")

run_test(){
    local output="$1"       
    local expected="$2"      
    local test_number="$3"   
    local test_name="$4"     

    if [ "$output" == "$expected" ]; then
        ((test_passed++))
        echo $test_number $arg "PASSED"
        
    else
        ((test_failed++))
        failed_tests+=("$test_number: $test_name")   
    fi
}

loop_terminal(){
    local array=("$@")
    for arg in "${array[@]}"; do
        if [[ $arg == *"v"* ]]; then  
            output=$(echo hello | ./s21_grep $arg 2>/dev/null)
            expected=$(echo hello | grep $arg 2>/dev/null)
        else
            output=$(echo argument | ./s21_grep $arg 2>/dev/null)
            expected=$(echo argument | grep $arg 2>/dev/null)
        fi
        description="'$arg'"
        run_test "$output" "$expected" $test_number "$description"
        test_number=$((test_number + 1))
    done
}

loop_combinations(){
    local array=("$@")
    for arg in "${array[@]}"; do
        output=$(./s21_grep $arg 2>/dev/null)
        expected=$(grep $arg 2>/dev/null)
        description="'$arg'"
        run_test "$output" "$expected" $test_number "$description"
        test_number=$((test_number + 1))
    done
}


terminal_combination=()
all_combination=()
bonus_combination=()

terminal_combination_function(){
    terminal_combination+=("$pattern")
    for flag_comb in "${flag_combination[@]}"; do
        terminal_combination+=("$flag")
        terminal_combination+=("$flag $flag")
        terminal_combination+=("$flag $pattern")
        for flag in "${flags[@]}"; do
            terminal_combination+=("$flag_comb")
            terminal_combination+=("$flag_comb $flag_comb")
            terminal_combination+=("$flag_comb $pattern")
        done
    done    
}

generate_combination(){
    for pattern in "${pattern_and_file[@]}"; do
        for file in "${pattern_and_file[@]}"; do
                for flag in "${flags[@]}"; do
                    all_combination+=("$pattern $file")
                    all_combination+=("$flag $pattern $file")
                    all_combination+=("$e $pattern $file")
                    all_combination+=("$pattern $e $pattern $file")
                    all_combination+=("$flag $pattern $e $pattern $file")
                    all_combination+=("$f $pattern $file")
                    all_combination+=("$pattern $f $pattern $file")
                    all_combination+=("$flag $pattern $f $pattern $file")
                    all_combination+=("$e $pattern $f $pattern $file")
                    all_combination+=("$flag $e $pattern $f $pattern $file")
                done
        done
    done
}

bonus_combination_function(){
for pattern in "${pattern_and_file[@]}"; do
    for file in "${pattern_and_file[@]}"; do
        for flag_comb in "${flag_combination[@]}"; do
            bonus_combination+=("$flag_comb $pattern $file")
            bonus_combination+=("$flag_comb $e $pattern $file")
            bonus_combination+=("$flag_comb $pattern $e $pattern $file")
            bonus_combination+=("$flag_comb $f $pattern $file")
            bonus_combination+=("$flag_comb $pattern $f $pattern $file")
        done
    done
done
}


test_cat(){

    terminal_combination_function
    loop_terminal "${terminal_combination[@]}"

    generate_combination
    loop_combinations "${all_combination[@]}"

    bonus_combination_function
    loop_combinations "${bonus_combination[@]}"
}

test_cat

echo "================ TEST RESULTS ================"
echo "Total tests: $((test_number - 1))"
echo "Passed tests: $test_passed"
echo "Failed tests: $test_failed"

if ((test_failed > 0)); then
    echo "Failed test details:"
    for test in "${failed_tests[@]}"; do
        echo "$test"
    done
fi

