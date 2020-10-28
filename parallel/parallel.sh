#!/bin/bash

#Compilação de Código.
mpicxx -g -Wall -std=c++11 odd_even_prl.cpp -o odp

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
	tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

	for cores in 2 4 8 #números de cores utilizados
	do
			for size in 120000 135000 150000 168000  #tamanho do problema
			do   	
				echo -e "\n$cores\t$size\t\t\c" >> "parallel_cores_samples.txt" 

				for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
				do
					#Executar o código.
					mpiexec -np  $cores --oversubscribe ./odp $size >> "parallel_time.txt" 
				done
			done

	done
	
	exit 

rm -rf odp

exit
