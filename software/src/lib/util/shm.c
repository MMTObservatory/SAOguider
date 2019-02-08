/* Handle shared memory in a reasonable way
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xos.h"
#include "xfile.h"
#include "xlist.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <sys/file.h>

typedef struct _ShmRec {
	xList	*next;

	char	*segment;
	int	 length;
	int	 mode;

	int	 shkey;
	int	 shmid;
	void	*shmad;

	int	 shlok;
} ShmRec;

static ShmRec *shmlist = NULL;
static void ShmInsert();
static void ShmDelete();
static ShmRec *ShmLookup();

void *MemSegment(segment, length, mode, Shkey, Shmid)
	char	*segment;
	int	 length;
	int	 mode;
	key_t	*Shkey;
	int	*Shmid;
{
	key_t shkey = strtol(segment, 0, 0);
	int   shmid;
        void *shmad;

   if ( mode ) {
	if ( (shmid = shmget(shkey, length, mode)) < 0 ) {
		fprintf(stderr, "MemSegment: key = 0x%0x: %d ", shkey, length);
		perror(": get id");
		return NULL;
	}

	if ( (shmad = (void *) shmat(shmid, 0, 0)) == (void *) -1 ) {
		perror("MemSegment: attach");
		return NULL;
	}

	ShmInsert(strdup(segment), length, mode, shkey, shmid, shmad);

    	if ( Shmid ) *Shmid = shmid;
    }

    if ( Shkey ) *Shkey = shkey;

    return shmad;
}

void  MemRelease(addr)
	void	*addr;
{
	ShmDelete(addr);
	shmdt(addr);
}

void  MemRemove(segment)
	char	*segment;
{
	int   shkey = strtol(segment, 0,0);

	int   shmid = shmget(shkey, 0, 0);

	shmctl(shmid, IPC_RMID, 0);
}


static void ShmInsert(segment, length, mode, shkey, shmid, shmad)
	char	*segment;
	int	length;
	int	mode;
	key_t	shkey;
	int	shmid;
	void	*shmad;
{
		ShmRec *shmrec;

	New(shmrec, ShmRec);

	shmrec->next = NULL;
	shmrec->segment = segment;
	shmrec->length  = length;
	shmrec->mode	= mode;
	shmrec->shkey	= shkey;
	shmrec->shmid	= shmid;
	shmrec->shmad   = shmad;

	shmrec->shlok	= -1;

	shmlist = ListPush(shmlist, shmrec);
}


static void ShmDelete(addr)
	void *addr;
{
	ShmRec *rec = ShmLookup(addr);

	shmlist = ListDelete(shmlist, rec);
	Free(rec);
}

static ShmRec *ShmLookup(addr)
        void *addr;
{
	ShmRec *rec;

	for ( rec = shmlist; rec; rec = (ShmRec *) rec->next )
		if ( addr == rec->shmad ) break;
	
	return rec;
}

