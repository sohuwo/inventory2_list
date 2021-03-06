//p416 title 10
//update inventory2.c(linked)

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"

#define NAME_LEN 25
#define FILE_ROUTE "D:/C/TEST/test/Debug/"

struct part
{
	int number;
	char name[NAME_LEN + 1];
	int on_hand;
	struct part *next;
};

struct file_part
{
	int number;
	char name[NAME_LEN + 1];
	int on_hand;
}data;

struct part *inventory = NULL;
//points to first part

struct part *find_part(int number);
void insert(void);
void search(void);
void update(void);
void print(void);
void save(void);
void recover(void);

//main: Prompts the user to enter an operation code,
//      then calls a function to perform the requested
//      action. Repeats until the user enters the 
//      command 'q'. Prints an error message if the user
//      enters an illegal code.

int main(void)
{
	char code;
	for (;;)
	{
		printf("Enter operation code: ");
		scanf(" %c", &code);
		//Skips to end of line
		while (getchar() != '\n')
			;
		switch (code)
		{
		case 'i':insert();
			break;
		case 's':search();
			break;
		case 'u':update();
			break;
		case 'p':print();
			break;
		case 'q':return 0;
	    case 'd':save();
				break;
	    case 'r':recover();
				break;
		default:printf("Illegal code\n");
		}
		printf("\n");
	}
	return 0;
}

//find_part:Looks up a part number in the inventory
//          list. Returns a pointer to the node
//          containing the part number; if the part
//          number is not found, returns NULL.

struct part *find_part(int number)
{
	struct part *p;

	for (p = inventory; p != NULL && number> p->number; p = p->next)
		;
	if (p != NULL && number == p->number)
		return p;
	return NULL;
}

//insert: Prompts the user for information about a new
//        part and then inserts the part into the
//        inventory list; the list remains sorted by
//        part number. Prints an error message and returns
//        prematurely if the part already exists or space
//        could not be allocated for the part.

void insert(void)
{
	struct part *cur, *prev, *new_node;

	new_node = malloc(sizeof(struct part));
	if (new_node == NULL)
	{
		printf("Database is full; can't"
			"add  more parts.\n");
		return;
	}

	printf("Enter part number: ");
	scanf("%d", &new_node->number);

	for (cur = inventory, prev = NULL; \
		cur != NULL && new_node->number > cur->number; \
		prev = cur, cur = cur->next)
		;
	if (cur != NULL && new_node->number\
		== cur->number)
	{
		printf("Part already exists.\n");
		free(new_node);
		return;
	}

	printf("Enter part name: ");
	read_line(new_node->name, NAME_LEN);
	printf("Enter quantity on hand: ");
	scanf("%d", &new_node->on_hand);

	new_node->next = cur;
	if (prev == NULL)
		inventory = new_node;
	else
		prev->next = new_node;

}

//search: Prompts the user to enter a part number, then
//        looks up the part in the database. If the part
//        exists, prints the name and quantity on hand;
//		  if not, prints an error message.

void search(void)
{
	int number;
	struct part *p;

	printf("Enter part number: ");
	scanf("%d", &number);
	p = find_part(number);
	if (p != NULL)
	{
		printf("Part name: %s\n", p->name);
		printf("Quantity on hand: %d\n", p->on_hand);
	}
	else
		printf("Part not found.\n");
}

//update: Prompts the user to enter a part number.
//        Prints an error message if the part doesn't
//        exist; otherwise, prompts the user to enter
//        change in quantity on hand and updates the
//        database.

void update(void)
{
	int number, change;
	struct part *p;

	printf("Enter part number: ");
	scanf("%d", &number);
	p = find_part(number);
	if (p != NULL)
	{
		printf("Enter change in quantity on hand: ");
		scanf("%d", &change);
		p->on_hand += change;
	}
	else
		printf("Part not found.\n");
}

//print: Prints a listing of all parts in the database,
//       showing the part number, part name, and
//       quantity on hand. Part numbers will appear in
//       ascending order.

void print(void)
{
	struct part *p;

	printf("Part Number    Part Name                    "
		"Quantity on Hand\n");
	for (p = inventory; p != NULL; p = p->next)
		printf("%7d        %-25s%11d\n", p->number,
			p->name, p->on_hand);
}

//save: Prompts the user to enter a filename, and save it.

void save()
{
	FILE *fp;
	struct part *p;
	char route[100] = FILE_ROUTE;
	char filename[100];

	printf("Enter name of output file: ");
	scanf("%s", filename);
	strcat(route, filename);

	if ((fp = fopen(route, "wb")) == NULL)
	{
		fprintf(stderr, "Can't save %s\n", route);
		return;
	}

	for (p = inventory; p != NULL; p = p->next)
	{
		data.number = p->number;
		sprintf(data.name, "%s", p->name);
		data.on_hand = p->on_hand;
		fwrite(&data, sizeof(struct file_part), 1, fp);
	}

	fclose(fp);
	printf("Save successfully\n");

	return;
}

//recover: download the database from the file

void recover()
{
	FILE *fp;
	struct part *new_node, *prev = NULL;
	char route[100] = FILE_ROUTE;
	char filename[100];

	printf("Enter name of input file: ");
	scanf("%s", filename);
	strcat(route, filename);

	if ((fp = fopen(route, "rb")) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", route);
		return;
	}

	new_node = malloc(sizeof(struct part));
	if (new_node == NULL)
	{
		printf("Database is full; can't"
			"add  more parts.\n");
		return;
	}

	if (fread(&data, sizeof(struct file_part), 1, fp) == 0)
		return;

	inventory = new_node;
	sprintf(new_node->name, "%s", data.name);
	new_node->number = data.number;
	new_node->on_hand = data.on_hand;
	new_node->next = NULL;

	while (fread(&data, sizeof(struct file_part), 1, fp) != 0)
	{
		prev = new_node;
		new_node->next = malloc(sizeof(struct part));
		if (new_node->next == NULL)
		{
			printf("Database is full; can't"
				"add  more parts.\n");
			exit(EXIT_FAILURE);
		}
		new_node = new_node->next;

		sprintf(new_node->name, "%s", data.name);
		new_node->number = data.number;
		new_node->on_hand = data.on_hand;
	}
	new_node->next = NULL;

	fclose(fp);
	printf("Recover successfully\n");

	return;
}
