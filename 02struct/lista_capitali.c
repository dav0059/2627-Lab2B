#define _GNU_SOURCE   // avverte che usiamo le estensioni GNU 
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa exit() etc ...
#include <stdbool.h>  // gestisce tipo bool
#include <assert.h>   // permette di usare la funzione ass
#include <string.h>   // funzioni per stringhe
#include <errno.h>    // rischiesto per usare errno
#include <math.h>

// Scopo del programma:
// imparare a costruire, visualizzare e distruggere le liste in C


// prototipi delle funzioni che appaiono dopo il main()
void termina(const char *messaggio);


// definzione struct che rappresenta
// una città con nome, e coordinate 
// + campo next per formare la lista 
typedef struct capit {
  char *nome;
  double lat;
  double lon;
  struct capit *next;
} capitale;


void capitale_stampa(const capitale *a, FILE *f) {
  fprintf(f,"%20s (%f,%f)\n",a->nome,a->lat,a->lon);
}

capitale *capitale_crea(char *s, double lat, double lon)
{
  capitale *a  = malloc(sizeof(*a));
  if(a==NULL) termina("Allocazione capitale fallita");
  a->lat = lat;
  a->lon = lon;
  a->nome = strdup(s); // creo una copia di s e l'assegno al nome
  a->next = NULL;
  return a;
}

void capitale_distruggi(capitale *a)
{
  free(a->nome);
  free(a);
}

// stampa tutti gli elementi della lista lis
void lista_capitale_stampa(const capitale *lis, FILE *f)
{
  while(lis!=NULL) {
    capitale_stampa(lis,f);
    lis = lis->next;
  }
  return;
}

// distrugge (dealloca) tutti gli elementi della lista lis
void lista_capitale_distruggi(capitale *lis)
{
  while(lis!=NULL) {
    capitale *tmp = lis->next; // necessario
    capitale_distruggi(lis);
    lis = tmp;
  }
  return;
}


// legge un oggetto capitale dal file f
// restituisce il puntatore all'oggetto letto
// oppure NULL se non riesce a leggere dal file
// (funzione già vista per array di capitali)
capitale *capitale_leggi(FILE *f)
{
  assert(f!=NULL);
  char *s;
  double lat, lon;
  int e = fscanf(f,"%ms %lf %lf",&s,&lat,&lon);
  if(e!=3) {
    if(e==EOF) return NULL; // finito il file non ci sono altri dati
    else termina("Formato input file non valido");
  }
  capitale *c = capitale_crea(s,lat,lon);
  free(s);
  return c;
}

// crea una lista con gli oggetti capitale letti da 
// *f inserendoli ogni volta in testa alla lista
capitale *crea_lista_testa(FILE *f)
{
  // costruzione lista leggendo capitali dal file
  capitale *testa=NULL;
  while(true) {
    capitale *b = capitale_leggi(f);
    if(b==NULL) break;
    // inserisco b in testa alla lista
    b->next = testa;
    testa = b;
  }  
  return testa;
}


// crea una lista con gli oggetti capitale letti da 
// *f inserendoli ogni volta in coda alla lista
// durante la costruzione mantiene un puntatore in 
// testa e in coda alla lista restituisce solo il puntatore 
// alla testa della lista
capitale *crea_lista_coda(FILE *f) {
  capitale *testa = NULL; // serve per il return
  capitale *coda = NULL;  // serve per l'inserimento in coda
  while(true) {
    capitale *b = capitale_leggi(f);
    if(b==NULL) break;
    // inserisco b in coda alla lista
    if(coda==NULL) {
      // caso lista vuota
      testa = coda = b;
    } else {
      // caso lista non vuota
      coda->next = b;
      coda = b;
    }
    coda->next=NULL;
  }
  return testa;
}


// cancella da una lista il primo elemento con nome 
// uguale alla stringa s versione ricorsiva
capitale *cancella_nome(capitale *testa, char *s)
{
  assert(s!=NULL);
  if(testa==NULL) return NULL; // lista vuota non c'è nulla da cancellare
  // verifico se il primo elemento va cancellato
  if(strcmp(testa->nome,s)==0) {
    capitale *tmp = testa->next;
    capitale_distruggi(testa);
    return tmp;
  }
  // ora so che il primo elemento non va cancellato
  // e che la lista non è vuota
  testa->next = cancella_nome(testa->next,s);
  return testa;
}

capitale *filter(capitale *testa, char *s)
{
  assert(s != NULL); 
  if(testa == NULL) return NULL; 
  if(strcmp(testa -> nome, s) == 0){ 
    capitale *tmp = testa -> next;
    capitale_distruggi(testa);  
    return filter(tmp, s); 
  }
  testa -> next = filter(testa -> next, s); 
  return testa;  
}

// [1, 2, 3, 4, 5]
// 1. 1 % 2 == 1 -> 1::filter[2, 3, 4, 5]
// 2. 2 % 2 == 0 -> 1::filter[3, 4, 5]
// 3. 3 % 2 == 1 -> 1::3::filter[4, 5]
// 4. 4 % 2 == 0 -> 1::3::filter[5] 
// 5. 5 % 2 == 1 -> 1::3::5::filter[] = 1::3::5::[] 

capitale *filter_iter(capitale *lis, char *s)
{
  assert(s != NULL); 
  
  capitale *prec = NULL; 
  capitale *testa = lis; 
  while(lis != NULL) {
    capitale *succ = lis -> next; 
    if(succ == NULL) {
      if(strcmp(lis -> nome, s) == 0) {
        if(prec != NULL) 
          prec -> next = NULL;
        else 
          testa = NULL;  
        capitale_distruggi(lis);  
      } 
      return testa; 
    }
    if(strcmp(succ -> nome, s) == 0) {
      lis -> next = succ -> next;  
      capitale_distruggi(succ); 
    } else {
      prec = lis; 
      lis = lis -> next; 
    } 
  } 
  return testa; 
}

// come sopra bozza versione iterativa; non 
capitale *cancella_nome_iter(capitale *lis, char *s)
{
  capitale *testa = lis; // salvo puntatore primo elementi
  capitale *prec = NULL; // predecessore di lis
  while(lis!=NULL) {
    if(strcmp(lis->nome,s)==0) {
      // qui è necessario eliminare dalla lista
      // l'oggetto a cui punta lis
      if(prec!=NULL) 
        prec->next = lis->next;
      else
        testa = lis->next;
      capitale_distruggi(lis);
      break;
    }
    prec = lis; 
    lis = lis->next;
    assert(prec->next==lis);
  }
  // ora è necessario restituire il puntatore al 
  // primo elemento della nuova lista: è cambiato
  // se l'elemento che era testa è stato cancellato
  return testa; 
}


// inserisce capitale "c" in lista "testa" 
// mantenendo ordinamento per latitudine decrescente
capitale *inserisci_lat_ric(capitale *testa, capitale *c)
{
  assert(c!=NULL);
  // base riscorsione, lista vuota
  if(testa==NULL) {
    c->next=NULL;
    return c; //  la nuova lista contiene solo c
  }
  // la lista non è vuota
  if(c->lat > testa->lat) {
    // inserisco c in cima
    c->next = testa;
    return c;
  }
  else { // c va inserito "in mezzo" alla lista  
    testa->next = inserisci_lat_ric(testa->next,c);
    return testa;
  }
}


// inserisce capitale "c" in lista "testa" 
// mantenendo ordinamento per latitudine decrescente
// bozza versione iterativa: non funzionante
// da completare per casa
capitale *inserisci_lat_iter(capitale *testa, capitale *c)
{
  assert(c!=NULL);
  capitale *p = testa;
  capitale *prec = NULL; 
  while(p!=NULL) {
    // l'elemento c deve essere inserito dopo quelli
    // che hanno latitudine maggiore di c->lat e
    // prima di quelli che hanno latitudine minore di c->lat
    if(c -> lat >= p -> lat) {
      if(prec == NULL) {
        c -> next = p; 
        return c; 
      } 
      prec -> next = c; 
      c -> next = p;  
      return testa; 
    }
    prec = p; 
    p = p -> next; 
  }    
  if(prec == NULL){ 
    c -> next = NULL;
    return c; 
  }
  prec -> next = c; 
  c -> next = NULL; 
  return testa; 
}

capitale *inserisci_ord_iter(capitale *testa, capitale *c, int(*comp)(capitale *, capitale *))
{
  assert(c != NULL); 
  capitale *p = testa; 
  capitale *prec = NULL; 
  while(true) {
    //la lista è finita
    if(p == NULL) {
      if(prec == NULL) {
        c -> next = NULL; 
        return c; 
      }
      prec -> next = c; 
      c -> next = p;
      return testa; 
    }
    //la lista non è finita e sono nel punto in cui 
    //va inserita la capitale
    if((*comp)(c, p) >= 0) {
      if(prec == NULL) {
        c -> next = p; 
        return c; 
      } 
      prec -> next = c; 
      c -> next = p; 
      return testa; 
    }
    //la lista non è finita e non sono ancora nel punto 
    //in cui va inserita la capitale 
    prec = p; 
    p = p -> next; 
  }
}

// crea una lista con gli oggetti capitale letti da 
// *f inserendoli per latitudini descrescente
capitale *crea_lista_lat(FILE *f)
{
  // costruzione lista leggendo capitali dal file
  capitale *testa=NULL;
  while(true) {
    capitale *b = capitale_leggi(f);
    if(b==NULL) break;
    // inserisco b in testa alla lista    
    testa = inserisci_lat_iter(testa,b); 
  }
  return testa;
}

capitale *crea_lista_ord(int (*comp)(capitale *, capitale *), FILE *f)
{
  capitale *testa = NULL; 
  while(true) {
    capitale *b = capitale_leggi(f); 
    if(b == NULL) break; 
    testa = inserisci_ord_iter(testa, b, comp); 
  }
  return testa; 
}

int capitale_lex_comp(capitale *a, capitale *b)
{
  assert(a != NULL); 
  assert(b != NULL); 
  return strcmp(a -> nome, b -> nome); 
}

int main(int argc, char *argv[])
{
  if(argc!=2) {
    printf("Uso: %s nomefile\n",argv[0]);
    exit(1);
  } 
  FILE *f = fopen(argv[1],"r");
  if(f==NULL) termina("Errore apertura file");

  // costruzione lista leggendo capitali dal file e inserendo in testa
  capitale *testa=crea_lista_testa(f);
  puts("--- inizio lista (inserimento in testa) ---");
  // stampa lista capitali appena creata
  lista_capitale_stampa(testa,stdout);  
  puts("--- fine lista ---");
  lista_capitale_distruggi(testa);

  // costruzione lista inserendo in coda
  rewind(f); // riavvolge il file
  testa=crea_lista_coda(f);
  puts("--- inizio lista (inserimento in coda) ---");
  // stampa lista capitali appena ri-creata
  lista_capitale_stampa(testa,stdout);  
  puts("--- fine lista ---");

  // test filter_iter
  testa = filter_iter(testa, "Londra"); 
  puts("--- inizio lista (con tutte le occorrenze di Londra cancellate) --");
  lista_capitale_stampa(testa, stdout); 
  puts("--- fine lista ---"); 
  lista_capitale_distruggi(testa); 

  // elimina Londra e Varsavia dalla lista
  rewind(f); 
  testa = crea_lista_coda(f); 
  testa = cancella_nome_iter(testa,"Londra");
  testa = cancella_nome_iter(testa,"Varsavia");
  puts("--- inizio lista (con Londra e Varsavia cancellate) ---");
  lista_capitale_stampa(testa,stdout);  
  puts("--- fine lista ---");
  lista_capitale_distruggi(testa);
  
  // // costruzione lista inserendo per latitudine decrescente
  rewind(f); // riavvolge il file
  testa=crea_lista_lat(f);
  puts("--- inizio lista (inserimento per latitudine) ---");
  // stampa lista capitali appena ri-creata
  lista_capitale_stampa(testa,stdout);  
  puts("--- fine lista ---");

  // costruzione lista inserendo per ordine lessicografico
  rewind(f); 
  testa = crea_lista_ord(capitale_lex_comp, f); 
  puts("--- inizio lista (inserimento lessicografico) ---"); 
  lista_capitale_stampa(testa, stdout); 
  puts("--- fine lista ---"); 
  
  if(fclose(f)==EOF)
    termina("Errore chiusura");
  // dealloca la memoria usata dalla lista 
  lista_capitale_distruggi(testa);
  
  return 0;
}



// stampa su stderr il  messaggio che gli passo
// se errno!=0 stampa anche il messaggio d'errore associato 
// a errno. dopo queste stampe termina il programma
void termina(const char *messaggio)
{
  if(errno==0) 
     fprintf(stderr,"%s\n",messaggio);
  else 
    perror(messaggio);
  exit(1);
}

