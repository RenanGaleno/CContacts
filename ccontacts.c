/* 
Este trabalho está licenciado sob uma Licença Creative Commons Atribuição-CompartilhaIgual 4.0 Internacional.
Para ver uma cópia desta licença, visite http://creativecommons.org/licenses/by-sa/4.0/.

Desenvolvido por:
Renan Galeno <renangaleno@gmail.com>
Marcos Antuanny <caramarcos@hotmail.com>
Victor Patrick <vp8_10@hotmail.com>
*/
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <curses.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

//Settings
#define DBSIZE 10

//App title
char *title = "CContacts";

//Screen size
short int row,col;

//Structs
typedef struct{
	char street[100];
	char number[5];
	char district[50];
	char city[50];
	char state[50];
	char postalcode[15];
} addr;
typedef struct{
	char name [100];
	char email[100];
	char phone[16];
	addr address;
} contact;

contact contacts[DBSIZE];

/* Auxiliar third party delay function - http://c-for-dummies.com/blog/?p=69 */
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

//Print centered function
void printCentered(int row, int totalcols, char *string2print){
	move(row, 0);
	clrtoeol();
	mvprintw(row, (totalcols-strlen(string2print))/2,"%s\n", string2print);
}
void printCenteredDec(int row, int totalcols, char *string2print, char decorationChar){
	short int _prtcnt_i;
	//Print the decoration char above title
	for(_prtcnt_i = 0; _prtcnt_i < ((totalcols-strlen(string2print))/2)-1; _prtcnt_i++){
		mvprintw(row, _prtcnt_i, "#");
	}
	//Print title in the center
	mvprintw(row, (totalcols-strlen(string2print))/2,"%s\n", string2print);
	//Print the decoration char after title
	for(_prtcnt_i = _prtcnt_i + strlen(string2print) + 2; _prtcnt_i < col; _prtcnt_i++){
		mvprintw(row, _prtcnt_i, "#");
	}
}

//Function alphasort
int _strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}
int cstring_cmp(const void* a, const void* b)
{
    const char *ia = (const char *)a;
    const char *ib = (const char *)b;
    return _strcicmp(ia, ib);
}

//Contact clear
void contactclr(contact *contactToClear){
	strcpy(contactToClear->name, "");
	strcpy(contactToClear->email, "");
	strcpy(contactToClear->phone, "");
	strcpy(contactToClear->address.street, "");
	strcpy(contactToClear->address.number, "");
	strcpy(contactToClear->address.district, "");
	strcpy(contactToClear->address.city, "");
	strcpy(contactToClear->address.state, "");
	strcpy(contactToClear->address.postalcode, "");
}
//Contacts clear
void contactsclr(contact *contactsToClear, size_t arraysize){
	for(int i=0; i<arraysize; i++){
		contactclr(&contactsToClear[i]);
	}
}

//Show header with title
void showHeader(){
	//Clear screen
	wclear(stdscr);
	//Declare i for counter
	short int _clrscr_i;
	//Print the first divisor line
	for(_clrscr_i = 0; _clrscr_i < col; _clrscr_i++){
		mvprintw(0,_clrscr_i,"-");
	}
	//Print the # above title
	for(_clrscr_i = 0; _clrscr_i < ((col-strlen(title))/2)-1; _clrscr_i++){
		mvprintw(1,_clrscr_i,"#");
	}
	//Print title in the center
	mvprintw(1, (col-strlen(title))/2,"%s\n", title);
	//Print the # after title
	for(_clrscr_i = _clrscr_i + strlen(title) + 2; _clrscr_i < col; _clrscr_i++){
		mvprintw(1, _clrscr_i, "#");
	}
	//Print a divisor line
	for(_clrscr_i = 0;_clrscr_i < col; _clrscr_i++){
		printw("-");
	}
}

void readContacts(contact *contactsToRead, short int alphabetic);

int getLastContactIndex(contact *getLastContactIndexContacts){
	int lastContact = -1;
	contact actualContact;
	//readContacts(getLastContactIndexContacts, 0);
	for(int i=0; i<DBSIZE; i++){
		actualContact = getLastContactIndexContacts[i];
		if(strlen(actualContact.name) > 0)
			lastContact = i;
			
	}
	return(lastContact);
}


//Declare buffer
char rbuffer[DBSIZE][500];
void readContacts(contact *contactsToRead, short int alphabetic){
	//Clear contacts
	contactsclr(contactsToRead, DBSIZE);
	//Clear buffer
	for(int i=0; i < DBSIZE; i++){
		strcpy(rbuffer[i], "");
	}
	//Declare file pointer
	FILE *fpR = fopen("contacts.tdb", "r");
	for(int i=0; i < DBSIZE; i++){
		//Read db file i line into buffer string
		fgets(rbuffer[i], 500, fpR);
	}
	//Param2, alphabetic order or normal
	if(alphabetic == 1){
		//Sort in alphabetic order, using previously defined cstring_cmp as comparer, with qsort
		qsort(rbuffer, sizeof(rbuffer)/sizeof(rbuffer[0]), sizeof(rbuffer[0]), cstring_cmp);
	}
	/*
	for(int i=0; i< DBSIZE; i++)
		printw(rbuffer[i]);
	return(0);
	*/
	
	//Run by buffer lines
	int i2 = 0;
	for(int i=0; i<DBSIZE; i++){
		//Buffers
		char rC;
		char rBuffer2[100] = "";
		//";", field separator counter
		int rI = 0;
		bool decreaseI = 0;
		//Run by the line
		for(int j=0; j<500; j++){
			//If it's a normal char, add into buffer, for later use
			if(rbuffer[i][j] != ';'){
				char rBuffer3[2] = {rbuffer[i][j], '\0'};
				strcat(rBuffer2, rBuffer3);
			}else{
				//If it's a ";", field separator, we'll put the buffer into contact, considering the order (rI)
				//Check if buffer is blank, if it is, not add, mark decrease index
				if(!strcmp(rBuffer2, "")){
					decreaseI = 1;
				}else{
					switch(rI){
						case 0:
							strcpy(contactsToRead[i2].name, rBuffer2);
						break;
						case 1:
							strcpy(contactsToRead[i2].email, rBuffer2);
						break;
						case 2:
							strcpy(contactsToRead[i2].phone, rBuffer2);
						break;
						case 3:
							strcpy(contactsToRead[i2].address.street, rBuffer2);
						break;
						case 4:
							strcpy(contactsToRead[i2].address.number, rBuffer2);
						break;
						case 5:
							strcpy(contactsToRead[i2].address.district, rBuffer2);
						break;
						case 6:
							strcpy(contactsToRead[i2].address.city, rBuffer2);
						break;
						case 7:
							strcpy(contactsToRead[i2].address.state, rBuffer2);
						break;
						case 8:
							strcpy(contactsToRead[i2].address.postalcode, rBuffer2);
						break;
					}
				}
				//Clear buffer for after use
				strcpy(rBuffer2, "");
				//Increase field counter
				rI++;
			}
		}
		//printw("Decrease i: %d", decreaseI);
		if(!decreaseI){
			i2++;
		}
	}
	
	fclose(fpR);
}

void saveContacts(contact *contactsToSave){
	//Declare file pointer
	FILE *fpS = fopen("contacts.tdb", "w");
	//Complete on end counter
	int completeOnEnd = 0;
	for(int i=0; i<DBSIZE; i++){
		//Run by contacts writing on database, if name is not blank
		if(strcmp(contactsToSave[i].name, "")){
			fprintf(fpS, "%s;%s;%s;%s;%s;%s;%s;%s;%s;\n", contactsToSave[i].name, contactsToSave[i].email, contactsToSave[i].phone, contactsToSave[i].address.street, contactsToSave[i].address.number, contactsToSave[i].address.district, contactsToSave[i].address.city, contactsToSave[i].address.state, contactsToSave[i].address.postalcode);
		}else{
			completeOnEnd++;
		}
	}
	for(int i=0; i<completeOnEnd; i++)
		fprintf(fpS, ";;;;;;;;;\n");
	fclose(fpS);
}

int menu();

void newContact(){
	showHeader();//Show the header, clearing the screen
	printCentered(3, col, "Criar contato");//Print "criar contato" centered

	contact newContact;//Declare new contact
	//Clear new contact memory space, to prevent unexpected things to happen
	contactclr(&newContact);

	//Get the contact info, denying null entries
	while(strlen(newContact.name) <= 0){
		printw("Nome: ");
		getstr(newContact.name);
	}
	while(strlen(newContact.email) <= 0){
		printw("Email: ");
		getstr(newContact.email);
	}
	while(strlen(newContact.phone) <= 0){
		printw("Telefone: ");
		getstr(newContact.phone);
	}
	while(strlen(newContact.address.street) <= 0){
		printw("Rua: ");
		getstr(newContact.address.street);
	}
	while(strlen(newContact.address.number) <= 0){
		printw("Número: ");
		getstr(newContact.address.number);
	}
	while(strlen(newContact.address.district) <= 0){
		printw("Bairro: ");
		getstr(newContact.address.district);
	}
	while(strlen(newContact.address.city) <= 0){
		printw("Cidade: ");
		getstr(newContact.address.city);
	}
	while(strlen(newContact.address.state) <= 0){
		printw("Estado: ");
		getstr(newContact.address.state);
	}
	while(strlen(newContact.address.postalcode) <= 0){
		printw("CEP: ");
		getstr(newContact.address.postalcode);
	}

	readContacts(contacts, 0);//Read actual contacts

	contacts[getLastContactIndex(contacts)+1] = newContact;//Add this new contact into contacts array

	saveContacts(contacts);//Save into database

	//Ok, contact saved
	curs_set(0);//Disable cursor
	showHeader();//Show the header, clearing the screen
	printCentered(4, col, "Contato criado!");//Show success message
	refresh();//Refresh screen message
	delay(1200);//Wait some ms before back to menu
	curs_set(1);//Enable cursor

	menu();//Back to menu
}

void deleteContact(){
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Apagar Contatos");
	//Read actual contacts in normal order
	readContacts(contacts, 0);

	//Get the phone number from the user
	char phone[16] = "";
	while(strlen(phone) <= 0){
		move(4, 0);
		clrtoeol();
		mvprintw(4, 0, "Digite o telefone a ser removido dos contatos: ");
		getstr(phone);
	}

	int contactsRemoved = 0;

	//Loop trought contacts looking for the contacts that match with the phone
	for(int i=0; i<DBSIZE; i++){
		if(!strcmp(contacts[i].phone, phone)){
			//Clear the contact memory space
			strcpy(contacts[i].name, "");
			//Set true on contactRemoved, advancing the counter
			contactsRemoved++;
		}
	}

	//Disable cursor
	curs_set(0);
	//Clear line 4
	move(4,0); clrtoeol();
	//Check if some contact was removed
	if(contactsRemoved){
		//At least one contact removed successfully
		if(contactsRemoved==1)
			mvprintw(4, 0, "1 contato com o telefone %s foi deletado com sucesso!", phone);
		if(contactsRemoved>1)
			mvprintw(4, 0, "%d contatos com o telefone %s foram deletados com sucesso!", contactsRemoved, phone);
	}else{
		mvprintw(4, 0, "Nenhum contato com o telefone %s foi encontrado", phone);
	}
	//Refresh screen to ensure showing
	refresh();

	//Save contacts
	saveContacts(contacts);

	//Delay some ms
	delay(1500);
	//Enable cursor
	curs_set(1);
	//Back to menu
	menu();
}

//Update contact
void updateContact(){
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Atualizar Contato");
	//Read actual contacts in normal order
	readContacts(contacts, 0);

	//Get the phone number from the user
	char phone[16] = "";
	while(strlen(phone) <= 0){
		move(4, 0);	clrtoeol();
		mvprintw(4, 0, "Digite o telefone do contato a ser atualizado: ");
		getstr(phone);
	}

	bool contactUpdated = 0;

	//Loop trought contacts looking for the contacts that match with the phone
	for(int i=0; i<DBSIZE && !contactUpdated; i++){
		if(!strcmp(contacts[i].phone, phone)){
			//Set true on contactUpdated
			contactUpdated = 1;

			contact newContact;//Declare new contact
			//Clear new contact memory space, to prevent unexpected things to happen
			contactclr(&newContact);

			//Get the contact info, denying null entries
			move(4, 0);	clrtoeol();
			mvprintw(4, 0, "Entre com as novas informações do contato: \n");
			while(strlen(newContact.name) <= 0){
				printw("Nome: ");
				getstr(newContact.name);
			}
			while(strlen(newContact.email) <= 0){
				printw("Email: ");
				getstr(newContact.email);
			}
			while(strlen(newContact.phone) <= 0){
				printw("Telefone: ");
				getstr(newContact.phone);
			}
			while(strlen(newContact.address.street) <= 0){
				printw("Rua: ");
				getstr(newContact.address.street);
			}
			while(strlen(newContact.address.number) <= 0){
				printw("Número: ");
				getstr(newContact.address.number);
			}
			while(strlen(newContact.address.district) <= 0){
				printw("Bairro: ");
				getstr(newContact.address.district);
			}
			while(strlen(newContact.address.city) <= 0){
				printw("Cidade: ");
				getstr(newContact.address.city);
			}
			while(strlen(newContact.address.state) <= 0){
				printw("Estado: ");
				getstr(newContact.address.state);
			}
			while(strlen(newContact.address.postalcode) <= 0){
				printw("CEP: ");
				getstr(newContact.address.postalcode);
			}

			//Assign new contact on contacts array
			contacts[i] = newContact;
		}
	}

	//Disable cursor
	curs_set(0);
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Atualizar Contato");
	//Check if a contact was updated
	if(contactUpdated){
		//Updated
		move(4, 0);	clrtoeol();
		mvprintw(4, 0, "Contato atualizado!");
	}else{
		//Not found
		move(4, 0);	clrtoeol();
		mvprintw(4, 0, "Contato não encontrado!");
	}
	//Refresh screen to show info
	refresh();

	//Save contacts
	saveContacts(contacts);

	//Delay some ms
	delay(1500);
	//Enable cursor
	curs_set(1);
	//Back to menu
	menu();
}

//Search by phone
void searchByPhone(){
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Buscar por telefone");
	//Ask user for the phone
	mvprintw(4, 0, "Digite o telefone: ");
	char phoneToSearch[16];
	getstr(phoneToSearch);
	//Read all contacts
	readContacts(contacts, 0);
	//Declare a new contacts array, for contacts with this phone
	contact byPhoneContacts[DBSIZE];
	//Clear the contacts array
	contactsclr(byPhoneContacts, DBSIZE);
	int i2 = 0;
	for(int i=0;i<DBSIZE;i++){
		//Run by contacts, checking if it's a result for the search
		if(!strcmp(contacts[i].phone, phoneToSearch)){
			byPhoneContacts[i2] = contacts[i];
			i2++;
		}
	}
	if(strcmp(byPhoneContacts[0].name, "")){
		//Check if there is a second contact with this name
		if(strcmp(byPhoneContacts[1].name, "")){
			//More than one contact, show the list
			//Run by the read contacts with this number
			for(int i=0; i<DBSIZE; i++){
				//If it's not blank, then...
				if(strlen(byPhoneContacts[i].name) > 0){
					//Check if it's for be shown on left, or right
					int colToShow = 0;
					//if(i%2 == 0)
					colToShow = ((i+1)%2 == 0)? col/2 : 0;
					//Show it
					mvprintw((i/2)+5, colToShow, "%d->%s", i, byPhoneContacts[i].name);
				}
			}
			//Ask user to choose
			mvprintw(4, 0, "Escolha um contato para ver detalhes, digitando o número correspondente:");
			char str_contactId[10];
			//Clear possible memory leak
			strcpy(str_contactId, "");
			//Read id from user
			while(strlen(str_contactId) <= 0){
				getstr(str_contactId);
			}
			//Cast to int
			int contactId = atoi(str_contactId);
			//Check if it's valid
			if(contactId >= 0 && contactId <= getLastContactIndex(byPhoneContacts) && strlen(byPhoneContacts[contactId].name) > 0){
				//Print title centered
				char title[100] = "Detalhes do Contato ";
				strcat(title, byPhoneContacts[contactId].phone);
				printCentered(3, col, title);
				//Print contact info
				mvprintw(4, 0, "Nome: %s\nEmail: %s\nTelefone: %s\nEndereço:\n	Rua:%s\n	Número:%s\n	Bairro:%s\n	Cidade:%s\n	Estado:%s\n	CEP:%s\n", byPhoneContacts[contactId].name, byPhoneContacts[contactId].email, byPhoneContacts[contactId].phone, byPhoneContacts[contactId].address.street, byPhoneContacts[contactId].address.number, byPhoneContacts[contactId].address.district, byPhoneContacts[contactId].address.city, byPhoneContacts[contactId].address.state, byPhoneContacts[contactId].address.postalcode);
				curs_set(0);
				printw("Pressione qualquer tecla para voltar.");
				getch();
				curs_set(1);
			}
		}else{
			//Only one contact, directly show it in details
			//Print title centered
			char title[100] = "Detalhes do Contato ";
			strcat(title, byPhoneContacts[0].phone);
			printCentered(3, col, title);
			//Print contact info
			mvprintw(4, 0, "Nome: %s\nEmail: %s\nTelefone: %s\nEndereço:\n	Rua:%s\n	Número:%s\n	Bairro:%s\n	Cidade:%s\n	Estado:%s\n	CEP:%s\n", byPhoneContacts[0].name, byPhoneContacts[0].email, byPhoneContacts[0].phone, byPhoneContacts[0].address.street, byPhoneContacts[0].address.number, byPhoneContacts[0].address.district, byPhoneContacts[0].address.city, byPhoneContacts[0].address.state, byPhoneContacts[0].address.postalcode);
			curs_set(0);
			printw("Pressione qualquer tecla para voltar.");
			getch();
			curs_set(1);
		}
	}else{
		//Contact not found
		//Disable cursor
		curs_set(0);
		//Print not found message
		mvprintw(4, 0, "Contato não encontrado!\nPressione qualquer tecla para voltar.");
		getch();
		curs_set(1);
	}
	menu();
}

//List contacts
void listContacts(short int alphabetic){
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Lista de Contatos");
	//Read actual contacts in normal order or alphabetic, depending on $alphabetic
	readContacts(contacts, alphabetic);

	//Run by the read contacts
	for(int i=0; i<DBSIZE; i++){
		//If it's not blank, then...
		if(strlen(contacts[i].name) > 0){
			//Check if it's for be shown on left, or right
			int colToShow = 0;
			//if(i%2 == 0)
			colToShow = ((i+1)%2 == 0)? col/2 : 0;
			//Show it
			if(alphabetic == 1){
				mvprintw((i/2)+5, colToShow, "%d->%s: %s", i, contacts[i].name, contacts[i].phone);
			}else{
				mvprintw((i/2)+5, colToShow, "%d->%s", i, contacts[i].name);
			}
		}
	}

	mvprintw(4, 0, "Escolha um contato para ver detalhes, digitando o número correspondente:");

	char str_contactId[10];
	strcpy(str_contactId, "");
	while(strlen(str_contactId) <= 0){
		getstr(str_contactId);
	}
	int contactId = atoi(str_contactId);
	if(contactId >= 0 && contactId <= getLastContactIndex(contacts) && strlen(contacts[contactId].name) > 0){
		//Contact exists (maybe...)
		//Clear and show the header
		showHeader();
		//Print title centered
		char title[100] = "Detalhes do Contato ";
		strcat(title, contacts[contactId].phone);
		printCentered(3, col, title);
		mvprintw(4, 0, "Nome: %s\nEmail: %s\nTelefone: %s\nEndereço:\n	Rua:%s\n	Número:%s\n	Bairro:%s\n	Cidade:%s\n	Estado:%s\n	CEP:%s\n", contacts[contactId].name, contacts[contactId].email, contacts[contactId].phone, contacts[contactId].address.street, contacts[contactId].address.number, contacts[contactId].address.district, contacts[contactId].address.city, contacts[contactId].address.state, contacts[contactId].address.postalcode);
		curs_set(0);
		printw("Pressione qualquer tecla para voltar.");
		getch();
		curs_set(1);
	}else{
		//Contact not exists
		move(4, 0);
		clrtoeol();
		mvprintw(4, 0, "O contato selecionado não existe, pressione qualquer tecla para continuar.");
		curs_set(0);
		getch();
		curs_set(1);
		listContacts(alphabetic);
	}
	menu();
}


//List by letter
void listByLetter(){
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Listar por letra");
	//Read actual contacts in alphabetic order
	readContacts(contacts, 1);
	//Declare and initialize a contacts array
	contact contactsByLetter[DBSIZE];
	//Clear the contacts array
	contactsclr(contactsByLetter, DBSIZE);

	//Ask for the letter
	char letterToList = ' ';
	curs_set(1);
	//While it's not a letter...
	while(!isalpha(letterToList)){
		move(4, 0); clrtoeol();//Clear line 4
		mvprintw(4, 0, "Pressione a tecla da primeira letra do nome do contato: ");//Ask for the letter
		letterToList = getch();//Use getch to get the char that should be a letter
	}

	//Run by contacts checking if the name's first letter is equal to the one informed by user
	int i2=0;
	for(int i=0; i < DBSIZE; i++){
		//Compare the contact's name's first letter to the informed by user to search
		if(contacts[i].name[0] == tolower(letterToList) || contacts[i].name[0] == toupper(letterToList)){
			//The letter is that, so copy to our array
			contactsByLetter[i2] = contacts[i];
			i2++;
		}
	}

	//Check if there are results
	if(strcmp(contactsByLetter[0].name, "")){
		//There are results
		//Run by the resultant contacts, listing them
		for(int i=0; i<DBSIZE; i++){
			//If it's not blank, then...
			if(strlen(contactsByLetter[i].name) > 0){
				//Check if it's for be shown on left, or right
				int colToShow = 0;
				//if(i%2 == 0)
				colToShow = ((i+1)%2 == 0)? col/2 : 0;
				//Show it
				mvprintw((i/2)+5, colToShow, "%d->%s", i, contactsByLetter[i].name);
			}
		}

		move(4, 0); clrtoeol();
		mvprintw(4, 0, "Escolha um contato para ver detalhes, digitando o número correspondente:");

		char str_contactId[10];
		strcpy(str_contactId, "");
		while(strlen(str_contactId) <= 0){
			getstr(str_contactId);
		}
		int contactId = atoi(str_contactId);
		if(contactId >= 0 && contactId <= getLastContactIndex(contactsByLetter) && strlen(contactsByLetter[contactId].name) > 0){
			//Contact exists (maybe...)
			//Clear and show the header
			showHeader();
			//Print title centered
			char title[100] = "Detalhes do Contato ";
			strcat(title, contactsByLetter[contactId].phone);
			printCentered(3, col, title);
			mvprintw(4, 0, "Nome: %s\nEmail: %s\nTelefone: %s\nEndereço:\n	Rua:%s\n	Número:%s\n	Bairro:%s\n	Cidade:%s\n	Estado:%s\n	CEP:%s\n", contactsByLetter[contactId].name, contactsByLetter[contactId].email, contactsByLetter[contactId].phone, contactsByLetter[contactId].address.street, contactsByLetter[contactId].address.number, contactsByLetter[contactId].address.district, contactsByLetter[contactId].address.city, contactsByLetter[contactId].address.state, contactsByLetter[contactId].address.postalcode);
			curs_set(0);
			printw("Pressione qualquer tecla para voltar.");
			getch();
			curs_set(1);
		}else{
			//Contact not exists
			move(4, 0); clrtoeol();
			mvprintw(4, 0, "O contato selecionado não existe, pressione qualquer tecla para continuar.");
			curs_set(0);
			getch();
			curs_set(1);
			listByLetter();
		}
	}else{
		//No results :/
		curs_set(0);
		move(4, 0); clrtoeol();
		mvprintw(4, 0, "Não há contatos com a letra escolhida.\nPressione qualquer tecla para voltar.");
		getch();
		curs_set(1);
	}

	menu();
}

//List by DDD
void listByDDD(){
	//Clear and show the header
	showHeader();
	//Print title centered
	printCentered(3, col, "Listar por DDD");
	//Read actual contacts in alphabetic order
	readContacts(contacts, 1);
	//Declare and initialize a contacts array
	contact contactsByDDD[DBSIZE];
	//Clear the contacts array
	contactsclr(contactsByDDD, DBSIZE);

	//Ask for the DDD
	char DDDToList[4] = "";
	curs_set(1);

	//While it's not a DDD...
	while(strlen(DDDToList) <= 0){
		move(4, 0); clrtoeol();//Clear line 4
		mvprintw(4, 0, "Digite o DDD dos contatos que devem ser listados: ");//Ask for the DDD
		getstr(DDDToList);
	}

	//Run by contacts checking if the name's first DDD is equal to the one informed by user
	int i2=0;
	for(int i=0; i < DBSIZE; i++){
		//Compare the contact's name's first DDD to the informed by user to search
		if(contacts[i].phone[0] == DDDToList[0] && contacts[i].phone[1] == DDDToList[1] && contacts[i].phone[2] == DDDToList[2] && contacts[i].phone[3] == DDDToList[3]){
			//The DDD is that, so copy to our array
			contactsByDDD[i2] = contacts[i];
			i2++;
		}
	}

	//Check if there are results
	if(strcmp(contactsByDDD[0].name, "")){
		//There are results
		//Run by the resultand contacts, listing them
		for(int i=0; i<DBSIZE; i++){
			//If it's not blank, then...
			if(strlen(contactsByDDD[i].name) > 0){
				//Check if it's for be shown on left, or right
				int colToShow = 0;
				//if(i%2 == 0)
				colToShow = ((i+1)%2 == 0)? col/2 : 0;
				//Show it
				mvprintw((i/2)+5, colToShow, "%d->%s", i, contactsByDDD[i].name);
			}
		}

		move(4, 0); clrtoeol();
		mvprintw(4, 0, "Escolha um contato para ver detalhes, digitando o número correspondente:");

		char str_contactId[10];
		strcpy(str_contactId, "");
		while(strlen(str_contactId) <= 0){
			getstr(str_contactId);
		}
		int contactId = atoi(str_contactId);
		if(contactId >= 0 && contactId <= getLastContactIndex(contactsByDDD) && strlen(contactsByDDD[contactId].name) > 0){
			//Contact exists (maybe...)
			//Clear and show the header
			showHeader();
			//Print title centered
			char title[100] = "Detalhes do Contato ";
			strcat(title, contactsByDDD[contactId].phone);
			printCentered(3, col, title);
			mvprintw(4, 0, "Nome: %s\nEmail: %s\nTelefone: %s\nEndereço:\n	Rua:%s\n	Número:%s\n	Bairro:%s\n	Cidade:%s\n	Estado:%s\n	CEP:%s\n", contactsByDDD[contactId].name, contactsByDDD[contactId].email, contactsByDDD[contactId].phone, contactsByDDD[contactId].address.street, contactsByDDD[contactId].address.number, contactsByDDD[contactId].address.district, contactsByDDD[contactId].address.city, contactsByDDD[contactId].address.state, contactsByDDD[contactId].address.postalcode);
			curs_set(0);
			printw("Pressione qualquer tecla para voltar.");
			getch();
			curs_set(1);
		}else{
			//Contact not exists
			move(4, 0); clrtoeol();
			mvprintw(4, 0, "O contato selecionado não existe, pressione qualquer tecla para continuar.");
			curs_set(0);
			getch();
			curs_set(1);
			listByDDD();
		}
	}else{
		//No results :/
		curs_set(0);
		move(4, 0); clrtoeol();
		mvprintw(4, 0, "Não há contatos com o DDD escolhido.\nPressione qualquer tecla para voltar.");
		getch();
		curs_set(1);
	}

	menu();
}

//Exit (credits)
void exitCredits(){
	//Disable cursor
	curs_set(0);
	//Clear screen and show the header
	showHeader();
	//Print the thanks on center
	mvprintw(6, (col-strlen("Esperamos que o software tenha sido útil!"))/2,"Esperamos que o software tenha sido útil!\n");
	//Delay
	refresh(); delay(600);
	//Print the credits
	mvprintw(9, 0, "Desenvolvido por:\n");
	printw("Renan Galeno <renangaleno@gmail.com>\n");
	printw("Marcos Antuanny <caramarcos@hotmail.com>\n");
	printw("Victor Patrick <vp8_10@hotmail.com>\n");
	printw("\n");
	//Delay
	refresh(); delay(600);
	printw("Este trabalho está licenciado sob uma Licença Creative Commons Atribuição-CompartilhaIgual 4.0 Internacional.\nPara ver uma cópia desta licença, visite http://creativecommons.org/licenses/by-sa/4.0/.\n\n");
	//Delay
	refresh(); delay(1000);
	//Print the press anything to exit message
	printw("Pressione qualquer tecla para sair.");
	getch();

}

//Menu
int menu(){
	curs_set(0);
	showHeader();
	mvprintw(3, 3, "1 - Criar contato");
	mvprintw(4, 3, "2 - Apagar contato");
	mvprintw(5, 3, "3 - Atualizar contato");
	mvprintw(6, 3, "4 - Buscar contato por telefone");
	mvprintw(7, 3, "5 - Listar contatos");
	mvprintw(8, 3, "6 - Listar contatos em ordem alfabética");
	mvprintw(9, 3, "7 - Listar por letra");
	mvprintw(10, 3, "8 - Listar por DDD");
	mvprintw(11, 3, "q ou 9 - Sair");

	char menuopt;
	curs_set(1);
	while(!(menuopt == '1' | menuopt == '2' | menuopt == '3' | menuopt == '4' | menuopt == '5' | menuopt == '6' | menuopt == '7' || menuopt == '8' || menuopt == '9' || menuopt == 'q')){
		move(12, 5);
		clrtoeol();
		mvprintw(12, 5, "> Entre com a opção desejada: ");
		menuopt = getch();
	}

	switch(menuopt){
		case '1':
			newContact();
		break;
		case '2':
			deleteContact();
		break;
		case '3':
			updateContact();
		break;
		case '4':
			searchByPhone();
		break;
		case '5':
			listContacts(0);
		break;
		case '6':
			listContacts(1);
		break;
		case '7':
			listByLetter();
		break;
		case '8':
			listByDDD();
		break;
		case '9':
			exitCredits();
		break;
		case 'q':
			exitCredits();
		break;
		default:
		break;
	}

	return menuopt - '0';
}

int main(){
	setlocale(LC_ALL, "ptb");//Set pt-br
	initscr();//Initialize cursor mode
	start_color();//Initialize color mode
	getmaxyx(stdscr,row,col);//Get screen dimensions
	attron(A_BOLD);//Use bold letters

	menu();//Call menu

	endwin();//End curses mode

	return(0);//Return 0, no errors
}