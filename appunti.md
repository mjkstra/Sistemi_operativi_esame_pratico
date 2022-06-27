# Sistemi operativi - esame pratico

# Table of contents  
- [Sistemi operativi - esame pratico](#sistemi-operativi---esame-pratico)
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
  - [C](#c)
    - [Librerie da includere](#librerie-da-includere)
    - [Direttive particolari](#direttive-particolari)
    - [Puntatori a funzione](#puntatori-a-funzione)
    - [Funzioni utili](#funzioni-utili)
***
<br>

## Premessa  

In questi appunti si trovano tutti gli strumenti per preparare e dare l'esame pratico di sistemi operativi. In aggiunta ci sarà del materiale proveniente dalle mie esperienze oppure dal manuale GNU che sarà complementare alle informazioni presenti sulle slide del corso \(che infatti sono incomplete\).<br><br>

## Bash  

### Hashbangs  

Ogni script bash inizia con una riga di questo tipo che serve per far capire al terminale con quale shell deve eseguire lo script  
```Bash
#!/usr/bin/env bash
```
<br>

### Comandi basici

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

### File descriptors e redirezionamento

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

### Variabili e array

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

### Variabili d'ambiente

In Linux esistono delle variabili d'ambiente, ossia variabili persistenti in ogni sessione, le principali:  

- **$SHELL**: indica la shell in utilizzo
- **$PWD**: indica la cartella corrente
- **$HOME**: indica la home directory
- **$PS1**: indica come è fatto il layout della riga di terminale iniziale<br><br>

### Concatenazione comandi

- In sequenza: `cmd1 ; cmd2`
- Solo se cmd1 ritorna 0: `cmd1 && cmd2`
- Se cmd1 ritorna 1 ignora cmd2 (**lazy evaluation**): `cmd1 || cmd2`

**NB:** In un file di script ricordo che ogni linea esegue un comando bash nello stesso terminale ( a differenza del [**Makefile**](#makefile) ). Se invece si vogliono eseguire sulla stessa riga bisogna usare la concatenazione.<br><br>

### Subshell

```Bash
a = $(echo "prova") # esegue il comando in una subshell e cattura SOLO lo standard output
(echo "prova") # esegue comando in una subshell
```
<br>

### Espansione aritmetica

```Bash
a = 1
(( a++ ))
(( a<1 ))
(( a=$b>$c?0:1 )) # operatore ternario
```
<br>

### Confronti

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

### Loops
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

## C

### Librerie da includere

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

### Direttive particolari

```C
#define F(A,B) A*B
#if
#else
#endif
```
<br>

### Puntatori a funzione

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

### Funzioni utili

```C
sprintf(dest, "format", src1,src2.. ); // stampa dentro dest (sovrascrivendolo) usando il formato specificato
char* token = strtok(buffer, delimiter_str); // tokenizza il buffer separandolo in base a delimiter(stringa). Viene rimosso il delimitatore dal token e la stringa originale viene modificata irreversibilmente
char* token = strtok(NULL,delimiter_str); // continua a tokenizzare
```