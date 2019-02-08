/* list.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xlist.h"

/* Insert member in front of at
 */
void           *_ListInsert(list, member, at)
    xList           *list;
    xList           *member;
    xList           *at;
{
    xList           *this;
    xList           *prev = NULL;

    xList           *mlast;

    for (this = list; this != at; this = this->next)
	prev = this;

    mlast = member;
    if (mlast)
	while (mlast->next != NULL)
	    mlast = mlast->next;

    if (prev) {
	prev->next = member;
	mlast->next = this;
    } else {
	if (member) {
	    mlast->next = list;
	    list = member;
	};
    }

    return (void *) list;
}


void           *_ListDelete(list, member)
    xList           *list;
    xList           *member;
{
    xList           *this;
    xList           *prev = NULL;

    for (this = list; this && this != member; this = this->next)
	prev = this;

    if (prev)
	prev->next = this->next;
    else
	if ( this ) list = this->next;

    if ( member ) member->next = NULL;

    return (void *) list;
}


void _ListFree(list, proc)
     xList      *list;
     void     (*proc)();
{

  while ( list ) {
    xList	*next = list->next;

    (*proc)(list);
    list = next;
  }
}


int   _ListNth(list, i, data)
		xList	*list;
		int	 i;
		int	*data;
{
	return i != *data;
}


void *_ListLookup(list, data, compare)
	xList	*list;
	void	*data;
	int	(*compare)();
{
		int	i;

	for ( i = 0; list; list = list->next, i++ )
		if ( !(*compare)(list, i, data) ) return list;

	return NULL;
}


void *_ListSortInsert(list, member, compare)
	xList    *list;
	xList    *member;
	int     (*compare)();
{
		xList	*l;
		int	 i;

	if ( list == NULL ) return member;

	for ( l = list,    i = 0; 
	      l && (*compare)(l, i, member) <= 0; 
	      l = l->next, i++ );

	return ListInsert(list, member, l);
}


