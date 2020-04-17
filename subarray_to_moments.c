#include <math.h>

Datum subarray_to_moments(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(subarray_to_moments);

/**
 * Returns mean, variance, skewness, and kurtosis from an array of numbers.
 * by Todd Hay
 */
Datum
subarray_to_moments(PG_FUNCTION_ARGS)
{
  // Our arguments:
  ArrayType *vals;
  ArrayType *ret;

  // The array element type:
  Oid valsType;

  // The array element type widths for our input array:
  int16 valsTypeWidth;
  int16 retTypeWidth;
  
  // The array element type "is passed by value" flags (not really used):
  bool valsTypeByValue;
  bool retTypeByValue;
  
  // The array element type alignment codes (not really used):
  char valsTypeAlignmentCode;
  char retTypeAlignmentCode;
  
  // The array contents, as PostgreSQL "Datum" objects:
  Datum *valsContent;
  Datum *retContent;

  // List of "is null" flags for the array contents (not used):
  bool *valsNullFlags;

  // The size of the input array:
  int valsLength, nargs;
  int startIndex, endIndex;         // start and end indices

  bool resultIsNull = true;

  int i;

  double n=0, mean=0, M2=0, M3=0, M4=0, n1;
  double x, delta, delta_n, delta_n2, term1;

  if (PG_ARGISNULL(0)) {
    ereport(ERROR, (errmsg("Null arrays not accepted")));
  }

  vals = PG_GETARG_ARRAYTYPE_P(0);

  if (ARR_NDIM(vals) == 0) {
    PG_RETURN_NULL();
  }
  if (ARR_NDIM(vals) > 1) {
    ereport(ERROR, (errmsg("One-dimesional arrays are required")));
  }

  if (array_contains_nulls(vals)) {
    ereport(ERROR, (errmsg("Array contains null elements")));
  }

  // Determine the array element types.
  valsType = ARR_ELEMTYPE(vals);

  valsLength = (ARR_DIMS(vals))[0];

  if (valsLength == 0) PG_RETURN_NULL();

  // Get start and end indices
  nargs = PG_NARGS();
  switch (nargs) {
  case 1:
    startIndex = 1;
    endIndex = valsLength;
    break;
  case 2:
    startIndex = PG_GETARG_INT32(1);
    endIndex = valsLength;
    break;
  case 3:
    startIndex = PG_GETARG_INT32(1);
    endIndex = PG_GETARG_INT32(2);
    break;
  default:
    ereport(ERROR, (errmsg("Too many arguments")));
  }

  if (endIndex > valsLength) ereport(ERROR, (errmsg("Stop index must be less than or equal to vector length")));

  if (startIndex < 1) ereport(ERROR, (errmsg("Start index must be greater than or equal to 1")));

  if (startIndex > endIndex) PG_RETURN_NULL();

  get_typlenbyvalalign(valsType, &valsTypeWidth, &valsTypeByValue, &valsTypeAlignmentCode);

  // Extract the array contents (as Datum objects).
  deconstruct_subarray(vals, valsType, valsTypeWidth, valsTypeByValue, valsTypeAlignmentCode,
                       &valsContent, &valsNullFlags, &valsLength, startIndex - 1, endIndex);

  
  switch (valsType) {
  case INT2OID:
  case INT4OID:
  case INT8OID:
  case FLOAT4OID:
  case FLOAT8OID:
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}

      n1 = n;
      n = n + 1;

      switch (valsType) {
      case INT2OID: x = DatumGetInt16(valsContent[i]); break;
      case INT4OID: x = DatumGetInt32(valsContent[i]); break;
      case INT8OID: x = DatumGetInt64(valsContent[i]); break;
      case FLOAT4OID: x = DatumGetFloat4(valsContent[i]); break;
      case FLOAT8OID: x = DatumGetFloat8(valsContent[i]); break;
      default: elog(ERROR, "Unknown valsType!");
      }
      
      delta = x - mean;
      delta_n = delta / n;
      delta_n2 = delta_n * delta_n;
      term1 = delta * delta_n * n1;
      mean = mean + delta_n;
      M4 = M4 + term1 * delta_n2 * (n*n - 3*n + 3) + 6 * delta_n2 * M2 - 4 * delta_n * M3;
      M3 = M3 + term1 * delta_n * (n - 2) - 3 * delta_n * M2;
      M2 = M2 + term1;
    }
    break;
  default:
    ereport(ERROR, (errmsg("subarray_to_moments subject must be SMALLINT, INTEGER, BIGINT, REAL or DOUBLE PRECISION valued")));
  }

  retContent = palloc0(sizeof(Datum) * 4);
  retContent[0] = Float8GetDatum(mean);         /* mean */
  retContent[1] = Float8GetDatum(M2 / n); /* var */
  retContent[2] = Float8GetDatum(sqrt(n) * M3 / pow(M2, 1.5)); /* skewness */
  retContent[3] = Float8GetDatum((n * M4) / (M2 * M2) - 3);    /* kurtosis */

  if (resultIsNull) {
    PG_RETURN_NULL();
  }else {
    get_typlenbyvalalign(FLOAT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
    ret = construct_array(retContent, 4, FLOAT8OID, retTypeWidth, retTypeByValue, retTypeAlignmentCode);
    PG_RETURN_ARRAYTYPE_P(ret);
  }
}
