/* range.h
 */
#ifndef RANGE_H
#define RANGE_H


typedef struct _Range {
	off_t	start;
	off_t	end;
} Range;

#define ARange(range)		(range->end)
#define NRange(range)		(range->start)
#define NthRange(range, n)	(range[(n) + 1])


#ifdef __STDC__
Range *range_add(Range *range, off_t start, off_t end);
Range *range_union(Range *range);
Range *range_inter(Range *r1, Range *r2);
void   range_print(File, Range *r);
Range *range_invrt(Range *r, off_t min, off_t max);
Range *range_copy(Range *r);
Range *range_cat(Range *r1, Range *r2);
#else
Range *range_add();
Range *range_union();
Range *range_inter();
Range *range_invrt();
void   range_print();
Range *range_copy();
Range *range_cat();
#endif

#endif
