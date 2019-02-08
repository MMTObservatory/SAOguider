/* cache.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xos.h"
#include "xfile.h"
#include "cache.h"

Cache	cache_init(size, width, max, freenode, data, hashcomp)
	int	 size;
	int	 width;
	int	 max;
	void   (*freenode)();
	void	*data;
	int    (*hashcomp)();
{
	Cache	c;

	Calloc(c, sizeof(struct _Cache));
	Calloc(c->cache, width * sizeof(CNode));

	c->size		= size;
	c->width	= width;
	c->max 		= max;
	c->freenode 	= freenode;
	c->data		= data;
	c->n		= 0;
	c->hashcomp	= hashcomp;

	return c;
}


CNode cache_hashe(c, hash, prev)
	Cache    c;
	CNode	 hash;
	CNode	*prev;
{
		unsigned x = hash->hash % c->width;
		CNode h;
		CNode p;

	if ( prev == NULL ) prev = &p;
	*prev = NULL;

	for ( h = c->cache[x]; h != NULL; *prev = h, h = h->next ) {
		/* FPrint(Stderr, "Check: %6d\n", hash); 	*/
		/* FPrint(Stderr, "     : %6d\n", h->hash); 	*/
	    if ( (*c->hashcomp)(h, hash, c->data) ) {
 /* FPrint(Stderr, "Hit  : %6d\n", hash->hash);  */
		return h;
	    }
	}

 /* FPrint(Stderr, "Miss : %6d\n", hash->hash);  */
	return NULL;
}

CNode cache_cache(c, hash)
	Cache	 c;
	CNode	 hash;
{
		CNode	 h = NULL;
		CNode	 p;
		unsigned x = hash->hash % c->width;
		int	i;


/* 	FPrint(Stderr, "Cache: %d %d %d\n", hash->hash, ((RowCache) hash)->star, ((RowCache) hash)->ends);

	for ( i = 0; i < c->width; i++ ) {
	    p = c->cache[i];
	    while ( p != NULL ) {
		FPrint(Stderr, "Table: %d %d %d - %d\n", p->hash, ((RowCache) p)->star, ((RowCache) p)->ends, i); 

	       p = p->next;
	    }
	}
	FPrint(Stderr, "\n");
	p = c->list;
	while ( p != NULL ) {
	    FPrint(Stderr, "List : %d %d %d\n", p->hash, ((RowCache) p)->star, ((RowCache) p)->ends);

	   p = p->list;
	}
	if ( c->last != NULL )
	    FPrint(Stderr, "Last : %d %d %d\n\n", c->last->hash, ((RowCache) c->last)->star, ((RowCache) c->last)->ends);
 */

	if ( c->max && c->n >= c->max ) {
		unsigned y = c->last->hash % c->width;

	    if ( (h = cache_hashe(c, c->last, &p)) == NULL ) abort();

	    if ( p != NULL ) p->next 	   = h->next;	/* Unlink the hash list */
	    else	     c->cache[y]   = h->next;

	    c->last = c->last->prev;

	    if ( c->last != NULL )
		c->last->list = NULL;
	    else
 	        c->list = NULL;

	    h->prev = NULL;

	    if ( c->freenode != NULL )
	    	(*c->freenode)(h, c->data);
	}

	if ( h == NULL ) {
	    Calloc(h, c->size);
	    c->n++;
	}

	h->hash = hash->hash;

	h->next = c->cache[x];				/* Link the hash list	*/
	c->cache[x] = h;

	h->list 	  = c->list;			/* Link the LRU list	*/

	if ( c->list )  c->list->prev = h;
	else		c->last       = h;

	c->list		  = h;

	return h;
}
