#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum ERROR_TYPE{
    NO_ERROR=0,
    NOT_AN_INTEGER,
    UNINIT_ARGUMENTS,
    NEGATIVE_INFINITY,
    POSITIVE_INFINITY,
}ERROR_TYPE;

typedef struct AVLnode {
	char *key;
	char *data;
	struct AVLnode *child[3];
	int height;
}AVLnode;

AVLnode *initialize(char *key, char *tagData)
{
	AVLnode *AVLnodeObject = (AVLnode *)malloc(sizeof(AVLnode));
	AVLnodeObject->key = key;
	AVLnodeObject->data = tagData;
	AVLnodeObject->child[0] = NULL;
	AVLnodeObject->child[1] = NULL;
	AVLnodeObject->child[2] = NULL;
	AVLnodeObject->height = 0;
	return AVLnodeObject;
}

int getBalanceFactor(AVLnode *AVLnodeObject, int *balance)
{
	int height[2], i;
	for(i=0;i<2;i++){
		if(AVLnodeObject->child[i]==NULL){
			height[i] = 0;
		}
		else{
			height[i] = AVLnodeObject->child[i]->height;
		}
	}
	*balance = height[0] - height[1];
	return 0;
}

AVLnode *rightRotate(AVLnode *AVLnodeTree)
{
    AVLnode *temp = AVLnodeTree->child[0]->child[1];
 	AVLnode *newAVLRoot = AVLnodeTree->child[0];

    newAVLRoot->child[1] = AVLnodeTree;
    AVLnodeTree->child[0] = temp;
    AVLnodeTree->height = (AVLnodeTree->child[0]->height > AVLnodeTree->child[1]->height ? AVLnodeTree->child[0]->height : AVLnodeTree->child[1]->height)+1;
    newAVLRoot->height = (newAVLRoot->child[0]->height > newAVLRoot->child[1]->height ? newAVLRoot->child[0]->height : newAVLRoot->child[1]->height)+1;
 
    return newAVLRoot;
}
 
AVLnode *leftRotate(AVLnode *AVLnodeTree)
{
	AVLnode *temp = AVLnodeTree->child[1]->child[0];
    AVLnode *newAVLRoot = AVLnodeTree->child[1];

    newAVLRoot->child[0] = AVLnodeTree;
    AVLnodeTree->child[1] = temp;
    AVLnodeTree->height = (AVLnodeTree->child[0]->height > AVLnodeTree->child[1]->height ? AVLnodeTree->child[0]->height : AVLnodeTree->child[1]->height)+1;
    newAVLRoot->height = (newAVLRoot->child[0]->height > newAVLRoot->child[1]->height ? newAVLRoot->child[0]->height : newAVLRoot->child[1]->height)+1;
  
    return newAVLRoot;
}

AVLnode *insert(AVLnode *AVLnodeTree, char *key, char *tagData)
{
	if(strcmp(AVLnodeTree->key, key)==0){
		// puts("Collision");
		if(AVLnodeTree->child[2] == NULL){
			AVLnodeTree->child[2] = initialize(key, tagData);
		}
		else{
			AVLnodeTree->child[2] = insert(AVLnodeTree->child[2], key, tagData);
		}
		return AVLnodeTree;
	}
	int child = strcmp(AVLnodeTree->key, key)>0 ? 0:1;
	// printf("Adding to %d\n", child);
	if(AVLnodeTree->child[child] == NULL){
		AVLnodeTree->child[child] = initialize(key, tagData);
	}
	else{
		AVLnodeTree->child[child] = insert(AVLnodeTree->child[child], key, tagData);
	}

	int balance;
	getBalanceFactor(AVLnodeTree, &balance);
	
    if (balance > 1){
    	if(strcmp(AVLnodeTree->child[0]->key, key)>0 ? 1:0)
        	return rightRotate(AVLnodeTree);
    }
 
    if (balance < -1){
    	if(strcmp(AVLnodeTree->child[1]->key, key)>0 ? 0:1)
        	return leftRotate(AVLnodeTree);
    }
 
    if (balance > 1){
        if(strcmp(AVLnodeTree->child[0]->key, key)>0 ? 0:1){
        	AVLnodeTree->child[0] =  leftRotate(AVLnodeTree->child[0]);
        	return rightRotate(AVLnodeTree);
        }
    }
 
    if (balance < -1){
    	if(strcmp(AVLnodeTree->child[0]->key, key)>0 ? 1:0){
        	AVLnodeTree->child[1] = rightRotate(AVLnodeTree->child[1]);
        	return leftRotate(AVLnodeTree);
    	}
    }
	return AVLnodeTree;
}

AVLnode *search(AVLnode *AVLnodeTree, char *key)
{
	if(strcmp(AVLnodeTree->key, key)==0){
		return AVLnodeTree;
	}
	int child = strcmp(AVLnodeTree->key, key)>0 ? 0:1;
	if(AVLnodeTree->child[child] == NULL){
		return NULL;
	}
	else{
		return search(AVLnodeTree->child[child], key);
	}
}

AVLnode *delete(AVLnode *AVLnodeTree, char *key)
{
	if(strcmp(AVLnodeTree->key, key)==0){
		return AVLnodeTree;
	}
	int child = strcmp(AVLnodeTree->key, key)>0 ? 0:1;
	if(AVLnodeTree->child[child] == NULL){
		return NULL;
	}
	else{
		return search(AVLnodeTree->child[child], key);
	}
}

void ioTraverse(AVLnode *AVLnodeObject)
{
	// printf("printing %s\n", AVLnodeObject->data);
	if(AVLnodeObject->child[0]!=NULL){
		ioTraverse(AVLnodeObject->child[0]);
	}
	printf("%s %s\n", AVLnodeObject->key, AVLnodeObject->data);
	if(AVLnodeObject->child[2]!=NULL){
		ioTraverse(AVLnodeObject->child[2]);
	}
	if(AVLnodeObject->child[1]!=NULL){
		ioTraverse(AVLnodeObject->child[1]);
	}
}

ERROR_TYPE strToInt(char *str, int *integer)
{
    if(!str){
        return UNINIT_ARGUMENTS;
    }
    *integer = 0;
    int i, len, negative = 0;
    len = strlen(str);
    if(str[0] == '-'){
        if(len == 1){
        	return NEGATIVE_INFINITY;
        }
        else{
        	negative = 1;
        	str = str + 1;
        	len--;
        }
    }
    if(str[0] == '+'){
    	if(len == 1){
        	return POSITIVE_INFINITY;
    	}
    	else{
    		negative = 0;
    		str = str + 1;
    		len--;
    	}
    }
    for(i = 0; i < len; i++){
    	if(str[i] < '0' || str[i] > '9'){
    		return NOT_AN_INTEGER;
    	}
        *integer = *integer * 10 + str[i] - '0';
    }
    if(negative == 1){
    	*integer = -*integer;
    }
    return NO_ERROR;
}

int main(int argc, char **argv)
{
    ERROR_TYPE error_code;
    int i, input;
    for(i = 1; i < argc; i++){
        error_code = strToInt(argv[i], &input);
        switch(error_code){
            case NO_ERROR:
                printf("Input is %d\n", input);
                break;
            case NOT_AN_INTEGER:
                puts("Input not an integer");
                break;
            case NEGATIVE_INFINITY:
                puts("-INFINITY");
                break;
            case POSITIVE_INFINITY:
                puts("+INFINITY");
                break;
        }
    }
    return 0;
}
