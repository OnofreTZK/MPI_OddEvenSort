 #!/bin/bash

#Compilação de Código.
g++ -g -Wall -std=c++11 odd_even_serial.cpp -o ods

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
	tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

	for cores in 1 # números de cores utilizados --> no código serial apenas 1.
	do
			for size in 105000 120000 125000 135000 #tamanho do problema
			do   	
				echo -e "\n$cores\t$size\t\t\c" >> "serial_cores_samples.txt" 

				for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
				do
					#Executar o código.
					./ods $size >> "serial_time.txt" 
				done
			done

	done
	
	exit 

rm -rf ods

exit
