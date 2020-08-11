
Datum kstest(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(kstest);

/**
 * Returns the KS test result two arrays of numbers.
 * by Todd A. Hay
 */
Datum
kstest(PG_FUNCTION_ARGS)
{
  // Our arguments:
  ArrayType *valsA, *valsB;

  // The array element type:
  Oid valsTypeA, valsTypeB;

  // The array element type widths for our input array:
  int16 valsTypeWidthA, valsTypeWidthB;

  // The array element type "is passed by value" flags (not really used):
  bool valsTypeByValueA, valsTypeByValueB;

  // The array element type alignment codes (not really used):
  char valsTypeAlignmentCodeA, valsTypeAlignmentCodeB;

  // The array contents, as PostgreSQL "Datum" objects:
  Datum *valsContentA, *valsContentB;

  // List of "is null" flags for the array contents (not used):
  bool *valsNullFlagsA, *valsNullFlagsB;

  // The size of the input array:
  int na, nb, ia = 0, ib = 0;

  float8 Dn=0, Dcrit, x, na_inv, nb_inv;
  float8 Fa, Fb, c;
  float8 *A, *B;
  int i, nargs;

  nargs = PG_NARGS();
  switch (nargs) {
  case 2:
    c = 1.3580986393225507;     /* KS threshold for 0.05 significance, scipy.special.kolmogi */
    break;
  case 3:
    c = PG_GETARG_FLOAT8(2);
    break;
  default:
    ereport(ERROR, (errmsg("kstest accepts between 2 and 3 arguments")));
  }
  
  if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
    /* ereport(ERROR, (errmsg("Null arrays not accepted"))); */
    PG_RETURN_NULL();
  }

  valsA = PG_GETARG_ARRAYTYPE_P(0);
  valsB = PG_GETARG_ARRAYTYPE_P(1);

  if ((ARR_NDIM(valsA) == 0) || (ARR_NDIM(valsB) == 0)) {
    PG_RETURN_NULL();
  }
  if ((ARR_NDIM(valsA) > 1) || (ARR_NDIM(valsB) > 1)) {
    ereport(ERROR, (errmsg("One-dimesional arrays are required")));
  }

  if (array_contains_nulls(valsA) || array_contains_nulls(valsB)) {
    ereport(ERROR, (errmsg("Arrays contain null elements")));
  }

  // Determine the array element types.
  valsTypeA = ARR_ELEMTYPE(valsA);
  valsTypeB = ARR_ELEMTYPE(valsB);

  if (valsTypeA != valsTypeB) {
    ereport(ERROR, (errmsg("Arrays must have the same type")));
  }

  if (valsTypeA != INT2OID &&
      valsTypeA != INT4OID &&
      valsTypeA != INT8OID &&
      valsTypeA != FLOAT4OID &&
      valsTypeA != FLOAT8OID) {
    ereport(ERROR, (errmsg("KS test subject must be SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION values")));
  }

  na = (ARR_DIMS(valsA))[0];
  nb = (ARR_DIMS(valsB))[0];

  if ((na == 0) || (nb == 0)) PG_RETURN_NULL();

  get_typlenbyvalalign(valsTypeA, &valsTypeWidthA, &valsTypeByValueA, &valsTypeAlignmentCodeA);
  get_typlenbyvalalign(valsTypeB, &valsTypeWidthB, &valsTypeByValueB, &valsTypeAlignmentCodeB);

  // Extract the array contents (as Datum objects).
  deconstruct_array(valsA, valsTypeA, valsTypeWidthA, valsTypeByValueA, valsTypeAlignmentCodeA, &valsContentA, &valsNullFlagsA, &na);
  deconstruct_array(valsB, valsTypeB, valsTypeWidthB, valsTypeByValueB, valsTypeAlignmentCodeB, &valsContentB, &valsNullFlagsB, &nb);

  // Extract array contents
  A = palloc(sizeof(float8) * na);
  B = palloc(sizeof(float8) * nb);

  switch (valsTypeA) {
    case INT2OID:
      for (i = 0; i < na; i++) {
        A[i] = DatumGetInt16(valsContentA[i]);
      }
      for (i = 0; i < nb; i++) {
        B[i] = DatumGetInt16(valsContentB[i]);
      }
      break;
    case INT4OID:
      for (i = 0; i < na; i++) {
        A[i] = DatumGetInt32(valsContentA[i]);
      }
      for (i = 0; i < nb; i++) {
        B[i] = DatumGetInt32(valsContentB[i]);
      }
      break;
    case INT8OID:
      for (i = 0; i < na; i++) {
        A[i] = DatumGetInt64(valsContentA[i]);
      }
      for (i = 0; i < nb; i++) {
        B[i] = DatumGetInt64(valsContentB[i]);
      }
      break;
    case FLOAT4OID:
      for (i = 0; i < na; i++) {
        A[i] = DatumGetFloat4(valsContentA[i]);
      }
      for (i = 0; i < nb; i++) {
        B[i] = DatumGetFloat4(valsContentB[i]);
      }
      break;
    case FLOAT8OID:
      for (i = 0; i < na; i++) {
        A[i] = DatumGetFloat8(valsContentA[i]);
      }
      for (i = 0; i < nb; i++) {
        B[i] = DatumGetFloat8(valsContentB[i]);
      }
      break;
  }

  // sort the arrays
  qsort(A, na, sizeof(float8), compare_float8);
  qsort(B, nb, sizeof(float8), compare_float8);

  // calculate supremum
  na_inv = 1 / (float8)na;
  nb_inv = 1 / (float8)nb;

  while ((ia < na) && (ib < nb)) {
    x = A[ia] < B[ib] ? A[ia] : B[ib];

    while((ia < na) && (A[ia] <= x))
      ia++;
    while((ib < nb) && (B[ib] <= x))
      ib++;

    Fa = ia * na_inv;
    Fb = ib * nb_inv;
    
    Dn = fmax(fabs(Fa - Fb), Dn);
    
  }
  
  pfree(A);
  pfree(B);

  Dcrit = sqrt((float8)(na+nb) / (float8)(na*nb)) * c;
  
  PG_RETURN_FLOAT8(Dn - Dcrit);  /* Dstat - Dcrit < 0 for null hypothesis */
}
