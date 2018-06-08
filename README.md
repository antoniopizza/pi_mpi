# Compute in parallel an approximation of PI
#### Programmazione Concorrente, Parallela e su Cloud
##### Università degli Studi di Salerno - Anno Accademico 2016/2017
##### Prof: Vittorio Scarano
##### Dott. Carmine Spagnuolo
##### Studente: Antonio Pizza 0522500574
#
### Problem statement
Calcolare il valore approssimato del π sfruttando il calcolo parallelo, sviluppando un programma in C utilizzando MPI. Lo scopo principale è quello di comparare due soluzione del calcolo del π fornite tramite la regola del Trapezio e tramite il metodo di Monte Carlo.  Verranno analizzati singolarmente i due metodi per poi confrontare le soluzioni. 
#
### Strumenti utilizzati

 1. 8 istanze Amazon EC2 m4.large (2 core) con StarCluster AMI (ami-52a0c53b)
 2. 1 nodo MASTER
 3. 7 nodi SLAVES
 
 #

### Soluzione proposta
Per entrambi gli algoritmi, lo scopo è quello di parallelizzare il processo di iterazioni di un ciclo, quindi è stato usato uno stesso approccio dove un nodo master si occupa dell'equa divisione delle porzioni da assegnare ad ogni nodo slave che provvederanno ad effettuare determinati calcoli per arrivare all'approssimazione finale del valore del π.

### Regola del trapezio

#### Descrizioni variabili
****Variabili MPI:****

```c
int rank; //rank del processore
int np; //numero totale di processori
```

****Variabili per il range:****
Per garantire la corretta esecuzione dell'algoritmo c'è bisogno di tener traccia del valore del numero dei cicli già effettuati in quanto questo valore viene utilizzato dagli slave per eseguire determinate operazioni.
```c
int inizio = 0;
int fine = 0;
```
****Calcolo del range:****
In questa sezione il nodo master calcola i valori del range per ogni slave inerenti alle iterazioni da effettuare per poi essere inviati. Utilizziamo due send per inviare i dati:  

 - la prima invia il valore da cui deve partire il cilo
 - la seconda che indica il valore di termine.

```c
int lenght = N / (np-1); 
int mod = (int)N % (np-1);
double result = 0.0, result_slave = 0.0, pi = 0.0;
for (int i = 1; i < np; i++) {
	MPI_Send(&inizio, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	int res = (i <= mod)?lenght+1:lenght;
	fine = inizio + res;
	MPI_Send(&fine, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	inizio = inizio + res;
}
```

****Calcolo del π MASTER:****
Una volta ricevuti tutti i risultati inviati dagli slave ed incrementato il valore di result, viene eseguita l'operazione di calcolo del π per poi essere stampato.
```c
for (int i = 1; i < np; i++) {
	MPI_Recv(&result_slave, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
	result += result_slave;
}
pi = 4 * d * result;
printf("pi=%lf\n", pi);
```

****Calcolo del π SLAVE:****
Nella seguente sezione vengono illustrate le operazioni svolte dai nodi slave, ovvero:

 - le due receive raccolgono i dati inviati dal master
 - il for calcola il valore di x2 che viene utilizzato nell'incremento di res
 - invia il risultato al master tramite una send. 

```c
double x2=0.0, res=0.0;

MPI_Recv(&inizio, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
MPI_Recv(&fine, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

for (int j = inizio; j < fine; j++) 
	x2=d2*j*j;
	res+=1.0/(1.0+x2);
}

MPI_Send(&res, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
```



### Metodoto Monte Carlo

Il metodo "Monte Carlo" è una strategia di risoluzione che utilizza la statistica per il calcolo del valore del π. L'idea di parallelizzare il problema è uguale a quella adottata per l'algoritmo del trapezio dove un nodo master divide il numero dei cicli in base al numero dei nodi slave che procederanno al calcolo dei valori utilizzati per ricavare il valore approssimato del π. 

#### Master
Nel primo ciclo, il nodo master calcola la porzione di iterazioni di ogni nodo slave, mentre nel secondo riceve il valori calcolati dai nodi slave aggiornando anche il valore di count che servirà per il calcolo del π.

 ```c
for (int i = 1; i < np; i++) {
	res = (i<=mod)?lenght+1:lenght;
	printf("slave%d -- res%d\n", i, res);
	MPI_Send(&res, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
}
        
for (int i = 1; i < np; i++) {
	MPI_Recv(&count_slave, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
	count+=count_slave;
}

pi=(double)count/niter*4;
```


Una volta ricevuti tutti i valori calcolati dagli slave calcola il valore approssimato del π


 #### Slave
Ricevuti il numero iterazioni da compiere, ogni slave eseguirà le operazioni nel for per poi inviare il risultato di count al nodo master tramite una send.

```c
 MPI_Recv(&res, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
 for (int j = 0; j < res; j++) {
	x = (double)rand()/RAND_MAX;
	y = (double)rand()/RAND_MAX;
	z = x*x+y*y;
	if (z<=1) count++;
 }
 MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
```




#### Confronto soluzioni
Analizzando le soluzioni dei due algoritmi eseguiti con lo stesso input possiamo dedurre che il calcolo del π è più preciso se eseguito con il metodo del trapezio rispetto a quello eseguito con Monte Carlo. Questo succede perché col metodo Monte Carlo il numero totale di iterazioni verrà diviso tra i vari slave ed i valori casuali calcolati tramite un seme fisso, saranno dimezzati e uguali per ogni slave rendendo così la soluzione meno precisa rispetto al metodo del trapezio.
