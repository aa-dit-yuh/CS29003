#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct AVLnode {
	char *key;
	char *data;
	struct AVLnode *child[2];
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

void ioTraverse(AVLnode *AVLnodeObject)
{
	printf("printing %s\n", AVLnodeObject->data);
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

int main(int argc, char *argv[])
{
	if(argc==1){
		return 0;
	}
	char *key, *tagData;
	int i = 1;
	AVLnode *AVLnodeRoot;

	/*Create the Root Node. */
	while(strcmp(argv[i], "I")){
		// puts("Tree empty");
		if(!strcmp(argv[i], "S")){
			i+=2;
		}
		else if(!strcmp(argv[i], "D")){
			i+=2;
		}
		else{
			i++;
		}
	}
	i++;
	key = argv[i++];
	tagData = argv[i++];
	// printf("Creating Root node\n");
	AVLnodeRoot = initialize(key, tagData);

	/* Loop through terminal arguments.*/
	while(i < argc){
		// printf("%s\n",argv[i]);
		if(!strcmp(argv[i],"I")){
			i++;
			key = argv[i];
			i++;
			tagData = argv[i];
			insert(AVLnodeRoot, key, tagData);
		}
		else if(!strcmp(argv[i],"S")){
			i++;
			key = argv[i];
			// search(AVLnodeRoot, key);
		}
		else if(!strcmp(argv[i],"D")){
			i++;
			key = argv[i];
			// delete(AVLnodeRoot, key);	
		}
		else if(!strcmp(argv[i],"T")){
			ioTraverse(AVLnodeRoot);
		}
		i++;
	}
	return 0;
}