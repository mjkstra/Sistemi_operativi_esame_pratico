# Table of contents  
- [Table of contents](#table-of-contents)
- [Premessa](#premessa)
- [Bash](#bash)
  - [Hashbangs](#hashbangs)
  - [Comandi basici](#comandi-basici)
  - [File descriptors e redirezionamento](#file-descriptors-e-redirezionamento)
  - [Variabili e array](#variabili-e-array)
  - [Variabili d'ambiente](#variabili-dambiente)
  - [Concatenazione comandi](#concatenazione-comandi)
  - [Subshell](#subshell)
  - [Espansione aritmetica](#espansione-aritmetica)
  - [Confronti](#confronti)
  - [Loops](#loops)
- [Makefile](#makefile)
  - [Target speciali](#target-speciali)
  - [Funzioni speciali](#funzioni-speciali)
  - [Makefile esempio](#makefile-esempio)
- [C](#c)
  - [Librerie da includere](#librerie-da-includere)
  - [Direttive particolari](#direttive-particolari)
  - [Puntatori a funzione](#puntatori-a-funzione)
  - [Funzioni utili](#funzioni-utili)
  - [Files](#files)
    - [Streams](#streams)
    - [Files Descriptors](#files-descriptors)
    - [Duplicazione dei file descriptors](#duplicazione-dei-file-descriptors)
  - [Forking e system calls](#forking-e-system-calls)
***
<br>

# Premessa  

In questi appunti si trovano gli strumenti per preparare e dare l'esame pratico di sistemi operativi. In aggiunta ci sarà del materiale proveniente dalle mie esperienze oppure dal manuale GNU che sarà complementare alle informazioni presenti sulle slide del corso. Sottolineo che in questi appunti **verranno saltati i concetti più basilari** che ritengo prerequisiti, se si ha bisogno di quelli integrate con le slide su moodle.<br><br>

# Bash  

## Hashbangs  

Ogni script bash inizia con una riga di questo tipo che serve per far capire al terminale con quale shell deve eseguire lo script  
```Bash
#!/usr/bin/env bash
```
<br>

## Comandi basici

```Bash
    alias cmd='cmd2' # cmd diventa alias di cmd2
    unalias cmd # elimina l'alias
    file f.txt # da informazioni sul tipo di file
    type vim # da informazioni sul percorso del file
    truncate f.txt # riduce o espande la grandezza di un file
    read # legge linee da un file descriptor
    function nome { echo "$1" } # definizione di funzione che stampa il primo argomento
    nome () { echo "$1" } # analogo a sopra
```
<br>

## File descriptors e redirezionamento

I 3 file descriptor di default su linux sono:
- 1: **standard input**
- 2: **standard output**
- 3: **standard error**

Per redirezionare si utilizzano questi operatori:
- Per l'input da source a dest : `input_dest < input_source`
- Per l'output da source a dest : `output_source > output_dest` ( se si vuole fare append usare `>>` )
- Per l'output di source all'input di dest: `output_source | input_source`
- Per l'output e l'error di source all'input di dest: `output_source |& input_source`

Redirezionamenti speciali:
- Da un file descriptor f1 ad un altro f2: `f1>&f2`  
  ad esempio stderr su stdout: `2>&1`
- Mutare un flusso f: `f > /dev/null`
<br><br>

## Variabili e array

```Bash
a = 5 # dichiarazione variabile
echo "$a"; # utilizzo

lista=("a", 1, "b") # dichiarazione lista
${lista[@]} # tutti gli elementi della lista
${lista[x]} # accesso in posizione x
${!lista[@]} # tutti gli indici della lista
${#lista[@]} # dimensione lista
lista[x]=value # assegnamento in posizione x
lista+=(value) # pushback di un elemento
${lista[@]:s:n} # subarray di lista da indice s ad n

$$ # contiene il pid del processo attuale
$? # contiene lo stato dell'ultimo comando: 0 se ok altrimenti 1 o qualsiasi valore
$1,$2 ecc.. # parametri di input
```
<br>

## Variabili d'ambiente

In Linux esistono delle variabili d'ambiente, ossia variabili persistenti in ogni sessione, le principali:  

- **$SHELL**: indica la shell in utilizzo
- **$PWD**: indica la cartella corrente
- **$HOME**: indica la home directory
- **$PS1**: indica come è fatto il layout della riga di terminale iniziale<br><br>

## Concatenazione comandi

- In sequenza: `cmd1 ; cmd2`
- Solo se cmd1 ritorna 0: `cmd1 && cmd2`
- Se cmd1 ritorna 1 ignora cmd2 (**lazy evaluation**): `cmd1 || cmd2`

**NB:** In un file di script ricordo che ogni linea esegue un comando bash nello stesso terminale ( a differenza del [**Makefile**](#makefile) ). Se invece si vogliono eseguire sulla stessa riga bisogna usare la concatenazione.<br><br>

## Subshell

```Bash
a = $(echo "prova") # esegue il comando in una subshell e cattura SOLO lo standard output
(echo "prova") # esegue comando in una subshell
```
<br>

## Espansione aritmetica

```Bash
a = 1
(( a++ ))
(( a<1 ))
(( a=$b>$c?0:1 )) # operatore ternario
```
<br>

## Confronti

```Bash
# sintassi POSIX standard (funziona sempre):
[ $a <token> $b ] # <token>: -ge | -eq | -lt  ecc..
[ $str1 <token> $str2 ] # <token>: \> | \< | != | == | =
[ -f /tmp/prova ] # controlla se prova è un file
[ -e /tmp/prova ] # controlla se prova esiste
[ -d /tmp/prova ] # controlla se prova è una directory
# NB: posso usare ! tipo:
[ ! -e file ]

if <cond>; then
    <cmd_then>
else
    <cmd_else>
fi;

```
<br>

## Loops
```Bash
for i in ${!lista[@]}; do
    echo ${lista[$i]}
done

while [[ $i < 10 ]]; do
    echo $i ; (( i++ ))
done

nargs=$#
while [[ $1 != "" ]]; do
    echo "ARG=$1"
    shift # shifta gli argomenti di input a sinistra
done
```
<br><br>

# Makefile

I makefile sono costituiti da **target** ognuno dei quali ha dei **prerequisiti**. Invocando `make` viene eseguito il **primo target** del makefile e in caso abbia prerequisiti verranno risolti eseguendo i relativi target. Ogni target contiene una **ricetta**, ossia un insieme di istruzioni,su ogni riga **precedute da un tab**. Per ogni riga vengono eseguiti comandi SH e non BASH, perciò **la sintassi di bash non funzionerà**. **Ogni riga esegue i comandi su un nuovo terminale**, perciò se si vuole eseguire una sequenza atomica di comandi, bisognerà farlo sulla stessa riga concatenando i comandi ( `&&` oppure `;` ) oppure andando a capo usando \ per fare l'escape.<br><br>

Normalmente invocando `make` viene cercato un file di nome `Makefile` o `makefile`, se si vuole dare un nome diverso allora occorrerà invocare `make -f makefilealternativo`<br>

Le variabili in un Makefile si definiscono così: `A=1` oppure `A:=1` e si richiamano con `$(A)`<br>

Per eseguire un determinato target del Makefile: `make target`<br>

Per rappresentare una stringa qualsiasi in un target usare `%`. ES: ogni file con estensione `.c` si può rappresentare con `%.c`<br>

Per silenziare un particolare comando, apporre `@` all'inizio di esso.<br>

## Target speciali

- `.SILENT` : i target specificati come prerequisito non stamperanno nulla su stderr e stdout

## Funzioni speciali

- `$(eval ...)` : consente di creare nuove regole make dinamiche. ES: `$(eval VAR+=aggiunta)` concatena aggiunta a var.
- `$(shell ...)` : cattura l’output di un commando shell. ES: `PWD=$(shell pwd)`.
- `$(wildcard *)` : restituisce un elenco di file che corrispondono alla stringa specificata. ES: `OBJ_FILES:=$(wildcard *.o)`
<br><br>

## Makefile esempio  

In realtà non è altro che la mia soluzione dell'esame del 15-06-2022.

```Makefile
DEST=./

main:
	if [ -e $(DEST) ]; then \
		cd $(DEST); \
		gcc main.c -std=gnu90 -o coda; \
	else \
		echo "?ERROR" >&2; \
	fi;

cexp:
	gcc prove.c -std=gnu90 -o prove;

exp: cexp
	./prove

.SILENT:
```



# C

## Librerie da includere

```C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
```
<br>

## Direttive particolari

```C
#define F(A,B) A*B
#if
#else
#endif
```
<br>

## Puntatori a funzione

```C
return_type (*fn_pointer) (type_arg1, type_arg2); // Definizione di un puntatore a funzione fn_pointer

// Esempio
int (*fn_pointer) (int,int); // esempio

int f(int a, int b){
    return a*b;
}

fn_pointer = &f;

int result = (*fn_pointer)(1,2);
```
<br>

## Funzioni utili

```C
sprintf(dest, "format", src1,src2.. ); // stampa dentro dest (sovrascrivendolo) usando il formato specificato
char* token = strtok(buffer, delimiter_str); // tokenizza il buffer separandolo in base a delimiter(stringa). Viene rimosso il delimitatore dal token e la stringa originale viene modificata irreversibilmente
char* token = strtok(NULL,delimiter_str); // continua a tokenizzare
```
<br>

## Files  
***
<br>

### Streams

```C
// Dichiariamo uno stream
FILE* f = fopen("path_to_file","flags");

/* 
flags can be:
    ● r: read
    ● w: write or overwrite (create)
    ● r+: read and write (update existing)
    ● w+: read and write. (truncate if exists or create)
    ● a: write at end (update)
    ● a+: read and write at end (create)
*/

// Read from stream
int a;
char str[10];
fscanf(f,"%d %s", &a, str); // NB: la fscanf vuole le aree di memoria delle variabili infatti uso &a per ottenere l'indirizzo dell'intero, mentre la stringa è un indirizzo di memoria di per sé.

// Print to stream
fprintf(f,"intero: %d stringa: %s", a, str);

// gets string reading maximum dim_buffer-1 from stream
fgets(buffer,dim_buffer,f);

// gets character from stream
fgetc(character,f);

// gets line from stream
int characters_read = getline(&line, &dim_line, f);

// tells if stream is ended
feof(f);

// close stream
fclose(f);
```
<br>

### Files Descriptors

```C
// Open a file descriptor
int f = open("filename", flags, mode);
/*
Flags can be ( per unirli usare | ):
    ● Deve contenere uno tra O_RDONLY, O_WRONLY, o O_RDWR
    ● O_CREAT: crea il file se non esistente
    ● O_APPEND: apri il file in append mode
    ● O_TRUNC: cancella il contenuto del file (se aperto con W)
    ● O_EXCL: se usata con O_CREAT, fallisce se il file esiste già

Mode ( opzionale ) specifica i permessi da dare al file ( fare man open per vederli tutti, oppure 0777 per darli tutti ).
*/

// Read from file descripotr
int bytesRead = read(f,buffer,bytes_to_read);

// Create a file
int esito = creat(path_to_file, mode); // se va male ritorna -1

// Move the pointer in the stream counting from whence
lseek(f, offset_bytes, whence);

/*
Whence can be:
    ● SEEK_SET = da inizio file,
    ● SEEK_CUR = dalla posizione corrente
    ● SEEK_END = dalla fine del file.
*/

// Close file descriptor
close(f);
```

**NB:** gli stream principali sono **stdin**,**stderr**,**stdout** ; i loro file descriptor si ricavano chiamando **fileno(stream)** oppure usando le costanti **STDIN_FILENO**, **STDERR_FILENO**, **STDOUT_FILENO**.
<br><br>

### Duplicazione dei file descriptors

```C
// The dup() system call creates a copy of the file descriptor oldfd, using the lowest-numbered unused file descriptor for the new descriptor.
int f = dup(oldfd);

//The dup2() system call performs the same task as dup(), but instead of using the lowest-numbered unused file descriptor, it  uses  the  file  descriptor number specified in newfd.  If the file descriptor newfd was previously open, it is silently closed before being reused.
int f = dup2(int oldfd, int newfd);
```
<br>

## Forking e system calls

```C
/*
Queste chiamate sostituiscono il nostro programma eseguendo il programma specificato:
    - path: full path to file
    - file: filename considering the folder or $PATH
    - argv: pgm arguments (NULL terminated)
    - env: variables to pass. eg: "A=3,B=prova"
*/
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[],char *const envp[]);
int execl(const char *path, const char * arg0, ...,argn,NULL);
int execlp(const char *file, const char * arg0, ...,argn,NULL);
int execle(const char *file, const char * arg0, ...,argn,
NULL, char *const envp[]);
int execve(const char *filename, char *const argv[], char
*const envp[]);


//Executes a command in an SH shell ( BASH SINTAX DOES NOT WORK ! )
int system(const char * string)
```
<br>

