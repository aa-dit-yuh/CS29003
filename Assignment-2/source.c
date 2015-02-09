/* Assignment-2
 * CS29003
 * Name: Aditya Narayan
 * Roll No: 13EC30001
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TABLE_SIZE_MAX 1018
int traceFlag;
int probeCount;

typedef enum TYPE{
	LINEAR=1,
	QUADRATIC,
	DOUBLE,
	CHAINING,
}TYPE; 

typedef enum STATUS{
	FREE=1,
	OCCUPIED,
	DELETED,
}STATUS;

/*------------RETURN TYPE OF EVERY FUNCTION------------*/
typedef enum ERROR_CODE {
	NO_ERROR=0,
	ERROR_INVALID_ARGUMENTS,
	ERROR_TABLE_FULL,
	ERROR_KEY_ALREADY_EXISTS,
	ERROR_KEY_NOT_FOUND,
	ERROR_REHASH_REQ,
	ERROR_INSERT_FAILURE,
}ERROR_CODE;

/*------------------Global prototypes------------------*/
int table_size[TABLE_SIZE_MAX];
typedef struct linkedList linkedList;
typedef struct hashElement hashElement;
typedef struct hashTable hashTable;
typedef union table table;
ERROR_CODE getLoadFactor(hashTable *, float *);
ERROR_CODE initializeHashTable(hashTable *, int, TYPE );
ERROR_CODE probe(hashTable *, int, STATUS *);
ERROR_CODE hashFunction(hashTable *, int, int *);
ERROR_CODE doubleHashFunction(hashTable *, int, int *);
ERROR_CODE search(hashTable *, int, int *);
ERROR_CODE delete(hashTable *, int, int *);
ERROR_CODE rehash(hashTable **);
ERROR_CODE interactive();
ERROR_CODE trace();
int nextPrime(int);
int prevPrime(int);
/* Stores a list of valid hash table sizes. */
int table_size[TABLE_SIZE_MAX] = {31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057, 4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231, 4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583, 4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751, 4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279, 5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639, 5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791, 5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133, 6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221, 6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301, 6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473, 6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673, 6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833, 6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207, 7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411, 7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561, 7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723, 7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919, 7927, 7933, 7937, 7949, 7951, 7963, 7993, 8009, 8011, 8017, 8039, 8053, 8059, 8069, 8081, 8087, 8089, 8093, 8101, 8111, 8117, 8123, 8147, 8161, 8167, 8171, 8179, 8191};

/*----------------The Hash Table struct----------------*/
union table{
	hashElement *slot;
	linkedList **list;
};

struct hashTable{
	/* Stores the bucket size/slots of hash table.*/	
	int bucketSize;
	/* Stores number of elements in the table.*/
	int element;
	/* The buckets/slots.*/
	table bucket;
	/* String that shows the type of hashing involved.*/
	TYPE type;
};

struct hashElement{
	/* Flag to denote if the element is deleted(Support for lazy deletion)*/
	STATUS status;
	/* Stores the key.*/ 
	int key;
};


/*----------------The linkedList struct----------------*/
struct linkedList{
	/* Stores the content. */
	int data;
	/* Points to the next element in the linkedList.*/
	struct linkedList *next;
	/* Points to the previous element in the linkedList.*/
	struct linkedList *prev;
};

/* Saves the load factor at the location specified by pointer alpha.*/
ERROR_CODE getLoadFactor(hashTable *hashTableObject, float *alpha)
{	
	if(hashTableObject==NULL){
		return ERROR_INVALID_ARGUMENTS;
	}
	*alpha=((float)hashTableObject->element)/hashTableObject->bucketSize;
	return NO_ERROR;
}

ERROR_CODE initializeHashTable(hashTable *hashTableObject, int tableSize, TYPE type)
{
	int i;
	if(tableSize<0){
		return ERROR_INVALID_ARGUMENTS;
	}
	hashTableObject->bucketSize=tableSize;
	hashTableObject->element=0;
	hashTableObject->type=type;
	if(type==LINEAR || type==QUADRATIC || type==DOUBLE){
		hashTableObject->bucket.slot=(hashElement *)malloc(sizeof(hashElement)*tableSize);
		for(i=0;i<tableSize;i++){
			hashTableObject->bucket.slot[i].status=FREE;
		}
	}
	else if(type==CHAINING){
		hashTableObject->bucket.list=(linkedList **)malloc(sizeof(linkedList *)*tableSize);
		for(i=0;i<tableSize;i++){
			hashTableObject->bucket.list[i]=NULL;
		}
	}
	return NO_ERROR;
}

ERROR_CODE probe(hashTable *hashTableObject, int bucket, STATUS *checkFlag)
{
	if(bucket>=hashTableObject->bucketSize || bucket<0){
		return ERROR_INVALID_ARGUMENTS;
	}
	if(hashTableObject->type==CHAINING){
		*checkFlag=FREE;
	}
	else{
		*checkFlag=hashTableObject->bucket.slot[bucket].status;
	}
	probeCount++;
	return NO_ERROR;
}

ERROR_CODE hashFunction(hashTable *hashTableObject, int key, int *hashKey)
{
	if(hashTableObject==NULL){
		return ERROR_INVALID_ARGUMENTS;
	}
	int insertQueries, doubleHashKey;
	STATUS checkFlag;
	if(hashTableObject->type==LINEAR){
		*hashKey=key%hashTableObject->bucketSize;
		insertQueries=0;
		while(insertQueries<=hashTableObject->bucketSize){
			if(probe(hashTableObject, (*hashKey+insertQueries)%hashTableObject->bucketSize, &checkFlag)==NO_ERROR && checkFlag==OCCUPIED){
				printf("Collision at slot=%d\n",(*hashKey+insertQueries)%hashTableObject->bucketSize);
				insertQueries++;
			}
			else{
				*hashKey+=insertQueries;
				*hashKey%=hashTableObject->bucketSize;
				return NO_ERROR;
			}
		}
		return ERROR_TABLE_FULL;
	}
	else if(hashTableObject->type==QUADRATIC){
		*hashKey=key%hashTableObject->bucketSize;
		insertQueries=0;
		while(insertQueries<=hashTableObject->bucketSize){
			if(probe(hashTableObject, (*hashKey+insertQueries*insertQueries)%hashTableObject->bucketSize, &checkFlag)==NO_ERROR && checkFlag==OCCUPIED){
				printf("Collision at slot=%d\n",(*hashKey+insertQueries*insertQueries)%hashTableObject->bucketSize);
				insertQueries++;
			}
			else{
				*hashKey+=insertQueries*insertQueries;
				*hashKey%=hashTableObject->bucketSize;
				return NO_ERROR;
			}
		}
		return ERROR_TABLE_FULL;
	}
	else if(hashTableObject->type==DOUBLE){
		*hashKey=key%hashTableObject->bucketSize;
		if(doubleHashFunction(hashTableObject, key, &doubleHashKey)!=NO_ERROR){
			return doubleHashFunction(hashTableObject, key, &doubleHashKey);
		}
		printf("Double hashKey=%d\n", doubleHashKey);
		insertQueries=0;
		while(insertQueries<=hashTableObject->bucketSize){
			if(probe(hashTableObject, (*hashKey+insertQueries*doubleHashKey)%hashTableObject->bucketSize, &checkFlag)==NO_ERROR && checkFlag==OCCUPIED){
				printf("Collision at slot=%d\n",(*hashKey+insertQueries*doubleHashKey)%hashTableObject->bucketSize);
				insertQueries++;
			}
			else{
				*hashKey+=insertQueries*doubleHashKey;
				*hashKey%=hashTableObject->bucketSize;
				return NO_ERROR;
			}
		}
		return ERROR_TABLE_FULL;
	}
	return ERROR_INVALID_ARGUMENTS;
}

ERROR_CODE doubleHashFunction(hashTable *hashTableObject, int key, int *hashKey)
{
	if(hashTableObject==NULL){
		return ERROR_INVALID_ARGUMENTS;
	}
	int r;
	if(hashTableObject->bucketSize==31){
		r=29;
	}
	else{
		r=prevPrime(hashTableObject->bucketSize);
	}
	*hashKey=r-(key%r);
	return NO_ERROR;
}

ERROR_CODE insert(hashTable *hashTableObject, int key, int *hashKey)
{
	float load;
	if(hashTableObject->type==CHAINING){
		*hashKey=key%hashTableObject->bucketSize;
		linkedList *linkedListObjectInsert=(linkedList *)malloc(sizeof(linkedList));
		linkedListObjectInsert->data=key;
		linkedListObjectInsert->next=NULL;
		if(hashTableObject->bucket.list[*hashKey]==NULL){
			  hashTableObject->bucket.list[*hashKey]=linkedListObjectInsert;
			  linkedListObjectInsert->prev=NULL;
			  return NO_ERROR;
		}
		linkedList *iterator=hashTableObject->bucket.list[*hashKey];
		while(iterator->next!=NULL){
			iterator=iterator->next;
			probeCount++;
		}
		iterator->next=linkedListObjectInsert;
		linkedListObjectInsert->prev=iterator;
	}
	else{
		if(hashFunction(hashTableObject, key, hashKey)!=NO_ERROR){
			return hashFunction(hashTableObject, key, hashKey);
		}
		hashTableObject->bucket.slot[*hashKey].key=key;
		hashTableObject->bucket.slot[*hashKey].status=OCCUPIED;
		hashTableObject->element++;
	}
	/* To ensure load does not exceed 0.75. */
	if(getLoadFactor(hashTableObject, &load)==NO_ERROR){
		if(load>0.75){
			return ERROR_REHASH_REQ;
		}
	}
	return NO_ERROR;
}

ERROR_CODE search(hashTable *hashTableObject, int key, int *hashKey)
{
	if(hashTableObject==NULL){
		return ERROR_INVALID_ARGUMENTS;
	}
	int insertQueries, doubleHashKey;
	STATUS checkFlag;
	if(hashTableObject->type==LINEAR){
		*hashKey=key%hashTableObject->bucketSize;
		insertQueries=0;
		while(insertQueries<=hashTableObject->bucketSize){
			if(probe(hashTableObject, (*hashKey+insertQueries)%hashTableObject->bucketSize, &checkFlag)==NO_ERROR){
				if(checkFlag==OCCUPIED || checkFlag==DELETED){
					if(checkFlag==OCCUPIED){
						if(hashTableObject->bucket.slot[(*hashKey+insertQueries)%hashTableObject->bucketSize].key==key){
							*hashKey+=insertQueries;
							*hashKey%=hashTableObject->bucketSize;
							return NO_ERROR;
						}
					}
				}
				else{
					return ERROR_KEY_NOT_FOUND;
				}
			}
			insertQueries++;
		}
		return ERROR_TABLE_FULL;
	}
	else if(hashTableObject->type==QUADRATIC){
		*hashKey=key%hashTableObject->bucketSize;
		insertQueries=0;
		while(insertQueries<=hashTableObject->bucketSize){
			if(probe(hashTableObject, (*hashKey+insertQueries*insertQueries)%hashTableObject->bucketSize, &checkFlag)==NO_ERROR){
				if(checkFlag==OCCUPIED || checkFlag==DELETED){
					if(checkFlag==OCCUPIED){
						if(hashTableObject->bucket.slot[(*hashKey+insertQueries*insertQueries)%hashTableObject->bucketSize].key==key){
							*hashKey+=insertQueries*insertQueries;
							*hashKey%=hashTableObject->bucketSize;
							return NO_ERROR;
						}
					}
				}
				else{
					return ERROR_KEY_NOT_FOUND;
				}
			}
			insertQueries++;
		}
		return ERROR_TABLE_FULL;
	}
	else if(hashTableObject->type==DOUBLE){
		*hashKey=key%hashTableObject->bucketSize;
		if(doubleHashFunction(hashTableObject, key, &doubleHashKey)!=NO_ERROR){
			return doubleHashFunction(hashTableObject, key, &doubleHashKey);
		}
		insertQueries=0;
		while(insertQueries<=hashTableObject->bucketSize){
			if(probe(hashTableObject, (*hashKey+insertQueries*doubleHashKey)%hashTableObject->bucketSize, &checkFlag)==NO_ERROR){
				if(checkFlag==OCCUPIED || checkFlag==DELETED){
					if(checkFlag==OCCUPIED){
						if(hashTableObject->bucket.slot[(*hashKey+insertQueries*doubleHashKey)%hashTableObject->bucketSize].key==key){
							*hashKey+=insertQueries*insertQueries;
							*hashKey%=hashTableObject->bucketSize;
							return NO_ERROR;
						}
					}
				}
				else{
					return ERROR_KEY_NOT_FOUND;
				}
			}
			insertQueries++;
		}
		return ERROR_TABLE_FULL;
	}
	else if(hashTableObject->type==CHAINING){
		*hashKey=key%hashTableObject->bucketSize;
		if(hashTableObject->bucket.list[*hashKey]==NULL){
			return ERROR_KEY_NOT_FOUND;
		}
		linkedList *iterator=hashTableObject->bucket.list[*hashKey];
		while(1){
			if(iterator->data==key){
				return NO_ERROR;
			}
			if(iterator->next==NULL){
				break;
			}
			iterator=iterator->next;
			probeCount++;
		};
		return ERROR_KEY_NOT_FOUND;
	}
	return ERROR_INVALID_ARGUMENTS;
}

ERROR_CODE delete(hashTable *hashTableObject, int key, int *hashKey)
{
	if(search(hashTableObject, key, hashKey)!=NO_ERROR){
		return ERROR_KEY_NOT_FOUND;
	}
	if(hashTableObject->type==CHAINING){
		*hashKey=key%hashTableObject->bucketSize;
		if(hashTableObject->bucket.list[*hashKey]==NULL){
			return ERROR_KEY_NOT_FOUND;
		}
		linkedList *iterator=hashTableObject->bucket.list[*hashKey];
		while(1){
			if(iterator->data==key){
				if(iterator->prev==NULL){
				      hashTableObject->bucket.list[*hashKey]=iterator->next;
				}
				if(iterator->next!=NULL){
				      iterator->next->prev=iterator->prev;
				}
				return NO_ERROR;
			}
			if(iterator->next==NULL){
				break;
			}
			iterator=iterator->next;
			probeCount++;
		};
		return ERROR_KEY_NOT_FOUND;
	}
	else{
		if(hashFunction(hashTableObject, key, hashKey)!=NO_ERROR){
			return hashFunction(hashTableObject, key, hashKey);
		}
		hashTableObject->bucket.slot[*hashKey].status=DELETED;
		hashTableObject->element--;
		return NO_ERROR;
	}
}

ERROR_CODE rehash(hashTable **hashTableObject)
{
	int i,slot;
	hashTable *rehashTableObject;
	rehashTableObject=(hashTable *)malloc(sizeof(hashTable));
	initializeHashTable(rehashTableObject, nextPrime((*hashTableObject)->bucketSize*2), (*hashTableObject)->type);
	for(i=0;i<(*hashTableObject)->bucketSize;i++){
		if((*hashTableObject)->type==CHAINING){
			if((*hashTableObject)->bucket.list[i]!=NULL){
				insert(rehashTableObject, (*hashTableObject)->bucket.slot[i].key, &slot);
				if(!traceFlag){
					printf(
						"Key %d rehash from slot %d to slot %d\n",
						(*hashTableObject)->bucket.slot[i].key,
						i,
						slot);
				}
			}
		}
		else{
			if((*hashTableObject)->bucket.slot[i].status==OCCUPIED){
				insert(rehashTableObject, (*hashTableObject)->bucket.slot[i].key, &slot);
				if(!traceFlag){
					printf(
						"Key %d rehash from slot %d to slot %d\n",
						(*hashTableObject)->bucket.slot[i].key,
						i,
						slot);
				}
			}
		}
	}
	free(*hashTableObject);
	(*hashTableObject)=rehashTableObject;
	return NO_ERROR;
}

ERROR_CODE print(hashTable *hashTableObject)
{
	int i;
	for(i=0;i<hashTableObject->bucketSize;i++){
		if(hashTableObject->type==CHAINING){
			if(hashTableObject->bucket.list[i]!=NULL){
			 	printf("At slot %d\n",i);
				linkedList *iterator=hashTableObject->bucket.list[i];
				while(iterator!=NULL){
					printf("Key=%d\n",iterator->data);
					iterator=iterator->next;
					probeCount++;
				}
			}
		}
		else{
			if(hashTableObject->bucket.slot[i].status==OCCUPIED){
				printf("Key %d exists at slot %d\n", hashTableObject->bucket.slot[i].key, i);
			}
		}
	}
	return NO_ERROR;
}

/* Function name: nextPrime
 * Input parameters: 
 *        1. integer n
 * Purpose: Returns the next prime number greater than n
 */
int nextPrime(int n)
{
	int low=0, high=1017, mid;
	while(1){
		mid= (low+high)/2;
		if(table_size[low]>n){
			return table_size[low];
		}
		if(table_size[mid]== n){
			return table_size[mid+1];
		}
		else if(table_size[mid]< n){
			low= mid+1;
		}
		else if(table_size[mid]> n){
			high= mid-1;
		}
	}
}

/* Function name: prevPrime
 * Input parameters: 
 *        1. integer n
 * Purpose: Returns the prime number lesser than n
 */
int prevPrime(int n)
{
	int low=0, high=1017, mid;
	while(1){
		mid= (low+high)/2;
		if(table_size[high]<n){
			return table_size[high];
		}
		if(table_size[mid]== n){
			return table_size[mid-1];
		}
		else if(table_size[mid]< n){
			low= mid+1;
		}
		else if(table_size[mid]> n){
			high= mid-1;
		}
	}
}


ERROR_CODE interactive()
{
	hashTable *hashTableObject;
	int tableSize, type, op, key, slot;
	while(1){ 
		printf(
			"Enter the type of hashing to be used:\n"
			"1: Linear\n"
			"2. Quadratic\n"
			"3. Double Hashing\n"
			"4. Chaining\n");
		scanf("%d",&type);
		printf(
			"Enter size of table:\n"
			"Program will choose the closest prime number greater than the input\n");
		scanf("%d",&tableSize);
		tableSize=nextPrime(tableSize);
		printf("Creating a hash table of size=%d\n", tableSize);
		hashTableObject=(hashTable *)malloc(sizeof(hashTable));
		if(initializeHashTable(hashTableObject, tableSize, type)!=NO_ERROR){
			puts("Invalid Input");
		}
		else{
			break;
		}
	}
	
	while(1){
		printf(
			"Enter the operation to be performed:\n"
			"1: Insert\n"
			"2. Find\n"
			"3. Delete\n"
			"4. Force Rehash\n"
			"5. Print\n"
			"6. Exit\n");
		scanf("%d",&op);
		if(op>0 && op<7){
			switch(op){
				case 1:
					printf("Enter key to be inserted:\n");
					scanf("%d",&key);
					if(search(hashTableObject, key, &slot)==NO_ERROR){
						printf("Key already exists at %d slot\n", slot);
					}
					else{
						probeCount=0;
						if(insert(hashTableObject, key, &slot)==ERROR_REHASH_REQ){
							printf("Key inserted at %d slot\n", slot);
							printf("The insertion cost=%d\n",probeCount);
							rehash(&hashTableObject);
							printf("The rehashing cost=%d\n",probeCount);
						}
						else{
							printf("Key inserted at %d slot\n", slot);
							printf("The insertion cost=%d\n",probeCount);
						}
					}
					break;
				case 2:
					printf("Enter key to be searched:\n");
					scanf("%d",&key);
					probeCount=0;
					if(search(hashTableObject, key, &slot)==NO_ERROR){
						printf("Key found at %d slot\n", slot);
						printf("Search cost=%d\n",probeCount);
					}
					else{
						puts("Key not found");
					}
					break;
				case 3:
					printf("Enter key to be deleted:\n");
					scanf("%d",&key);
					probeCount=0;
					if(delete(hashTableObject, key, &slot)==NO_ERROR){
						printf("Key deleted from %d slot\n", slot);
						printf("Delete cost=%d\n",probeCount);
					}
					else{
						puts("Key not found");
					}
					break;
				case 4:
					probeCount=0;
					rehash(&hashTableObject);
					printf("Forced rehash cost=%d\n",probeCount);
					break;
				case 5:
					print(hashTableObject);
					break;
				case 6:
					return NO_ERROR;
					break;
			}
		}	
	}
}

ERROR_CODE trace()
{
	hashTable *hashTableObject;
	int i,key, slot, type, tableSize, op, input;
	float alpha;
	hashTableObject=(hashTable *)malloc(sizeof(hashTable));
	while(1){ 
		printf(
			"Enter the type of hashing to be used:\n"
			"1: Linear\n"
			"2. Quadratic\n"
			"3. Double Hashing\n"
			"4. Chaining\n");
		scanf("%d",&type);
		if(type<5 && type>0){
		  break;
		}
		else{
		  puts("Invalid Input");
		}
	}
	printf(
		"Enter size of table:\n"
		"Program will choose the closest prime number greater than the input\n");
	scanf("%d",&tableSize);
	tableSize=nextPrime(tableSize);
	initializeHashTable(hashTableObject, tableSize, type);
	printf("Table size=%d\n", tableSize);
	printf("Enter number of random input to the table:\n");
	scanf("%d",&input);
	while(1){
		printf(
			"Enter the operation to be performed:\n"
			"1: Insert\n"
			"2. Find\n"
			"3. Delete\n"
			"4. Exit\n");
		scanf("%d",&op);
		switch(op){
			case 1:{
				for(i=0;i<input;i++){
					key=rand();
					printf("key = %d\n", key);
					if(search(hashTableObject, key, &slot)!=NO_ERROR){
						probeCount=0;
						if(insert(hashTableObject, key, &slot)==ERROR_REHASH_REQ){
							printf("Key inserted at %d slot\n", slot);
							printf("The insertion cost=%d\n",probeCount);
							getLoadFactor(hashTableObject, &alpha);
							printf("alpha = %f\n", alpha);
							rehash(&hashTableObject);
							printf("The rehashing cost=%d\n",probeCount);
						}
						else{
							printf("Key inserted at %d slot\n", slot);
							printf("The insertion cost=%d\n",probeCount);
						}
					}
					else{
						puts("Key already inserted");
						printf("The insertion cost=%d\n",probeCount);
					}
																	getLoadFactor(hashTableObject, &alpha);
							printf("alpha = %f\n", alpha);
				}
				break;
				}	
			case 2:{
				for(i=0;i<input;i++){  
					key=rand();
					printf("key = %d\n", key);
					probeCount=0;
					if(search(hashTableObject, key, &slot)==NO_ERROR){
						printf("Key found at %d slot\n", slot);
						printf("Search cost=%d\n",probeCount);
					}

					else{
						puts("Not found");
						printf("Search cost=%d\n",probeCount);
					}
																	getLoadFactor(hashTableObject, &alpha);
							printf("alpha = %f\n", alpha);
				  }
				  break;
			}
			case 3:{
				for(i=0;i<input;i++){
					key=rand();
					printf("key = %d\n", key);
					probeCount=0;
					if(delete(hashTableObject, key, &slot)==NO_ERROR){
						printf("Key deleted from %d slot\n", slot);
						printf("Delete cost=%d\n",probeCount);							
						getLoadFactor(hashTableObject, &alpha);
							printf("alpha = %f\n", alpha);
					}
					else{
						puts("Already deleted");
						printf("Delete cost=%d\n",probeCount);
					}
												getLoadFactor(hashTableObject, &alpha);
							printf("alpha = %f\n", alpha);
				}
				break;
			}
			case 4:{
				return NO_ERROR;
				break;
			}
			default:
				puts("Wrong input");
				break;
		}
	}	
	return NO_ERROR;
}

int main()
{
	int input;
	srand(time(NULL));
	setvbuf(stdout, NULL, _IONBF, 0);
	while(1){
		printf(
			"Enter mode of operation:\n"
			"1: Interactive Mode\n"
			"2: Trace Mode\n"
			"3: Exit\n");
		scanf("%d",&input);
		switch(input){
			case 1:{
				traceFlag=0;
				interactive();
				break;
			}
			case 2:{
				traceFlag=1;
				trace();
				break;
			}
			case 3:
			  return 0;
		}
	}
}
