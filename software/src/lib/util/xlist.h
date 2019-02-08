/* list.h
**/

#ifndef XLIST_H
#define XLIST_H

#ifndef NULL
#define NULL ((void *) 0)
#endif


typedef struct _xList {
    struct _xList   *next;
}               xList;

void           *ListDelete();
void           *ListInsert();

#define ListPush(list, member)	ListInsert(list, member, list)
#define ListPop(list)		ListDelete(list, list)
#define ListAdd(list, member)	ListInsert(list, member, NULL)

#define ListNext(list)		((void *)((((xList *) list)->next)))

#define ListInsert(l, m, at)	_ListInsert((xList *) l, (xList *) m, (xList *) at)
#define ListSortInsert(l, m, c)	_ListSortInsert((List *) l, (xList *) m, c)
#define ListDelete(l, m)	_ListDelete((xList *) l, (xList *) m)
#define ListLookup(l, m, c)	_ListLookup((xList *) l, (void *) m, c)
#define ListFree(l, proc)	_ListFree((xList*)l, proc);

#define ListNth			_ListNth

#ifdef __STDC__
int 	_ListNth();
void   *_ListInsert(xList *list, xList *member, xList *at);
void   *_ListSortInsert(xList *list, xList *member, int (*compare)());
void   *_ListDelete(xList *list, xList *member);
void   *_ListLookup(xList *list, void *data, int (*compare)());
#else
int 	_ListNth();
void   *_ListInsert();
void   *_ListSortInsert();
void   *_ListDelete();
void   *_ListLookup();
#endif


#endif
