#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ALPHA 64

const float loading_factor = 0.75;

struct game{

    char *RefWord;
    int chance;

};
typedef struct game *gamePointer;

struct tagVincoli{

    int count;
    int n_pres;
    bool pres_max;

    bool *posix;

};
typedef struct tagVincoli *vincoliPointer;

unsigned int conteggio_plausibili, NoW;
int l;
char c, *giuste;
bool vincoliON; //true: i vincoli sono stati riempiti, false: i vincoli non sono necessari.
bool gameON;

//specifiche hash table
unsigned int DIM=100000;
char *dict;
bool *valid;

//prototipi funzioni***********************************************************************************************************************************************
void nuova_partita(gamePointer partita, vincoliPointer *vincoli);

void amplify_dict(vincoliPointer *vincoli);
void resize_table();
unsigned int hash(char *word, unsigned int dim);

void inizializing_table_and_vectors_to_nuovaPartita();
void stampa_filtrate();
void special_quickSort(char *vett[], long int lo, long int hi);
unsigned int partitionLomuto(char *vett[], long int lo, long int hi);
void scrematura(vincoliPointer *vincoli);
bool check_plausible(char *word, vincoliPointer *vincoli);

bool classifyWord(char *word1, char *word2);
bool sameWord(char *word1, char *word2);
bool searchWord(char *dict, char *word);
void printword(char *parola);

int find_a_place(char *array, char c );
char from_int_to_char_ascii(int num);
int letters_in_hash(char lettera);
int letters_in_ascii(char lettera);

void free_vincoli(vincoliPointer *geri);


//definizione main*************************************************************************************************************************************************
int main(){

    int i;
    char *wordRead;

    gamePointer match=NULL;
    vincoliPointer vincoli[ALPHA];
    conteggio_plausibili=0;

    vincoliON=false;

    l=0;
    do{
        c=getchar();
        if(c!='\n') l=l*10 + c-48;
    }while(c!='\n');


    dict =(char*)malloc( DIM*l*sizeof( char) );
    memset(dict, '\0', DIM*l*sizeof( char));

    valid=(bool*)malloc(DIM * sizeof(bool) );
    memset(valid, (int)false, DIM*sizeof( bool));

    wordRead=(char*)malloc(sizeof(char)*l);

    giuste=(char*)malloc(sizeof(char)*l);

    match=(struct game*)malloc(sizeof(struct game));
    match->RefWord = (char*)malloc(sizeof(char)*l);

    for(i=0; i<ALPHA; i=i+1){
        vincoli[i]=NULL;
    }

    //from here starts the real structure of the game
    do{	amplify_dict(vincoli); }while(c=='i');


    if(c!='n') {
        printf("\n-dictionary initialization failed");
        return 0;
    }


    nuova_partita(match, vincoli);//inizio a riempire il dizionario


    free(giuste);
    free(wordRead);
    free_vincoli(vincoli);
    free(match->RefWord);
    free(match);
    free(valid);
    free(dict);
//	printf("\n\nRETURN 0\n\n");
    return 0;
}


//definizione funzioni**********************************************************************************************************************************************
void nuova_partita(gamePointer partita, vincoliPointer *vincoli){

    char parola[l],
            outWord[l]; //parola da stampare su Fout

    int lettere_presenti, spot, i, j;

    while(true){

        //*******************************************initializing vincoli *****************************************************************
        gameON=true;
        free_vincoli(vincoli);

        memset(giuste, '\0', l);//---------------------------------------------------------------------------------------------------

        //*******************************************initializing tree to nuova partita *****************************************************************

        conteggio_plausibili=0;
        inizializing_table_and_vectors_to_nuovaPartita();

        i=0;
        while((c=getc(stdin))!='\n' && c!=EOF ){
            partita->RefWord[i]=c;
            i+=1;
        }
        if(c==EOF) return;

        //leggo il numero di chance
        partita->chance=0;
        while((c=getc(stdin))!='\n' && c!=EOF ){
            partita->chance=(partita->chance)*10 + c-48;
        }
        if(c==EOF) return;

        while(gameON){ //un p� brutale-----------------------------------------------------------------------------

            memset(parola, '\0', l);//---------------------------------------------------------------------------------------------------
            memset(outWord, '\0', l);//---------------------------------------------------------------------------------------------------

            //reading the first word

            i=0;
            while((c=getc(stdin))!='\n' && c!='+' && c!=EOF){
                parola[i]=c;
                i+=1;
            }


            if(c==EOF) return;

            if(c=='+'){//let's see wich command I have to do
                c=getc(stdin);
                while(getc(stdin)!='\n');//reding the rest of the line

                //+nuova_partita
                if(c=='n'){
                    gameON=false ;
                }
                //+stampa_filtrate, here I also have to do a filtering
                if(c=='s'){
//				printf("\n_plausible_stampa:%d\n", conteggio_plausibili);
                    stampa_filtrate();
                }
                //+inserisci_inizio
                if(c=='i'){
                    amplify_dict(vincoli);
                    if(c!='i') printf("\n +inserisci failed or something went wrong");
                }
            }
            else {

                if(sameWord(parola, partita->RefWord)) { //la parola letta ? giusta
                    puts("ok");
                    vincoliON=false;
                }
                else{
                    if(searchWord(dict, parola)){

                        partita->chance-=1;
                        vincoliON=true;

                        for(i=0; i<l; i+=1){ //+ a tutte le lettere al posto giusto
                            if(parola[i]==partita->RefWord[i]){
                                outWord[i]='+';
                            }
                        }

                        for(i=0; i<l; i+=1){//per ogni lettera di parola[]
                            //conto tutte le lettere parola[i] in refWord
                            lettere_presenti=0;
                            for(j=0; j<l; j+=1){//conto quante lettere parola[i] sono presenti in (*partita)->RefWord[]
                                if(partita->RefWord[j]==parola[i]){
                                    lettere_presenti+=1;
                                }
                            }
                            if(lettere_presenti==0){
                                //ASSENTI
                                outWord[i]='/';
                            }
                            else{

                                for(j=0; j<l; j+=1){//tolgo le lettere al posto giusto cos? mi rimangono solo quelle presenti al posto sbagliato
                                    if((outWord[j]=='+' &&  parola[i]==partita->RefWord[j])){
                                        lettere_presenti-=1;
                                    }
                                }

                                for(j=0; j<i; j+=1){ //tolgo le lettere che sono gi? state segnate '/' prima di j
                                    if(parola[j]==parola[i] && outWord[j]=='|' ){
                                        lettere_presenti-=1;
                                    }
                                }

                                for(j=0; j<l; j+=1){
                                    if(parola[i]==partita->RefWord[j] && outWord[i]=='\0'){ //al posto di (*partita)->RefWord[j] avevo messo parola[j]
                                        if(lettere_presenti>0){
                                            lettere_presenti-=1;
                                            outWord[i]='|';
                                        }
                                        else{
                                            outWord[i]='/';
                                        }
                                    }
                                }
                            }

                        }

                        printword(outWord); //: %s->outWord , parola+++++++++++++++++++++++++++++++++

                        //aggiorniamo i VINCOLI******************************************************************************************************************************************************

                        for(i=0; i<l; i+=1){
                            //outWord[i] pu? essere di tre tipi: +, /, |

                            if(outWord[i]=='+'){
                                //aggiungo parola[i] a giuste[] e a presenti[]
                                giuste[i]=parola[i];
                                spot=letters_in_ascii(parola[i]);
                                if(vincoli[spot]==NULL){
                                    vincoli[spot]=(struct tagVincoli*)malloc(sizeof(struct tagVincoli));
                                    vincoli[spot]->posix=(bool *)malloc(sizeof(bool)* l);
                                    vincoli[spot]->count=0;
                                    vincoli[spot]->n_pres=0;
                                    vincoli[spot]->pres_max=false;
                                    //inizilizzo il vettore appena creato
                                    for(j=0; j<l; j+=1){
                                        vincoli[spot]->posix[j]=true;
                                    }
                                }
                                vincoli[spot]->count +=1;
                            }
                            if(outWord[i]=='|'){
                                //aggiungo parola[i] a presenti[] e correggo la matrice
                                spot=letters_in_ascii(parola[i]);

                                if(vincoli[spot]==NULL){
                                    vincoli[spot]=(struct tagVincoli*)malloc(sizeof(struct tagVincoli));
                                    vincoli[spot]->posix=(bool *)malloc(sizeof(bool)* l);
                                    vincoli[spot]->count=0;
                                    vincoli[spot]->n_pres=0;
                                    vincoli[spot]->pres_max=false;
                                    //inizilizzo il vettore appena creato
                                    for(j=0; j<l; j+=1){
                                        vincoli[spot]->posix[j]=true;
                                    }
                                }
                                vincoli[spot]->count+=1;
                                vincoli[spot]->posix[i]=false;
                            }
                        }

                        for(i=0; i<l; i+=1){

                            spot=letters_in_ascii(parola[i]);
                            if(vincoli[spot]!=NULL && outWord[i]!='/'){
                                if( vincoli[spot]->count > vincoli[spot]->n_pres && vincoli[spot]->pres_max==false) vincoli[spot]->n_pres=vincoli[spot]->count;// :vincoli[i]->n_pres!=-1 &&
                                vincoli[spot]->count=0;
                            }
                        }

                        for(i=0; i<l; i+=1){
                            if(outWord[i]=='/'){//devo ferificare se in parola[] ci sono altre parola[i]; se � in eccesso sar� sicuramente dentro presenti, altrimenti sar� per forza assente
                                spot=letters_in_ascii(parola[i]);

                                if(vincoli[spot]!=NULL) {
                                    if(vincoli[spot]->n_pres!=-1){
                                        vincoli[spot]->pres_max=true;//se trovo almeno una lettera uguale vuol dire che in refWord non ce ne sono altre
                                        vincoli[spot]->posix[i]=false;
                                    }
                                }
                                else{
                                    vincoli[spot]=(struct tagVincoli*)malloc(sizeof(struct tagVincoli));
                                    vincoli[spot]->count=0;
                                    vincoli[spot]->n_pres=-1; // inizializzandolo a -1 sto dicendo che no esiste, fa parte di ASSENTI
                                    //inizilizzo il vettore appena creato
                                }
                            }
                        }


                        scrematura(vincoli);

                        printf("%u\n", conteggio_plausibili);


                        if( partita->chance <= 0) {
                            puts("ko");
                            vincoliON=false;
                        }
                    }
                    else{
                        puts("not_exists");
                    }
                }
            }
        }
    }
}


unsigned int hash(char *word, unsigned int dim){

    const int p=64;
    unsigned int idx=0;
    int i, pp=1;

    for(i=0; i<l; i++){

        idx=idx + letters_in_hash(word[i])*pp;
        pp=pp*p;
    }


    return idx % dim;

}

void amplify_dict(vincoliPointer *vincoli){

    long int pose;
    int q;
    char wordRead[l];

    do{
        q=0;
        while((c=getc(stdin))!='\n' && c!='+'){
            wordRead[q]=c;
            q++;
        }


        if(c!='+'){

            NoW+=1;
            conteggio_plausibili+=1;


            pose=hash(wordRead, DIM);

            while( dict[pose*l] != '\0' ){
                pose+=1;
                pose=pose%DIM;
            }

            for(q=0; q<l; q+=1) dict[pose*l+q]=wordRead[q]; //sostituibile con memcpy------------------------------------------------------------------

            if(vincoliON){

                if(check_plausible(wordRead, vincoli)){
                    valid[pose]=true;
                }
                else{
                    valid[pose]=false;
                    conteggio_plausibili-=1;
                }
            }
            else {
                valid[pose]=true;
            }


            if((float)NoW/DIM > loading_factor){

                resize_table(&dict, &valid);//devo aggiornare anche hashCells e validCells

            }
        }

    }while(c!='+');

    c=getc(stdin);
    while(getc(stdin)!='\n'); //finisco di leggere +?....

    return;
}

void resize_table(){

    char *newDict;
    bool *newValid;
    unsigned int index=0, t, q;

    unsigned int dim=DIM;
    DIM=2*DIM;


    newDict=(char*)malloc( DIM * l * sizeof(char) );
    memset(newDict, '\0', DIM*l*sizeof( char));

    newValid=(bool*)malloc(DIM * sizeof(bool) );
    memset(newValid, (bool)0, DIM*sizeof( bool));

    //faccio scorrere dict grande [dim*l]
    for(t=0; t<dim*l; t+=l){

        if(dict[t]!='\0') {//se trovo una parola

            index=hash(&dict[t], DIM);

            while( newDict[index*l] != '\0'){
                index+=1;
                index=index%DIM;
            }

            for(q=0; q<l; q+=1) newDict[index*l+q]=dict[t+q];

            if(vincoliON){
                newValid[index]=valid[t/l];
            }
            else{
                newValid[index]=true;
            }
        }
    }

    free(dict);
    free(valid);
    dict=newDict;
    valid=newValid;
    return;
}


void inizializing_table_and_vectors_to_nuovaPartita(){

    long int q;

    for(q=0; q<DIM*l; q+=l){

        if(dict[q]!='\0') {//se trovo un elemento
            valid[q/l]=true;
            conteggio_plausibili+=1;
        }
        else valid[q/l]=false;
    }
}


void stampa_filtrate(){//questa funzione consiste nel creare un array grande "conteggio_plausibili", riempirlo, ordinarlo e stamparlo

    char *vett[conteggio_plausibili];//creo
    long int q, v_c=0;

    for(q=0; q<DIM; q+=1){ //riempio

        if(valid[q]){
            vett[v_c]=&dict[q*l];
            v_c+=1;
        }
    }


    special_quickSort(vett, 0, conteggio_plausibili-1); //ordino

    for(q=0; q<conteggio_plausibili; q+=1) printword(vett[q]); //stampo

    return;
}

void special_quickSort(char *vett[], long int lo, long int hi) {

    if(lo<hi){

        long int piv=partitionLomuto(vett, lo, hi);

        special_quickSort(vett, lo, piv-1);
        special_quickSort(vett, piv+1, hi);

    }
}

unsigned int partitionLomuto(char *vett[], long int lo, long int hi){

    char *pivot, *temp;
    long int i=lo-1, j;

    pivot=vett[hi];

    for(j=lo; j<hi; j+=1){

        if(classifyWord(vett[j], pivot)){
            i+=1;
            temp=vett[j];
            vett[j]=vett[i];
            vett[i]=temp;
        }
    }

    temp=vett[hi];
    vett[hi]=vett[i+1];
    vett[i+1]=temp;

    return i+1;
}

void scrematura(vincoliPointer *vincoli){// potrei togliere *vincoli e mettere vincoli

    long int k;
    int r, q, n, p;
    char lt, *word;

    bool pass;

    for(k=0; k<DIM; k+=1){

        if(valid[k]){

            pass=true;
            word=&dict[k*l];

            r=0;
            while( r<l && pass){//faccio scorrere giuste[]
                if(giuste[r]!='\0' && giuste[r]!=word[r]) pass=false;//non c'� la lettera giusta in quel punto
                r+=1;
            }

            if(pass){

                r=0;
                while( r<ALPHA && pass){//faccio scorrere vincoli-lettere[]

                    if(vincoli[r]!=NULL){

                        lt=from_int_to_char_ascii(r);
                        q=find_a_place(word, lt);

                        if( vincoli[r]->n_pres==-1 && q!=l) pass=false; //assente

                        if( vincoli[r]->n_pres!=-1 && pass){
                            //arrivato a questo punto la lettera[r] deve per forza essere presente, devo verificare quante volte e se rispetta i parametri
                            if(q==l) pass=false; //significa che la lettera vincoli->lettere[r] non � presente nella parola quando invece dovrebbe esserlo, deve esserci ma non la trovo

                            //conto quante volte compare la lettera
                            n=0;
                            p=0;
                            while( p<l && pass){
                                if(word[p]==lt){
                                    //verifico la "matrice"
                                    if(vincoli[r]->posix[p]==false) pass=false; //so che in quel punto non deve esseri quella lettera quindi se c'� non va mica bene
                                    n+=1;
                                }
                                p+=1;
                            }
                            if(( n<vincoli[r]->n_pres || (n>vincoli[r]->n_pres && vincoli[r]->pres_max==true) ) && pass) pass=false;
                        }
                    }
                    r+=1;
                }
            }

            if(pass==false ){
                conteggio_plausibili-=1;
                valid[k]=false;
            }
        }
    }
}

bool check_plausible(char *word, vincoliPointer *vincoli){

    int r, q, n, p;
    char lt;


    for(r=0; r<l; r+=1){//faccio scorrere giuste[]
        if(giuste[r]!='\0' && giuste[r]!=word[r]) return false;//non c'� la lettera giusta in quel punto
    }

    for(r=0; r<ALPHA; r+=1){//faccio scorrere vincoli-lettere[]

        if(vincoli[r]!=NULL){

            lt=from_int_to_char_ascii(r);
            q=find_a_place(word, lt);

            if( vincoli[r]->n_pres==-1 && q!=l) return false; //assente

            if( vincoli[r]->n_pres!=-1){
                //arrivato a questo punto la lettera[r] deve per forza essere presente, devo verificare quante volte e se rispetta i parametri
                if(q==l) return false; //significa che la lettera vincoli->lettere[r] non � presente nella parola quando invece dovrebbe esserlo, deve esserci ma non la trovo

                //conto quante volte compare la lettera
                n=0;
                for(p=0; p<l; p+=1){
                    if(word[p]==lt){
                        //verifico la "matrice"
                        if(vincoli[r]->posix[p]==false) return false; //so che in quel punto non deve esseri quella lettera quindi se c'� non va mica bene
                        n+=1;
                    }
                }
                if( n<vincoli[r]->n_pres || (n>vincoli[r]->n_pres && vincoli[r]->pres_max==true)) return false;
            }
        }
    }

    return true;

}



bool classifyWord(char *word1, char *word2){ //chack if word1 is before word2 in alphabetical order

    int count=0;

    while(word1[count]==word2[count]){
        count+=1;
        if(count>=l) return false;
    }
    if(word1[count]<word2[count]) return true;
    else return false;

}

bool sameWord(char *word1, char *word2){

    int k;
    for(k=0; k<l; k++){
        if(word1[k]!=word2[k]) return false;
    }
    return true;
}

bool searchWord(char* table, char *word){

    unsigned int idx=hash(word, DIM);

    while(table[idx*l]!='\0'){
        if(sameWord(&table[idx*l], word)) return true;
        idx+=1;
        idx=idx%DIM;
    }

    return false;


}

void printword(char *parola){

    int t;

    for(t=0; t<l; t+=1){
        putchar(parola[t]);
    }
    putchar('\n');
}



int find_a_place(char *array, char c ){

    int k=0;
    for(k=0; k<l; k+=1){
        if(array[k]==c || array[k]=='\0') return k;
    }
    return k;
}

char from_int_to_char(int num){

    if(num==1) return '-';

    if(num==38) return '_';

    if(num>=2 && num<=11) return num+46;

    if(num>=12 && num<=37) return num+53;

    if(num>=39 && num<=64) return num+58;

    return '+';
}

char from_int_to_char_ascii(int num){

    if(num==0) return '-';

    if(num==37) return '_';

    if(num>=1 && num<=10) return num+47;

    if(num>=11 && num<=36) return num+54;

    if(num>=38 && num<=63) return num+59;

    return '+';
}

int letters_in_hash(char lettera){

    if(lettera=='-')  return 1;

    if(lettera=='_')  return 38;

    if(lettera>='0' && lettera<='9') return lettera-46;

    if(lettera>='A' && lettera<='Z') return lettera-53;

    if(lettera>='a' && lettera<='z') return lettera-58;

    return -1;
}

int letters_in_ascii(char lettera){

    if(lettera=='-')  return 0;

    if(lettera=='_')  return 37;

    if(lettera>='0' && lettera<='9') return lettera-47;

    if(lettera>='A' && lettera<='Z') return lettera-54;

    if(lettera>='a' && lettera<='z') return lettera-59;

    return -1;
}



void free_vincoli(vincoliPointer *geri){

    int barenghi;

    for(barenghi=0; barenghi<ALPHA; barenghi+=1){
        if(geri[barenghi]!=NULL ){

            if(geri[barenghi]->n_pres!=-1) free(geri[barenghi]->posix);

            free(geri[barenghi]);
            geri[barenghi]=NULL;
        }
    }

}




