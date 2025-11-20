#!/bin/sh

clear

echo "Compiling :"
make

echo "Testing with pi.c :"
./tests/pi.run
echo
echo "----------------------------------------"
echo

echo "Testing with simple_tests_outputs.c :"
./tests/simple_test_outputs.run
echo
echo "----------------------------------------"
echo 


echo "Testing with simple_tests.c :"
./tests/simple_test.run
echo
echo "----------------------------------------"
echo 

j=1
while [ "$j" -lt 30 ]
do
	echo "Testing with test_dependencies_outputs.c :"
	./tests/test_dependencies_outputs.run
	echo
	echo "----------------------------------------"
	echo


	echo "Testing with test_dependencies.c :"
	./tests/test_dependencies.run
	echo
	echo "----------------------------------------"
	echo 


	echo "Testing with fibo.c :"
	i=1
	while [ "$i" -lt 30 ]
	do
		echo "FIBO " expr "$i"
		./tests/fibo.run "$i"
		i=$((i+1))
		echo "   ---------------   "
		echo
	done
	echo
	echo "----------------------------------------"
	echo 
	j=$((j+1))
done

make $expr clean
