#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>
#include<stdint.h>
#include<math.h>

//Idee per migliorare ogni attributo x e y ridurlo a 1 solo

//Descrive la generica stuttura della destinazione una rotta aerea c
typedef struct rotta {
    unsigned int x;
    unsigned int y;
    uint8_t costo;
    struct rotta * next;
}* Rotta;

//Costo e rotte partenti da quel nodo
typedef struct nodo {
    uint8_t costo;
    Rotta dest;
} * Nodo;

//Lista di tutti i voli creati per permettere una facile deallocazione
typedef struct voli {
    unsigned int x;
    unsigned int y;
    struct voli * next;
} *Voli;

typedef struct travel {
    unsigned int xyp;
    unsigned int xyd;
    unsigned int costo;
    struct travel *next;
} * Travel;


bool init(Nodo *mappa, unsigned int* x, unsigned int* y, Voli* part, bool ** visitati, Travel *lista);
bool toggle_air_route(Nodo *mappa, unsigned int x, unsigned int y, Voli* part, bool ** visitati, Travel * lista);
unsigned int NewCost(unsigned int a, int v, unsigned int r, unsigned int d);
bool change_cost(Nodo *mappa, unsigned int x, unsigned int y, bool** visitati, Travel *lista);
void ModificaHeap (unsigned int heap[], unsigned int costo[], unsigned int xyt, unsigned int * pos, unsigned int n);
int travel_cost(Nodo mappa, unsigned int x, unsigned int y, Travel *lista, bool **visitati);

int main() {
    char comando[17];
    unsigned int x=0, y=0;
    bool risp=1;
    Nodo mappa=NULL; //mappa in cui inseriremo i costi di ciascun esagono e le rotte aeree partenti
    Voli part=NULL;
    Travel lista=NULL;
    bool * visitati=NULL;
    //Lettura comando
    while (scanf("%s", comando) != EOF) {
        if(!strcmp(comando, "init"))
            risp = init(&mappa, &x, &y, &part, &visitati, &lista);

        if(!strcmp(comando, "toggle_air_route"))
            risp = toggle_air_route(&mappa, x, y, &part, &visitati, &lista);

        if(!strcmp(comando, "change_cost"))
            risp = change_cost(&mappa, x, y, &visitati, &lista);

        if(!strcmp(comando, "travel_cost"))
            printf("%i\n", travel_cost(mappa, x, y, &lista, &visitati));

        else {
            if(risp==0)
                printf("OK\n");
            else
                printf("KO\n");
        }
    }
    //Deallocazione
    while (part!=NULL){
        while (mappa[part->x*y+part->y].dest!=NULL) {
            Rotta temp = mappa[part->x*y+part->y].dest;
            mappa[part->x*y+part->y].dest = temp->next;
            free(temp);
        }
        Voli moment = part;
        part = part->next;
        free(moment);
    }
    if (mappa!=NULL)
        free(mappa);
    if (visitati!=NULL)
        free(visitati);
    visitati = NULL;
    while (lista!=NULL) {
        Travel temp = lista;
        lista=lista->next;
        free(temp);
    }
    return 0;
 }

bool init(Nodo *mappa, unsigned int* x, unsigned int* y, Voli *part, bool ** visitati, Travel * lista) {
    unsigned int i, j;

    //Deallocazione
    while ((*part)!=NULL){
        while ((*mappa)[(*part)->x*(*y)+(*part)->y].dest!=NULL) {
            Rotta temp = (*mappa)[(*part)->x*(*y)+(*part)->y].dest;
            (*mappa)[(*part)->x*(*y)+(*part)->y].dest = temp->next;
            free(temp);
        }
        Voli moment = (*part);
        (*part) = (*part)->next;
        free(moment);
    }
    if ((*mappa)!=NULL)
        free(*mappa);
    (*mappa) = NULL;
    if ((*visitati)!=NULL)
        free(*visitati);
    (*visitati) = NULL;
    while ((*lista)!=NULL) {
        Travel temp = (*lista);
        (*lista)=(*lista)->next;
        free(temp);
    }

    //Lettura dimensioni
    if (scanf("%i %i", x, y)!=2)
        return 1;

    //Allocazione
    (*mappa) = malloc((*x)*(*y)*sizeof(struct nodo));
    (*visitati) = malloc((*x)*(*y)*sizeof(bool));
    if ((*mappa) == NULL || (*visitati) == NULL)
        return 1;

    //Inizializzazione
    for (i = 0; i < (*x); i++) {
        for (j = 0; j < (*y); j++) {
            (*mappa)[i*(*y)+j].costo=1;
            (*mappa)[i*(*y)+j].dest=NULL;
            (*visitati)[i*(*y)+j]=0;
        }
    }

    return 0;
}

bool toggle_air_route(Nodo* mappa, unsigned int x, unsigned int y, Voli *part, bool ** visitati, Travel * lista){
    //Lettura coordinate e controllo di validità
    unsigned int x1, x2, y1, y2;
    if (scanf("%i %i %i %i", &x1, &y1, &x2, &y2)!=4)
        return 1;
    if(x==0 || y==0 || (*mappa)==NULL)
        return 1;
    if (x1>=x || y1>=y || x2>=x || y2>=y)
        return 1;

    //Controllo se nel nodo non esistono rotte aeree
    if ((*mappa)[x1*y+y1].dest==NULL) {
        (*mappa)[x1*y+y1].dest=(Rotta) malloc(sizeof(struct rotta));
        if ((*mappa)[x1*y+y1].dest == NULL)
            return 1;
        (*mappa)[x1*y+y1].dest->x=x2;
        (*mappa)[x1*y+y1].dest->y=y2;
        (*mappa)[x1*y+y1].dest->next=NULL;
        (*mappa)[x1*y+y1].dest->costo=(*mappa)[x1*y+y1].costo;

        //Deallocazione visitati
            for (int k=0; k<x*y; k++)
                (*visitati)[k]=0;
            while ((*lista)!=NULL) {
                Travel temp = (*lista);
                (*lista)=(*lista)->next;
                free(temp);
            }

        //Segnaliamo che abbiamo costruito una rotta aerea partente da x1 y1
        if((*part)==NULL) {
            (*part) = (Voli) malloc(sizeof(struct voli));
            if ((*part) == NULL)
                return 1;
            (*part)->x=x1;
            (*part)->y=y1;
            (*part)->next=NULL;
            return 0;
        }
        Voli moment = (*part);
        while (moment->next!=NULL)
            moment=moment->next;
        moment->next = (Voli) malloc(sizeof(struct voli));
        if (moment->next == NULL)
            return 1;
        moment=moment->next;
        moment->x=x1;
        moment->y=y1;
        moment->next=NULL;
        return 0;
    }

    //Esistono rotte aeree, controlliamo se la prima è da rimuovere
    Rotta temp = (*mappa)[x1*y+y1].dest;
    if ((*mappa)[x1*y+y1].dest->x==x2 && (*mappa)[x1*y+y1].dest->y==y2) {
        (*mappa)[x1*y+y1].dest=temp->next;
        free(temp);
        //Deallocazione visitati
        if ((*visitati)[x1*y+y1]==1) {
            for (int k=0; k<x*y; k++)
                (*visitati)[k]=0;
            while ((*lista)!=NULL) {
                Travel temp = (*lista);
                (*lista)=(*lista)->next;
                free(temp);
            }
        }

        //Se non ci sono altre rotte segnaliamo
        if ((*mappa)[x1*y+y1].dest==NULL) {
            Voli moment = (*part);
            if((*part)->x==x1 && (*part)->y==y1) {
                (*part)=(*part)->next;
                free(moment);
                return 0;
            }
            while (!(moment->next->x==x1 && moment->next->y==y1))
                moment=moment->next;
            Voli moment2 = moment->next;
            moment->next = moment2->next;
            free(moment2);
        }
        return 0;
    }

    //Esistono rotte aeree e non dobbiamo rimuovere la prima, controlliamo se ce ne sono altre uguali o se possiamo inserire
    int i=2;
    while (temp->next!=NULL && !(temp->next->x==x2 && temp->next->y==y2)) {
        temp=temp->next;
        i++;
    }

    //Interrompiamo se sono già presenti 5 rotte
    if (i>5)
        return 1;

    //Aggiungiamo la rotta se non è presente
    if (temp->next==NULL) {
        temp->next = (Rotta) malloc(sizeof(struct rotta));
        if (temp->next == NULL)
            return 1;
        temp=temp->next;
        temp->x=x2;
        temp->y=y2;
        temp->next=NULL;
        temp->costo=(*mappa)[x1*y+y1].costo;
        //Deallocazione visitati
            for (int k=0; k<x*y; k++)
                (*visitati)[k]=0;
            while ((*lista)!=NULL) {
                Travel temp = (*lista);
                (*lista)=(*lista)->next;
                free(temp);
            }
        return 0;
    }

    //Ultimo caso rimuoviamo
    Rotta temp2 = temp->next;
    temp->next = temp2->next;
    free(temp2);
    //Deallocazione visitati
    if ((*visitati)[x1*y+y1]==1) {
        for (int k=0; k<x*y; k++)
            (*visitati)[k]=0;
        while ((*lista)!=NULL) {
            Travel temp = (*lista);
            (*lista)=(*lista)->next;
            free(temp);
        }
    }
    return 0;
}

unsigned int NewCost(unsigned int a, int v, unsigned int r, unsigned int d) {
    int cambio=floor(((float)v)*(float)(r-d)/((float)r));
    int newcost = (int)(a) + cambio;
    if (newcost<0)
        return 0;
    if (newcost>100)
        return 100;
    return (unsigned int) newcost;
}

bool change_cost(Nodo* mappa, unsigned int x, unsigned int y, bool ** visitati, Travel * lista) {
    unsigned int xc, yc, r;
    int v;
    //Lettura dimensioni
    if (scanf("%i %i %i %i", &xc, &yc, &v, &r)!=4)
        return 1;
    if(x==0 || y==0 || (*mappa)==NULL)
        return 1;
    if(r==0 || xc>=x || yc>=y || v<-10 || v>10)
        return 1;
    int i, j;

    //Partiamo da dist=0 per arrivare a dist=r
    for (i=0; i<r; i++) {
        //Cambiamo il primo
        if(i==0) {
            (*mappa)[xc*y+yc].costo = NewCost((*mappa)[xc*y+yc].costo, v, r, i);
            Rotta temp=(*mappa)[xc*y+yc].dest;
            while(temp!=NULL) {
                temp->costo = (*mappa)[xc*y+yc].costo;
                temp=temp->next;
            }
            //Deallocazione visitati
            if ((*visitati)[xc*y+yc]==1) {
                for (int k=0; k<x*y; k++)
                    (*visitati)[k]=0;
                while ((*lista)!=NULL) {
                    Travel temp = (*lista);
                    (*lista)=(*lista)->next;
                    free(temp);
                }
            }
        }

        //Cambio vicini
        else {
            //Cambio sx
            unsigned int xt=xc-i;
            unsigned int yt=yc;
            if (xt<x && yt<y) {
                (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                Rotta temp=(*mappa)[xt*y+yt].dest;
                while(temp!=NULL) {
                    temp->costo = NewCost(temp->costo, v, r, i);
                    temp=temp->next;
                }
                //Deallocazione visitati
                if ((*visitati)[xt*y+yt]==1) {
                    for (int k=0; k<x*y; k++)
                        (*visitati)[k]=0;
                    while ((*lista)!=NULL) {
                        Travel temp = (*lista);
                        (*lista)=(*lista)->next;
                        free(temp);
                    }
                }
            }
            //Discesa a dx
            for (j=0; j<i; j++) {
                if (yt%2!=0)
                    xt++;
                yt--;
                if (xt<x && yt<y) {
                    (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                    Rotta temp=(*mappa)[xt*y+yt].dest;
                    while(temp!=NULL) {
                        temp->costo = NewCost(temp->costo, v, r, i);
                        temp=temp->next;
                    }
                    //Deallocazione visitati
                    if ((*visitati)[xt*y+yt]==1) {
                        for (int k=0; k<x*y; k++)
                            (*visitati)[k]=0;
                        while ((*lista)!=NULL) {
                            Travel temp = (*lista);
                            (*lista)=(*lista)->next;
                            free(temp);
                        }
                    }
                }
            }
            //Costante a dx
            for (j=0; j<i; j++) {
                xt++;
                if (xt<x && yt<y) {
                    (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                    Rotta temp=(*mappa)[xt*y+yt].dest;
                    while(temp!=NULL) {
                        temp->costo = NewCost(temp->costo, v, r, i);
                        temp=temp->next;
                    }
                    //Deallocazione visitati
                    if ((*visitati)[xt*y+yt]==1) {
                        for (int k=0; k<x*y; k++)
                            (*visitati)[k]=0;
                        while ((*lista)!=NULL) {
                            Travel temp = (*lista);
                            (*lista)=(*lista)->next;
                            free(temp);
                        }
                    }
                }
            }
            //Salita a dx
            for (j=0; j<i; j++) {
                if (yt%2!=0)
                    xt++;
                yt++;
                if (xt<x && yt<y) {
                    (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                    Rotta temp=(*mappa)[xt*y+yt].dest;
                    while(temp!=NULL) {
                        temp->costo = NewCost(temp->costo, v, r, i);
                        temp=temp->next;
                    }
                    //Deallocazione visitati
                    if ((*visitati)[xt*y+yt]==1) {
                        for (int k=0; k<x*y; k++)
                            (*visitati)[k]=0;
                        while ((*lista)!=NULL) {
                            Travel temp = (*lista);
                            (*lista)=(*lista)->next;
                            free(temp);
                        }
                    }
                }
            }
            //Salita a sx
            for (j=0; j<i; j++) {
                if (yt%2==0)
                    xt--;
                yt++;
                if (xt<x && yt<y) {
                    (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                    Rotta temp=(*mappa)[xt*y+yt].dest;
                    while(temp!=NULL) {
                        temp->costo = NewCost(temp->costo, v, r, i);
                        temp=temp->next;
                    }
                    //Deallocazione visitati
                    if ((*visitati)[xt*y+yt]==1) {
                        for (int k=0; k<x*y; k++)
                            (*visitati)[k]=0;
                        while ((*lista)!=NULL) {
                            Travel temp = (*lista);
                            (*lista)=(*lista)->next;
                            free(temp);
                        }
                    }
                }
            }
            //Costante a sx
            for (j=0; j<i; j++) {
                xt--;
                if (xt<x && yt<y) {
                    (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                    Rotta temp=(*mappa)[xt*y+yt].dest;
                    while(temp!=NULL) {
                        temp->costo = NewCost(temp->costo, v, r, i);
                        temp=temp->next;
                    }
                    //Deallocazione visitati
                    if ((*visitati)[xt*y+yt]==1) {
                        for (int k=0; k<x*y; k++)
                            (*visitati)[k]=0;
                        while ((*lista)!=NULL) {
                            Travel temp = (*lista);
                            (*lista)=(*lista)->next;
                            free(temp);
                        }
                    }
                }
            }
            //Discesa a sx
            for (j=0; j<i-1; j++) {
                if (yt%2==0)
                    xt--;
                yt--;
                if (xt<x && yt<y) {
                    (*mappa)[xt*y+yt].costo = NewCost((*mappa)[xt*y+yt].costo, v, r, i);
                    Rotta temp=(*mappa)[xt*y+yt].dest;
                    while(temp!=NULL) {
                        temp->costo = NewCost(temp->costo, v, r, i);
                        temp=temp->next;
                    }
                    //Deallocazione visitati
                    if ((*visitati)[xt*y+yt]==1) {
                        for (int k=0; k<x*y; k++)
                            (*visitati)[k]=0;
                        while ((*lista)!=NULL) {
                            Travel temp = (*lista);
                            (*lista)=(*lista)->next;
                            free(temp);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void ModificaHeap (unsigned int* heap, unsigned int* costo, unsigned int xyt, unsigned int *pos, unsigned int n) {
    //Ricerca del valore
    unsigned int i, temp;;
    i=pos[xyt];
    if (i>n) {
        temp=heap[i];
        heap[i]=heap[n];
        heap[n]=temp;
        pos[heap[i]]=i;
        pos[heap[n]]=n;
        i=n;
    }
    //Ribilanciamento
    while (costo[heap[i]]<costo[heap[i/2]]){
        temp=heap[i];
        heap[i]=heap[i/2];
        heap[i/2]=temp;
        pos[heap[i]]=i;
        pos[heap[i/2]]=i/2;
        i=i/2;
    }
}

int travel_cost(Nodo mappa, unsigned int x, unsigned int y, Travel *lista, bool ** visitati){
    //Lettura coordinate e controllo di validità
    unsigned int xp, xd, yp, yd, i, xmin, ymin, xt, yt, costot, j, temp1, xyt, xyp, xyd;
    if (scanf("%i %i %i %i", &xp, &yp, &xd, &yd)!=4)
        return -1;
    if(x==0 || y==0 || mappa==NULL)
        return -1;
    if(xp>=x || xd>=x || yp>=y || yd>=y)
        return -1;
    if (xp==xd && yp==yd)
        return 0;
    xyp=xp*y+yp;
    xyd=xd*y+yd;
    if(mappa[xyp].costo==0)
        return -1;

    //Vediamo se c'è gia stata
    Travel temp2 = (*lista);
    if (temp2!=NULL) {
        if(xyp==temp2->xyp && xyd==temp2->xyd)
            return temp2->costo;
        while (temp2->next!=NULL) {
            if(xyp==temp2->next->xyp && xyd==temp2->next->xyd)
                return temp2->next->costo;
            temp2=temp2->next;
        }
    }

    unsigned int xy=x*y;
    //Calcolo effettivo se destinazione e partenza diverse e valide
    //Se non conosci la dimensione alloca dinamicamente, non usare lo stack
    unsigned int* heap = malloc(sizeof(unsigned int)*(xy+1)); //Qua ci stanno solo le coordinate organizzate in un heap in base al costo di arrivo memorizzato per ciascuna coordinata nel vettore sotto, lo 0 non è utilizzato per favorire la ricerca dei figli, se =x*y no valore
    unsigned int* costo = malloc(sizeof(unsigned int)*xy); //Per cercare velocemente il costo per arrivare a quella dest, se =0 fuori dalla coda
    unsigned int* pos = malloc(sizeof(unsigned int)*xy); //Per tenere traccia della posizione di un certo indirizzo nell'heap, se =0 fuori dalla coda
    unsigned int* prec = malloc(sizeof(unsigned int)*xy); //Per tenere traccia del tragitto
    unsigned int n=1; //Serve per tenere conto quanti valori diversi da INT_MAX ci sono in modo tale da skipparli più velocemente
    unsigned int u=xy+1; //Serve per tenere conto del primo posto inutilizzabile
    unsigned int a=0; //Serve per contare quanti esagoni con costo di uscita 0 diversi dalla destinazione posso rimuovere in fase di Inizializzazione
    if(heap == NULL || costo==NULL || pos==NULL || prec==NULL)
        return -1;
    //Inizializzazione dei vettori
    heap[0]=0;
    for (i=0; i<xy; i++){
        if (i==xyd || mappa[i].costo!=0) {
            if (i<xyp){
                heap[i+2-a]=i;
                costo[i]=INT_MAX;
                pos[i]=i+2-a;
            }
            if (i==xyp) {
                costo[i]=0;
                pos[i]=1;
                heap[1]=i;
            }
            if (i>xyp){
                heap[i+1-a]=i;
                costo[i]=INT_MAX;
                pos[i]=i+1-a;
            }
        }
        else {
            a++;
            costo[i]=0;
        }
    }
    u-=a;
    //partenza è il 1° minimo
    xmin=xp;
    ymin=yp;
    unsigned int xymin=xmin*y+ymin;
    //Inizia l'iterazione: si controllano i vicini, le rotte aeree e poi si estrae il minimo (se è la destinazione finisce il ciclo)
    do {
        //Calcolo gia il costo e lo metto come condizione
        costot=costo[xymin]+mappa[xymin].costo;
        if(costot<costo[xyd]) {

            //Se  ci troviamo in posizioni limite  controlli
            if(xmin==0 || xmin==x-1 || ymin==0 || ymin==y-1) {
                //vicino sx
                xt=xmin-1;
                yt=ymin;
                xyt=xt*y+yt;
                //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                if(xt<x && costo[xyt]!=0 && costot<costo[xyt]) {
                    prec[xyt]=xymin;
                    if(costo[xyt]==INT_MAX)
                        n++;
                    costo[xyt]=costot;
                    ModificaHeap(heap, costo, xyt, pos, n);
                }

                //vicino dx
                xt=xmin+1;
                xyt=xt*y+yt;
                //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0)
                if(xt<x && costo[xyt]!=0 && costot<costo[xyt]) {
                    prec[xyt]=xymin;
                    if(costo[xyt]==INT_MAX)
                        n++;
                    costo[xyt]=costot;
                    ModificaHeap(heap, costo, xyt, pos, n);
                }

                //ymin pari
                if(ymin%2==0) {
                    //vicino basso dx
                    xt=xmin;
                    yt=ymin-1;
                    xyt=xt*y+yt;
                    if(yt<y) {
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }

                        //vicino basso sx
                        xt=xmin-1;
                        xyt=xt*y+yt;
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(xt<x && costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }
                    }

                    //vicino alto sx;
                    xt=xmin;
                    yt=ymin+1;
                    xyt=xt*y+yt;
                    if(yt<y) {
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(yt<y && costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }

                        //vicino alto dx
                        xt=xmin-1;
                        xyt=xt*y+yt;
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(xt<x && costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }
                    }
                }

                //ymin dispari
                else {
                    //vicino basso sx
                    xt=xmin;
                    yt=ymin-1;
                    xyt=xt*y+yt;
                    if(yt<y) {
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }

                        //vicino basso dx
                        xt=xmin+1;
                        xyt=xt*y+yt;
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(xt<x && costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }
                    }
                    //vicino alto dx;
                    xt=xmin;
                    yt=ymin+1;
                    xyt=xt*y+yt;
                    if(yt<y) {
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }

                        //vicino alto sx
                        xt=xmin+1;
                        xyt=xt*y+yt;
                        //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                        if(xt<x && costo[xyt]!=0 && costot<costo[xyt]) {
                            prec[xyt]=xymin;
                            if(costo[xyt]==INT_MAX)
                                n++;
                            costo[xyt]=costot;
                            ModificaHeap(heap, costo, xyt, pos, n);
                        }
                    }
                }
            }

            //Se non ci troviamo al limite nessun controllo
            else {
            //vicino sx
                xt=xmin-1;
                yt=ymin;
                xyt=xt*y+yt;
                //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                if(costo[xyt]!=0 && costot<costo[xyt]) {
                    prec[xyt]=xymin;
                    if(costo[xyt]==INT_MAX)
                        n++;
                    costo[xyt]=costot;
                    ModificaHeap(heap, costo, xyt, pos, n);
                }

                //vicino dx
                xt=xmin+1;
                xyt=xt*y+yt;
                //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                if(costo[xyt]!=0 && costot<costo[xyt]) {
                    prec[xyt]=xymin;
                    if(costo[xyt]==INT_MAX)
                        n++;
                    costo[xyt]=costot;
                    ModificaHeap(heap, costo, xyt, pos, n);
                }

                //ymin pari
                if(ymin%2==0) {
                    //vicino basso dx
                    xt=xmin;
                    yt=ymin-1;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }

                    //vicino basso sx
                    xt=xmin-1;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }

                    //vicino alto sx;
                    yt=ymin+1;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }

                    //vicino alto dx
                    xt=xmin;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }
                }

                //ymin dispari
                else {
                    //vicino basso sx
                    xt=xmin;
                    yt=ymin-1;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }

                    //vicino basso dx
                    xt=xmin+1;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }

                    //vicino alto dx;
                    yt=ymin+1;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }

                    //vicino alto sx
                    xt=xmin;
                    xyt=xt*y+yt;
                    //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                    if(costo[xyt]!=0 && costot<costo[xyt]) {
                        prec[xyt]=xymin;
                        if(costo[xyt]==INT_MAX)
                            n++;
                        costo[xyt]=costot;
                        ModificaHeap(heap, costo, xyt, pos, n);
                    }
                }
            }

            //rotte aeree
            Rotta temp=mappa[xmin*y+ymin].dest;
            while(temp!=NULL) {
                xyt=(temp->x)*y+(temp->y);
                //Condizioni: 1 deve essere nella mappa, 2 deve essere nell'heap (quindi costo per arrivarci !=0), 3 il nuovo costo < del precedente
                if(costo[xyt]!=0 && costot<costo[xyt]) {
                    prec[xyt]=xymin;
                    if(costo[xyt]==INT_MAX)
                        n++;
                    costo[xyt]=costot;
                    ModificaHeap(heap, costo, xyt, pos, n);
                }
                temp=temp->next;
            }
        }

        //Rimuovo il nodo appena analizzato
        costo[xymin]=0;

        //Se n=0 non esistono vicini e finisce il ciclo
        if(n!=0) {
            //Faccio scalare in modo tale da avere heap con il 1 nodo da ribilanciare, intmax e nodi inutilizzabili non mischiati
            u--;
            heap[1]=heap[n];
            pos[heap[1]]=1;
            if (u>n) {
                heap[n]=heap[u];
                pos[heap[n]]=n;
            }
            n--;
            j=1;

            //Esci dal ciclo se il nodo è più piccolo o uguale a due figli o se i nodi dei figli sono inutilizzabili (se sono dentro al ciclo cambio per forza)
            while((j*2+1<u) && (costo[heap[j]]>costo[heap[j*2]] || costo[heap[j]]>costo[heap[j*2+1]])) {
                //se primo figlio < del secondo allora scambio con il primo figlio
                if(costo[heap[2*j]]<costo[heap[2*j+1]]){
                    temp1=heap[2*j];
                    heap[2*j]=heap[j];
                    heap[j]=temp1;
                    pos[heap[j]]=j;
                    pos[heap[2*j]]=2*j;
                    j=j*2;
                }
                //se non è il primo è il secondo
                else {
                    temp1=heap[2*j+1];
                    heap[2*j+1]=heap[j];
                    heap[j]=temp1;
                    pos[heap[j]]=j;
                    pos[heap[2*j+1]]=2*j+1;
                    j=j*2+1;
                }
            }
            //confronto nel caso limite in cui esiste un solo figlio
            if (j*2<u && costo[heap[j]]>costo[heap[j*2]]) {
                temp1=heap[2*j];
                heap[2*j]=heap[j];
                heap[j]=temp1;
                pos[heap[j]]=j;
                pos[heap[2*j]]=2*j;
            }
            //Nuovo minimo
            xmin=heap[1]/y;
            ymin=heap[1]%y;
            xymin=xmin*y+ymin;
        }
    } while ((xymin!=xyd) && n!=0);
    if (n==0) {
        free(heap);
        free(costo);
        free(pos);
        free(prec);
        return -1;
    }
    i=costo[xyd];
    if((*lista)==NULL) {
        (*lista) = malloc(sizeof(struct travel));
        (*lista)->xyp=xyp;
        (*lista)->xyd=xyd;
        (*lista)->costo=i;
        (*lista)->next=NULL;
    }
    else {
        temp2->next=malloc(sizeof(struct travel));
        temp2=temp2->next;
        temp2->xyp=xyp;
        temp2->xyd=xyd;
        temp2->costo=i;
        temp2->next=NULL;
    }
    xyt=prec[xyd];
    while(xyt!=xyp){
        (*visitati)[xyt]=1;
        xyt=prec[xyt];
    }
    (*visitati)[xyp]=1;
    free(heap);
    free(costo);
    free(pos);
    free(prec);
    return i;
}
