
typedef union pgnum {
  int16 i16;
  int32 i32;
  int64 i64;
  float4 f4;
  float8 f8;
} pgnum;

/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code adapted from Public domain code by Nicolas Devillard - 1998 from here:
 *  http://ndevilla.free.fr/median/median/index.html
 */

#define ELEM_SWAP(a,b) { register float8 t=(a);(a)=(b);(b)=t; }

float8 select_kth_value(float8 *arr, int n, int k);

float8 select_kth_value(float8 *arr, int n, int k) {
  int low, high;
  int middle, ll, hh;

  low = 0;
  high = n-1;

  for (;;) {
    // One element only
    if (high <= low) return arr[k];

    // Two elements only
    if (high == low + 1) {
      if (arr[low] > arr[high]) ELEM_SWAP(arr[low], arr[high]);
      return arr[k];
    }

    // Find median of low, middle and high items; swap into position low
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

    // Swap low item (now in position middle) into position (low+1)
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    // Nibble from each end towards middle, swapping items when stuck
    ll = low + 1;
    hh = high;
    for (;;) {
      do ll++; while (arr[low] > arr[ll]);
      do hh--; while (arr[hh]  > arr[low]);

      if (hh < ll) break;

      ELEM_SWAP(arr[ll], arr[hh]);
    }

    // Swap middle item (in position low) back into correct position
    ELEM_SWAP(arr[low], arr[hh]);

    // Re-set active partition
    if (hh <= k) low = ll;
    if (hh >= k) high = hh - 1;
  }
}

#undef ELEM_SWAP



typedef struct {
  float8 value;
  int count;
} valcount;

static int compare_float8(const void *a, const void *b);
static int compare_float8(const void *a, const void *b) {
  double x = *(const double*)a - *(const double*)b;
  return x < 0 ? -1 : x > 0;
}


/* int compare_float8(const void *a, const void *b); */
int compare_valcount(const void *a, const void *b);

/* int compare_float8(const void *a, const void *b) { */
/*   float8 x = *(const float8*)a - *(const float8*)b; */
/*   return x < 0 ? -1 : x > 0; */
/* } */

int compare_valcount(const void *a, const void *b) {
  return ((const valcount*)b)->count - ((const valcount*)a)->count;
}

float8 fmax(float8 a, float8 b) {
  return (a > b) ? a : b;
}

void
deconstruct_subarray(ArrayType *array,
                     Oid elmtype,
                     int elmlen, bool elmbyval, char elmalign,
                     Datum **elemsp, bool **nullsp, int *nelemsp,
                     int startIndex, int endIndex);

void
deconstruct_subarray(ArrayType *array,
                     Oid elmtype,
                     int elmlen, bool elmbyval, char elmalign,
                     Datum **elemsp, bool **nullsp, int *nelemsp,
                     int startIndex, int endIndex)
{
  Datum      *elems;
  bool       *nulls;
  int         nelems;
  char       *p;
  bits8      *bitmap;
  int         bitmask;
  int         i,j;

  Assert(ARR_ELEMTYPE(array) == elmtype);

  nelems = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));

  Assert(endIndex - startIndex <= nelems);
  nelems = endIndex - startIndex;

  *elemsp = elems = (Datum *) palloc(nelems * sizeof(Datum));
  if (nullsp)
    *nullsp = nulls = (bool *) palloc0(nelems * sizeof(bool));
  else
    nulls = NULL;

  *nelemsp = nelems;

  p = ARR_DATA_PTR(array);
  bitmap = ARR_NULLBITMAP(array);
  bitmask = 1;

  for (i = 0; i < startIndex; i++) {
    p = att_addlength_pointer(p, elmlen, p);
    p = (char *) att_align_nominal(p, elmalign);

    /* advance bitmap pointer if any */
    if (bitmap) {
      bitmask <<= 1;
      if (bitmask == 0x100) {
        bitmap++;
        bitmask = 1;
      }
    }
  }

  for (i = startIndex; i < endIndex; i++)
  {
    j = i-startIndex;
    /* Get source element, checking for NULL */
    if (bitmap && (*bitmap & bitmask) == 0) {
      elems[j] = (Datum) 0;
      if (nulls)
        nulls[j] = true;
      else
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("null array element not allowed in this context")));
    }
    else {
      elems[j] = fetch_att(p, elmbyval, elmlen);
      p = att_addlength_pointer(p, elmlen, p);
      p = (char *) att_align_nominal(p, elmalign);
    }

    /* advance bitmap pointer if any */
    if (bitmap) {
      bitmask <<= 1;
      if (bitmask == 0x100) {
        bitmap++;
        bitmask = 1;
      }
    }
  }
}
