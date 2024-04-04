## Tests T0 - IIC2333

### Instrucciones

#### Parte 1: ```runner```

1. Copiar el directorio ```testsT0``` dentro del directorio de ```T0```.

2. Desde el directorio ```P1```, ejecutar el comando ```$ make```.

3. Para ejcutar el i-ésimo test deben utilizar el siguente comando desde el directorio donde se encuentra ```T0```:
````
$ ./runner ./testsT0/P1/inputs/test{i}.txt output.csv
````
4. El output esperado del i-ésimo test corresponde a ```testsT0/P1/outputs/outputs{i}.csv```.


#### Tests

A continuación, se presentan los comando usados para obtener los outputs esperados. Los comandos a ejecutar de cada test considerando el ```<amount>``` y el ```<max>```.

- output1.csv: ```$ ./runner ./testsT0/P1/inputs/test1.txt output.csv 1```
- output2.csv: ```$ ./runner ./testsT0/P1/inputs/test2.txt output.csv 1```
- output3.csv: ```$ ./runner ./testsT0/P1/inputs/test3.txt output.csv 6```
- output4.csv: ```$ ./runner ./testsT0/P1/inputs/test3.txt output.csv 3```
- output5.csv: ```$ ./runner ./testsT0/P1/inputs/test3.txt output.csv 3 4```

#### Parte 2: ```schedulesly```

1. Para ejecutar el i-ésimo test deben utilizar el siguente comando desde el directorio ```T0```:
````
$ ./schedulesly ./testsT0/P2/inputs/in{i}.txt output.txt
````
Considerar que los tests de la Parte 2, el ```i``` se representa de la siguiente manera 01, 02, 03, 04, 05, 06 y 07.

2. El output esperado del i-ésimo test corresponde a ```testsT0/P1/outputs/out{i}.txt```.
