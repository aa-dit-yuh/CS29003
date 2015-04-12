
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

typedef struct node
{  int data;
struct node *left,*right;
int ht;
char tagdata[200];
}node;

int flag=0;
node *insert(node *,int,char[]);
node *Delete(node *,int);
void  inorder(node *);
int   height( node *);
node *rotateright(node *);
node *rotateleft(node *);
node *RR(node *);
node *LL(node *);
node *LR(node *);
node *RL(node *);
int BF(node *);
int height(node *T)
{
               int lh,rh;
               if(T==NULL)
                              return(0);
               if(T->left==NULL)
                              lh=0;
               else
                              lh=1+T->left->ht;
               if(T->right==NULL)
                              rh=0;
               else
                              rh=1+T->right->ht;
               if(lh>rh)
                              return(lh);
               return(rh);
}
node * rotateright(node *x)
{
               node *y;
               y=x->left;
               x->left=y->right;
               y->right=x;
               x->ht=height(x);
               y->ht=height(y);
               return(y);
}
node * rotateleft(node *x)
{
               node *y;
               y=x->right;
               x->right=y->left;
               y->left=x;
               x->ht=height(x);
               y->ht=height(y);
               return(y);
}
node * RR(node *T)
{
               T=rotateleft(T);
               return(T);
}
node * LL(node *T)
{
               T=rotateright(T);
               return(T);
}
node * LR(node *T)
{
               T->left=rotateleft(T->left);
               T=rotateright(T);
               return(T);
}
node * RL(node *T)
{
               T->right=rotateright(T->right);
               T=rotateleft(T);
               return(T);
}
int BF(node *T)
{
               int lh,rh;
               if(T==NULL)
               return(0);
               if(T->left==NULL)
                              lh=0;
               else
                              lh=1+T->left->ht;
               if(T->right==NULL)
                              rh=0;
               else
                              rh=1+T->right->ht;
               return(lh-rh);
}

node *Successor(node* n)
{
    node* p = n->right;

    // loop down to find the leftmost leaf
    while (p->left != NULL)
       p =p->left;

    return p;
}

node* balance(node* root)
{
    int balance = BF(root);

    // Left Left Case
    if (balance > 1 && BF(root->left) >= 0)
        return LL(root);

    // Left Right Case
    if (balance > 1 && BF(root->left) < 0)
    {
        return LR(root);
    }

    // Right Right Case
    if (balance < -1 && BF(root->right) <= 0)
        return RR(root);

    // Right Left Case
    if (balance < -1 && BF(root->right) > 0)
    {
       return RL(root);
    }
    return root;
}
int max(int x, int y)
{
if(x>y)return x;
else y;
    
}
/******************deletion ***************/
node* delete(node* root, int key, int flag)
{
    if (root == NULL)
    {
     if(flag == 0)
     {
         printf("Node having key = %d doesnot exist.", key);
     }
     return root;
    }

    if (key < root->data)
     {
      root->left = delete(root->left, key, flag);

      if (root == NULL)
      return root;

      root->ht = max(height(root->left), height(root->right)) + 1;

      root =  balance(root);
     }
    else if(key > root->data)
     {
      root->right = delete(root->right, key, flag);

      if (root == NULL)
      return root;

      root->ht = max(height(root->left),height(root->right)) + 1;

      root =  balance(root);
     } 
    else
    {
      flag++;
  /*******************deletion with one child****************/
      if( (root->left == NULL) || (root->right == NULL) )
      {
        node *temp = root->left ? root->left : root->right;

        /**************deletion having no child************/
        if(temp == NULL)
        {
            temp = root;
            root = NULL;
        }
        /***************deletion having one child**********/
        else 
        {
            *root = *temp; 
        }

        free(temp);
      }
      else
      {
        // Node having two child: deletion of inorder Successor (smallest in the right subtree)
        node* temp = Successor(root);
        
        //Handling Duplicate Keys
        
        //If the inorder successor has a different key value  
        if(temp->data != root->data)     
        {
        root->data = temp->data;				//copying successer
        strcpy(root->tagdata, temp->tagdata);
     
        root->right = delete(root->right, temp->data, flag);
        }
        
        //If the inorder successor has the same key value
        else
        { //Deleting the inorder successor 
          root->right = delete(root->right, key, flag);     
          
          //Again performing delete on the previously found node 
          root = delete(root, key, flag);
        }
       //Recursively deleting any other nodes with the same key value in the left as well as right subtree of the node found
       root->left = delete(root->left, key, flag);
       root->right = delete(root->right, key, flag);
    }
    if (root == NULL)
    return root;

    root->ht = max(height(root->left),height(root->right)) + 1;
    root =  balance(root);
  }
    return root;
}

node * insert(node *T,int x,char td[])
{
               if(T==NULL)
               {
                              T=(node*)malloc(sizeof(node));
                              T->data=x;
                              T->left=NULL;
                              T->right=NULL;
				strcpy(T->tagdata,td);
               }
               else
                              if(x > T->data)                // insert in right subtree
                              {
                                             T->right=insert(T->right,x,td);
                                             if(BF(T)==-2)
                                                            if(x>T->right->data)
                                                                           T=RR(T);
                                                            else
                                                                           T=RL(T);
                              }
                              else
                                            
                                             {
                                                            T->left=insert(T->left,x,td);
                                                            if(BF(T)==2)			//
                                                                           if(x < T->left->data)
                                                                                          T=LL(T);
                                                                           else
                                                                                          T=LR(T);
                                             }
                                             T->ht=height(T);
                                             return(T);
}

void inorder(node *T)
{
               if(T!=NULL)
               {
                              inorder(T->left);
                              printf("%d|%d|%s \t",T->data,T->ht,T->tagdata);
                              inorder(T->right);
               }

}
int sTOint(char *s)
{
    int sum=0;
    int j=0;
    for(j=strlen(s)-1;j>=0;j--)
{		
                sum+=(int)pow(10,strlen(s)-j-1)*(s[j]-'0');
	{
                return sum;}

}
}
void search(node* n, int key, int flag)
 {
   if (n == NULL)
    {
     if(flag == 0)
     {
        printf("\nKEY = %d NOT found.", key);
     }
     return;
    }
    if (key < n->data)
        search(n->left, key, flag);
    else if(key > n->data)
        search(n->right, key, flag);
    else if(key == n->data)
       {
        flag++;
        printf("\nKEY = %d found at height = %d with tagdata  = %s ",n->data, n->ht, n->tagdata);
        search(n->right, key, flag);
        search(n->left, key, flag);
       }
}
void main(int arg_n,char* arg[])
{
                                                           node *root = NULL;

  int i, j, k;
  int key;
  char td[100];
  

  for(i=1; i<arg_n; i++)
  {   
    if(strcmp(arg[i], "I") == 0)
    { 
       key =  sTOint(arg[++i]);   
      strcpy(td,arg[++i]);
     root = insert(root, key, td);
     }
    else if(strcmp(arg[i], "S") == 0)
     {
       key = sTOint(arg[++i]);
       search(root, key, 0);         
     }
    else if(strcmp(arg[i], "D") == 0)
     {
       key =  sTOint(arg[++i])
;       
       root = delete(root, key, 0);
       printf("\n Nodes key = %d deleted successfully.\n", key);         
     }
    else if(strcmp(arg[i], "T") == 0)
     {
       printf("\nIn_Order_traversal : key|height|flag data\n");  
       inorder(root);         
     }    
  } 
  printf("\n");

}





