#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<stdarg.h>
#include<stdbool.h>

struct monome{
	double coef;
	int degree;
	struct monome* next;
};
struct polynome {
	struct monome* maxMonome;
	int isLaplace;
};

typedef struct monome monome;
typedef struct polynome polynome;

/* fonction appelant malloc pour créer un monome donné ou polynôme vide. On ne servira que de celles là ensuite. */

monome* creerMonome(double c, int d){
	monome*m=malloc(sizeof(monome));
	m->coef=c;
	m->degree=d;
	m->next=NULL;
	return m;
}



polynome* creerPolynomeVide(){
	polynome*p=malloc(sizeof(polynome));
	p->maxMonome=NULL;
	p->isLaplace=0;
	return p;
}	

/* calcule la valeur du monome ou du polynome en x */

double evaluerMonome(monome*m, double x){
	return (m->coef)*pow(x,m->degree);
}

double evaluerPolynome(polynome*p ,double x){
	double s=0;
	monome*tmp=p->maxMonome;
	while(tmp!=NULL){
		s+=evaluerMonome(tmp,x);
		tmp=tmp->next;
	}
	return s;
}

/* affiche sous forme lisible (ex: 3x^3+4x^2+5) */


bool retourchariot=false;

void afficherMonome(monome*m){
	if(m->degree!=0){
		printf("%fX^(%i)",m->coef,m->degree);
	}
	else{
		if(m->coef!=0.){
			printf("%f",m->coef);
		}
		else{
			printf("0");
		}
	}
	if(retourchariot){
		printf("\n");
	}
}

void afficherMonomeL(monome*m,int pos){
	if(pos==1){
		printf("%f",m->coef);
	}
	else if(pos==0){
		printf("_________");
	}
	else{
		printf(" p^(%3i) ",m->degree);
	}	
}

void afficherPolynome(polynome* p){
	monome*tmp=p->maxMonome;
	if(p->isLaplace==0){
		if(tmp!=NULL){
			afficherMonome(tmp);
			tmp=tmp->next;
		}
		else{
			printf("vide");
		}
		while(tmp!=NULL){
			printf("+");
			afficherMonome(tmp);
			tmp=tmp->next;
		}
		printf("\n");
	}
	else{
		if(tmp==NULL){
			printf("vide");
		}
		else{
			afficherMonomeL(tmp,1);		//premier ligne
			tmp=tmp->next;
			while(tmp!=NULL){
				printf("   ");
				afficherMonomeL(tmp,1);
				tmp=tmp->next;
			}
			printf("\n");				//deuxieme ligne
			tmp=p->maxMonome;
			afficherMonomeL(tmp,0);		
			tmp=tmp->next;
			while(tmp!=NULL){
				printf(" + ");
				afficherMonomeL(tmp,0);
				tmp=tmp->next;
			}
			printf("\n");				//troisieme ligne
			tmp=p->maxMonome;
			afficherMonomeL(tmp,-1);		
			tmp=tmp->next;
			while(tmp!=NULL){
				printf("   ");
				afficherMonomeL(tmp,-1);
				tmp=tmp->next;
			}
		}
		printf("\n");
	}
}
 
 
/* fonction utilisant free pour libérer la mémoire d’un polynôme alloué par le malloc de creerPolynome */

void libererM(monome*m){
	free(m);
}

void liberer(polynome*p){
	monome*tmp;	
	while(p->maxMonome!=NULL){
		tmp=p->maxMonome->next;
		libererM(p->maxMonome);
		p->maxMonome=tmp;
	}
	free(p);
}


/*
Cette procédure permet de créer un monome (a partir de son coefficient et son degré) et de l’intégrer à un polynôme (onveillera a preserver le tri par degré décroissant des monomes). Notez que elle ne retourne rien mais modifie lepolynôme passé en 1er argument.
*/

void ajouterMonomeA(polynome* p, double coeff , int degre ){
	//printf("\n	debug in ajouterMonomeA");
	monome*m=creerMonome(coeff,degre);
	monome*tmp=p->maxMonome;
	if(p->maxMonome==NULL){
		p->maxMonome=m;
	}	
	else if(p->maxMonome->degree>degre){
		while(tmp->next != NULL && tmp->degree>degre){
			tmp=tmp->next;
		}
		if(tmp->degree==degre){
			tmp->coef+=coeff;
		}
		else {
			m->next=tmp->next;
			tmp->next=m;
		}
	}
	else if(p->maxMonome->degree==degre){
		p->maxMonome->coef+=coeff;
	}
	else{
		m->next=p->maxMonome;
		p->maxMonome=m;
	}
}

void polynomeClean(polynome*p){
	monome*m=p->maxMonome;
	monome*tmp;
	if(m!=NULL){
		while(m!=NULL && m->next!=NULL){
			while(m->next->coef==0. && m->next!=NULL){
				tmp=m->next;
				m->next=m->next->next;
				libererM(tmp);
				//printf("\n		debug in cleaning 2while	m->degree=%i	P(x)= ",m->degree);
				//afficherPolynome(p);
				//printf("\n		debug in cleaning 2while	m->next=%p",m->next);
				if(m->next==NULL){
					//printf("\n	in break");
					break;
				}
			}
			//printf("\n		sortie du 2while");
			m=m->next;
			//printf("\n		m->next=%p",m);
		}
		//printf("\n		sortie du 1while");
	}
	else if(p->maxMonome->coef==0. && p->maxMonome->degree!=0){
		tmp=p->maxMonome;
		p->maxMonome=p->maxMonome->next;
		libererM(tmp);
		polynomeClean(p);
	}
	if(p->maxMonome->coef==0.){
		p->maxMonome->degree=0;
	}
	if(p->maxMonome==NULL){
		ajouterMonomeA(p,0.,0);
	}
}
/* Ces fonctions aux noms explcites créent et retournent un nouveau polynôme sans modifier les polynômes passes en entrée*/
polynome* copie(polynome* p){
	polynome*q=creerPolynomeVide();
	monome*tmp=p->maxMonome;
	while(tmp!= NULL ){
		ajouterMonomeA(q,tmp->coef,tmp->degree);
		tmp=tmp->next;
	}
	q->isLaplace=p->isLaplace;
	return q;
}


double coeffMonomeDansPolynome(polynome*p,int n){
	monome*m=p->maxMonome;
	while(m->degree>n){
		if(m->next!=NULL){
			m=m->next;
		}
		//printf("\n				debug coeffMonomeDansPolynome degreDemander=%i  degreActuel=%i\n",n,m->degree);
		if(m->next==NULL &&  m->degree>n){
			break;
		}
	}
	if(m->degree==n){
		return m->coef;
	}
	else{
		return 0;
	}
}

int degreeMin(polynome* p1 ,polynome* p2){
	int min1=0;
	int min2=0;
	monome*m=p1->maxMonome;
	while(m!=NULL){
		if(m->degree<min1){
			min1=m->degree;
		}
		m=m->next;
	}
	m=p2->maxMonome;
	while(m!=NULL){
		if(m->degree<min2){
			min2=m->degree;
		}
		m=m->next;
	}
	return min1+min2;
}

double coefCk(polynome* p1 ,polynome* p2,int n){
	//printf("\n		debug coefCk n=%i",n);
	double s=0;
	double a,b;
	int min=degreeMin(p1,p2);
	for(int i=min;i<=fabs(n);i++){
		//printf("\n		debug coefCk i=%i	n-i=%i",i,n-i);
		a=coeffMonomeDansPolynome(p1,i);
		b=coeffMonomeDansPolynome(p2,n-i);
		
		//printf("		debug coefCk a=%f b=%f\n",a,b);
		
		s+=a*b;
	}
	
	return s;
}

polynome* produit(polynome* p1 ,polynome* p2){
	int n=p1->maxMonome->degree+p2->maxMonome->degree;
	polynome*p=creerPolynomeVide();
	int min=degreeMin(p1,p2);
	//printf("min=%i",min);
	for(int i=min;i<=n;i++){
		//printf("\n	in for produit  n=%i",i);
		ajouterMonomeA(p,coefCk(p1,p2,i),i);
	}
	//printf("\n		debug avant cleaninig ");
	//afficherPolynome(p);
	polynomeClean(p);
	return p;
}


polynome* somme(polynome* p1, polynome* p2){
	monome*m=p1->maxMonome;
	polynome*p=creerPolynomeVide();
	while(m!=NULL){
		ajouterMonomeA(p,m->coef,m->degree);
		m=m->next;
	}
	m=p2->maxMonome;
	while(m!=NULL){
		ajouterMonomeA(p,m->coef,m->degree);
		m=m->next;
	}
	return p;
}

polynome* soustraction(polynome* p1, polynome* p2){
	monome*m=p1->maxMonome;
	polynome*p=creerPolynomeVide();
	while(m!=NULL){
		ajouterMonomeA(p,m->coef,m->degree);
		m=m->next;
	}
	m=p2->maxMonome;
	while(m!=NULL){
		ajouterMonomeA(p,-m->coef,m->degree);
		m=m->next;
	}
	polynomeClean(p);
	return p;
}


polynome* puissance(polynome* p, int n){
	polynome*q=copie(p);
	for(int i=1;i<n;i++){
		q=produit(q,p);
	}
	return q;
}

polynome* produitCst(double c , polynome* p){//calcule un polynome obtenu en multipliant p par la constante c
	polynome*C=creerPolynomeVide();
	ajouterMonomeA(C,c,0);
	polynome*q=produit(C,p);
	liberer(C);
	return q;
}

double factorielle(int n){
	if(n>100){
		return pow(n,n)*exp(-n)*sqrt(2*M_PI*n);
	}
	int i=1;
	for(int j=1;j<=n;j++){
		i*=j;
	}
	(double) i;
	return i;
}



polynome*inverseOrdre(polynome*p){
	polynome*q=creerPolynomeVide();
	monome*m=p->maxMonome;
	while(m!=NULL){
		ajouterMonomeA(q,m->coef,m->degree);
		m=m->next;
	}
	return q;
}

polynome*toLaplace(polynome*p){		//return a copy
	polynome*q=copie(p);
	monome*m=q->maxMonome;
	while(m!=NULL){
		m->coef*=factorielle(m->degree);	
		m->degree=m->degree*(-1)-1;
		m=m->next;
	}
	polynome*tmp=q;
	//printf("va inverser");
	q=inverseOrdre(q);
	//printf("a inverse");
	liberer(tmp);
	q->isLaplace=1;
	return q;
}

polynome*fromLaplace(polynome*p){		//return a copy
	polynome*q=copie(p);
	q->isLaplace=0;
	monome*m=q->maxMonome;
	while(m!=NULL){
		//printf("m->degree=%i\n",m->degree);
		if(m->degree!=0){
			m->degree=(m->degree+1)*(-1);
			m->coef*=1/factorielle(m->degree);
		}
		m=m->next;
	}
	polynome*tmp=q;
	q=inverseOrdre(q);
	liberer(tmp);
	return q;
}		
		
polynome* derive(polynome*p){//calcule le polynome dérivé		return a copy
	polynome*tmp;
	polynome*q=p;
	p=toLaplace(p);
	polynome*derivateur=creerPolynomeVide();
	ajouterMonomeA(derivateur,1.,1);
	tmp=p;
	p=produit(p,derivateur);
	ajouterMonomeA(p,evaluerPolynome(q,0.),0);
	liberer(tmp);
	tmp=p;
	p=fromLaplace(p);
	liberer(tmp);
	return p;
}

polynome*produitDeConvolution(polynome*p1,polynome*p2){
	p1=toLaplace(p1);
	p2=toLaplace(p2);
	polynome*p=produit(p1,p2);
	liberer(p1);
	liberer(p2);
	p1=fromLaplace(p);
	liberer(p);
	return p1;
}

polynome*primitive(polynome*p){
	polynome*tmp;
	polynome*q=p;
	p=toLaplace(p);
	polynome*integrateur=creerPolynomeVide();
	ajouterMonomeA(integrateur,1.,-1);
	tmp=p;
	p=produit(p,integrateur);
	ajouterMonomeA(p,evaluerPolynome(q,0.),0);
	liberer(tmp);
	tmp=p;
	p=fromLaplace(p);
	liberer(tmp);
	return p;
}	

polynome*composition(polynome*p1,polynome*p2){
	polynome*p=creerPolynomeVide();
	monome*m=p1->maxMonome;
	polynome*tmp_p;
	polynome*tmp_m;
	polynome*tmp_s;
	while(m!=NULL){
		tmp_p=puissance(p2,m->degree);
		tmp_m=produitCst(m->coef,tmp_p);
		liberer(tmp_p);
		tmp_s=somme(p,tmp_m);
		liberer(tmp_m);
		liberer(p);
		p=tmp_s;
		m=m->next;
	}
	return p;
}




int main(){
	polynome*poly=creerPolynomeVide();
	afficherPolynome(poly);	// print "vide"
	monome*m1=creerMonome(0.25,2);
	retourchariot=true;
	afficherMonome(m1);	//print "0.250000X^(2)"
	retourchariot=false;
	poly->maxMonome=m1;
	afficherPolynome(poly);	//print "0.250000X^(2)"
	printf("\nP(x=1)=%f	coef=%f\n",evaluerPolynome(poly,1),m1->coef);	//print "P(x=1)=0.250000	coef=0.250000"
	printf("P(x=2)=%f\n",evaluerPolynome(poly,2));				//print "P(x=2)=1.000000"

	ajouterMonomeA(poly,2,1);
	printf("P(x)=");
	afficherPolynome(poly);
	printf("P(x)=");
	ajouterMonomeA(poly,2,3);
	afficherPolynome(poly);
	printf("P(x=2)=%f\n",evaluerPolynome(poly,2));				//print "P(x=2)=21.000000"

	polynome*q=copie(poly);
	printf("\n");
	printf("Pcopie(x)= ");
	afficherPolynome(q);
	liberer(q);

	q=produit(poly,poly);
	printf("P(x)*P(x)=");
	afficherPolynome(q);
	liberer(q);
	
	printf("P(x)+P(x)=");
	q=somme(poly,poly);
	afficherPolynome(q);

	printf("P(x)-P(x)=");	
	liberer(q);
	q=soustraction(poly,poly);
	afficherPolynome(q);

	printf("P(x)^5=");
	liberer(q);
	q=puissance(poly,5);
	afficherPolynome(q);
	
	printf("P(x)*5=");
	liberer(q);
	q=produitCst(5.,poly);
	afficherPolynome(q);

	printf("\n\nL{P(p)}=\n");
	liberer(q);
	q=toLaplace(poly);
	afficherPolynome(q);
	
	printf("\nL^-1{L{P(p)}}=");
	polynome*q2=fromLaplace(q);
	afficherPolynome(q2);
	
	printf("dP(x)/dx=");
	liberer(q2);
	liberer(q);
	q=derive(poly);
	afficherPolynome(q);

	printf("(P*P)(x)=");
	liberer(q);
	q=produitDeConvolution(poly,poly);
	afficherPolynome(q);

	printf("primitive=");
	liberer(q);
	q=primitive(poly);
	afficherPolynome(q);

	printf("P(P(x))=");
	liberer(q);
	q=composition(poly,poly);
	afficherPolynome(q);


}
















