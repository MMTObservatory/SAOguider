/* range.c
 */
/* Starbase Data Tables - An Ascii Database for UNIX
 
   copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include <xos.h>
#include <xfile.h>
#include "range.h"

static int cmp_starts(v1, v2)
	const void *v1;
	const void *v2;
{
	Range	*r1;
	Range	*r2;

	r1 = (Range *) v1;
	r2 = (Range *) v2;
	
	return r1->start < r2->start ? -1 : (r1->start > r2->start ? 1 : 0);
}

Range *range_add(range, start, end)
	Range	*range;
	off_t	 start;
	off_t	 end;
{
		int	a, n, r;

	if ( range == NULL ) {
		a = 0;
		n = 0;
	} else {
		a = ARange(range);
		n = NRange(range);
	}

	/* These statements optimize extension of the 
	 * first or last range.
	 */
	for ( r = 1; range && r <= NRange(range); r += NRange(range) ) {
	    if ( start >= NthRange(range, r-1).start
	      && start <= NthRange(range, r-1).end ) {
		if ( end > NthRange(range, r-1).end ) {
		     NthRange(range, r-1).end = end;
		}
		return range;
	    }
	    if ( end >= NthRange(range, r-1).start
	      && end <= NthRange(range, r-1).end   ) {
		if ( start < NthRange(range, r-1).start ) {
		     NthRange(range, r-1).start  = start;
		}
		return range;
	    }
	}

	if ( a <= n ) {
		a *= 1.5;
		a += 10;
		ReAlloc(range, (a + 1) * sizeof(Range));
		ARange(range) = a;
		NRange(range) = n;
	}

	NthRange(range, NRange(range)).start = start;
	NthRange(range, NRange(range)).end   = end;
	NRange(range)++;

	return range;
}

void range_print(file, range)
	File	 file;
	Range	*range;
{
	int	i;

	for ( i = 0; i < NRange(range); i++ ) {
		FPrint(file, "%d	%d\n"
			, NthRange(range, i).start
		        , NthRange(range, i).end
			, NthRange(range, i).end
		        - NthRange(range, i).start);
	}
}

Range *range_invrt(range, min, max)
	Range	*range;
	off_t	 min;
	off_t	 max;
{
	Range	*r0, *r1, *rN;
	off_t	 start;

	if ( range == NULL ) {
	    return range_add(range, min, max);
	}

	range = range_union(range);

	r0 = &NthRange(range, 0);
	r1 = &NthRange(range, 1);
	rN = &NthRange(range, NRange(range)-1);
	
	start = r0->start;

	while ( r0 < rN ) {
	    r0->start = r0->end;
	    r0->end   = r1->start;
	    r0++;
	    r1++;
	}

	if ( rN->end < max ) {
	     rN->start = rN->end;
	     rN->end   = max;
	} else {
	    NRange(range)--;
	}
	if ( start > min ) {
	    range = range_add(range, min, start);
	    r0 = &NthRange(range, 0);
	    qsort(r0, NRange(range), sizeof(Range), cmp_starts);
	}

	return range;
}

Range *range_union(range)
	Range	*range;
{
	Range	*r0, *r1;

	if ( range == NULL      ) return NULL;
	if ( NRange(range) <= 1 ) return range;

	r0 = &NthRange(range, 0);
	r1 = &NthRange(range, 1);
	
	qsort(r0, NRange(range), sizeof(Range), cmp_starts);

        while ( r1 < &NthRange(range, NRange(range)) ) {
            if (r1->start - r0->end > 1) {
		r0++;
		if ( r0 != r1 ) {
			r0->start = r1->start;
			r0->end   = r1->end;
		}
		r1++;
            } else {
              /* if the second range is longer, copy to first range */
                if (r1->end > r0->end)
            	     r0->end = r1->end;
          	r1++;
            }
        }

        NRange(range) = (r0 - &NthRange(range, 0))  + 1;
        ARange(range) = NRange(range);
	ReAlloc(range, (ARange(range) + 1) * sizeof(Range));

	return range;
}

Range *range_inter(range0, range1)
	Range	*range0;
	Range	*range1;
{
		Range	*r0, *r1, *r2 = NULL;

	if ( range0 == NULL ) {
	    if ( range1 ) Free(range1);
	    return NULL;
	}

	if ( range1 == NULL ) {
	    if ( range0 ) Free(range0);
	    return NULL;
	}

	qsort(&NthRange(range0, 0), NRange(range0), sizeof(Range), cmp_starts);
	qsort(&NthRange(range1, 0), NRange(range1), sizeof(Range), cmp_starts);

	r0 = &NthRange(range0, 0);
	r1 = &NthRange(range1, 0);

	while ( r0 < &NthRange(range0, NRange(range0))
	     && r1 < &NthRange(range1, NRange(range1)) ) {
		if ( ( r0->start <= r1->end   )
		  && ( r0->end   >= r1->start ) ) {
			r2 = range_add(r2
				, Max(r0->start, r1->start)
				, Min(r0->end  , r1->end  ));
		}
		if ( r0->end < r1->end ) r0++;
		else			 r1++;
	}

	Free(range0);
	Free(range1);

	return r2;
}


Range *range_copy(r)
	Range *r;
{
	Range	*R;

	if ( !r ) return NULL;

	Malloc(R, (NRange(r)  + 1) * sizeof(Range));
	memcpy(R, r, (NRange(r) + 1) * sizeof(Range));

	return R;
}

Range *range_cat(r1, r2)
	Range	*r1;
	Range	*r2;
{
	int	a;
	if ( r1 == NULL ) return r2;
	if ( r2 == NULL ) return r1;

 	a = NRange(r1) + NRange(r2);

	ReAlloc(r1, (a + 1) * sizeof(Range));
	memcpy(&NthRange(r1, NRange(r1))
	     , &NthRange(r2, 0)
	     , NRange(r2) * sizeof(Range));
	ARange(r1) = a;
	NRange(r1) = a;

	Free(r2);
	return r1;
}
