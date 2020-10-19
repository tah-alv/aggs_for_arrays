#include <math.h>

Datum downsample(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(downsample);

/**
 * Returns a sum from an array of integers, indicies of first and last bin > 0, and number of bins > 0.
 * by Todd Hay
 */
Datum
downsample(PG_FUNCTION_ARGS)
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
  int valsLength, d, nargs;
  int i, j;
  int startIndex, endIndex, numElements;         // start and end indices
  bool do_avg;

  float8 sum, divisor;

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
  if (valsType != INT2OID &&
      valsType != INT4OID &&
      valsType != INT8OID &&
      valsType != FLOAT4OID &&
      valsType != FLOAT8OID) {
    ereport(ERROR, (errmsg("Downsample subject must be SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION values")));
  }

  valsLength = (ARR_DIMS(vals))[0];

  if (valsLength == 0) PG_RETURN_NULL();

  // Get decimation factor
  d = PG_GETARG_INT32(1);
  if (d == 1)  PG_RETURN_ARRAYTYPE_P(vals);

  // Get decimation mode
  nargs = PG_NARGS();
  switch (nargs) {
  case 2:                       
    startIndex = 1;
    endIndex = valsLength;
    do_avg = false;
    break;
  case 3:                       /* maybe average bins */
    startIndex = PG_GETARG_INT32(2);
    endIndex = valsLength;
    do_avg = false;
    break;
  case 4:                       /* maybe average bins and set startIndex*/
    startIndex = PG_GETARG_INT32(2);
    endIndex = PG_GETARG_INT32(3);
    do_avg = false;
    break;
  case 5:                       /* maybe average bins and set startIndex / endIndex*/
    startIndex = PG_GETARG_INT32(2);
    endIndex = PG_GETARG_INT32(3);
    do_avg = PG_GETARG_BOOL(4);
    break;
  default:
    ereport(ERROR, (errmsg("Too many arguments")));
  }

  divisor = do_avg ? 1.0/(float8)d : 1.0;

  numElements = endIndex - (startIndex - 1);
  
  if (numElements % d != 0) ereport(ERROR, (errmsg("Array length must be multiple of decimation factor")));

  get_typlenbyvalalign(valsType, &valsTypeWidth, &valsTypeByValue, &valsTypeAlignmentCode);

  // Extract the array contents (as Datum objects).
  deconstruct_subarray(vals, valsType, valsTypeWidth, valsTypeByValue, valsTypeAlignmentCode,
                       &valsContent, &valsNullFlags, &valsLength, startIndex - 1, endIndex);

  retContent = palloc0(sizeof(Datum) * numElements / d);

  switch (valsType) {
  case INT2OID:
    for (i = 0; i <= numElements - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetInt16(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum * divisor);
    }
    break;
  case INT4OID:
    for (i = 0; i <= numElements - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetInt32(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum * divisor);
    }
    break;
  case INT8OID:
    for (i = 0; i <= numElements - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetInt64(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum * divisor);
    }
    break;
  case FLOAT4OID:
    for (i = 0; i <= numElements - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetFloat4(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum * divisor);
    }
    break;
  case FLOAT8OID:
    for (i = 0; i <= numElements - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetFloat8(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum * divisor);
    }
    break;
  }

  get_typlenbyvalalign(FLOAT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
  ret = construct_array(retContent, numElements/d, FLOAT8OID, retTypeWidth, retTypeByValue, retTypeAlignmentCode);
  PG_RETURN_ARRAYTYPE_P(ret);
}
