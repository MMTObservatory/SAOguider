/* cache.h
 */

typedef struct _CNode {
	struct	_CNode *next;
	int		hash;
	struct	_CNode *list;
	struct	_CNode *prev;
} *CNode;

typedef struct _Cache {
	CNode  *cache;
	int	size;
	int	width;
	int	n;
	int	max;
	void  (*freenode)();
	void   *data;
	CNode	list;
	CNode	last;
	int   (*hashcomp)();
} *Cache;


Cache cache_init();
CNode cache_hashe();
CNode cache_cache();

