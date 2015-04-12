#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define SKIPLIST_MAX_LEVEL 6

typedef struct node {
    int key;
    char *data;
    struct node **forward;
}node;

typedef struct skipList {
    int level;
    int size;
    node *header;
}skipList;

skipList *initializeSkipList()
{
    int i;
    skipList *list = malloc(sizeof(skipList));
    node *header = (node *)malloc(sizeof(node));
    list->header = header;
    header->key = INT_MAX;
    header->forward = (node **)malloc(sizeof(node*) * (SKIPLIST_MAX_LEVEL+1));
    for (i = 0; i <= SKIPLIST_MAX_LEVEL; i++) {
        header->forward[i] = list->header;
    }

    list->level = 1;
    list->size = 0;

    return list;
}

static int rand_level()
{
    int level = 1;
    while (rand() < RAND_MAX/2 && level < SKIPLIST_MAX_LEVEL)
        level++;
    return level;
}

int skiplistInsert(skipList *list, int key, char *data)
{
    node *update[SKIPLIST_MAX_LEVEL+1];
    node *x = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[1];

    if (key == x->key) {
        x->data = data;
        return 0;
    } 
    else {
        level = rand_level();
        if (level > list->level) {
            for (i = list->level+1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }

        x = (node *)malloc(sizeof(node));
        x->key = key;
        x->data = data;
        x->forward = (node **)malloc(sizeof(node*) * (level + 1));
        for (i = 1; i <= level; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }
    }
    return 0;
}

node *skipListSearch(skipList *list, int key)
{
    node *x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
    if (x->forward[1]->key == key) {
        return x->forward[1];
    }
    else {
        return NULL;
    }
    return NULL;
}

static void skiplist_node_free(node *x)
{
    if (x) {
        free(x->forward);
        free(x);
    }
}

int skipListDelete(skipList *list, int key)
{
    int i;
    node *update[SKIPLIST_MAX_LEVEL + 1];
    node *x = list->header;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }

    x = x->forward[1];
    if (x->key == key) {
        for (i = 1; i <= list->level; i++) {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[1] = x->forward[i];
        }
        skiplist_node_free(x);

        while (list->level > 1 && list->header->forward[list->level] == list->header)
            list->level--;
        return 0;
    }
    return 1;
}

static void skiplistIOTraverse(skipList *list)
{
    node *x = list->header;
    while (x && x->forward[1] != list->header) {
        printf("%d[%s]->", x->forward[1]->key, x->forward[1]->data);
        x = x->forward[1];
    }
    printf("NIL\n");
}

int main(int argc, char **argv)
{
	if(argc==1){
		return 0;
	}
	int i = 1;
	int key;
	char *tagData;
    skipList *list;
    list = initializeSkipList();

	/* Loop through terminal arguments.*/
	while(i < argc){
		if (!strcmp(argv[i],"I")) {
			i += 2;
			key = atoi(argv[i]);
			i++;
			tagData = argv[i];
			skiplistInsert(list, key, tagData);
		}
		else if (!strcmp(argv[i],"S")) {
			i += 2;
			key = atoi(argv[i]);
			node *query = skipListSearch(list, key);
			if(query == NULL){
				puts("No such key found");
			}
			else{
				printf("key = %d, data = %s\n", key, query->data);
			}
		}
		else if (!strcmp(argv[i],"D")) {
			i += 2;
			key = atoi(argv[i]);
			skipListDelete(list, key);
		}
		else if (!strcmp(argv[i],"T")) {
			skiplistIOTraverse(list);
		}
		i++;
	}

    return 0;
}