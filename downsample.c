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
  int valsLength, d;
  int i, j;

  float8 sum;

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

  if (valsLength % d != 0) ereport(ERROR, (errmsg("Array length must be multiple of decimation factor")));

  get_typlenbyvalalign(valsType, &valsTypeWidth, &valsTypeByValue, &valsTypeAlignmentCode);

  // Extract the array contents (as Datum objects).
  deconstruct_array(vals, valsType, valsTypeWidth, valsTypeByValue, valsTypeAlignmentCode,
                    &valsContent, &valsNullFlags, &valsLength);

  retContent = palloc0(sizeof(Datum) * valsLength / d);

  
  switch (valsType) {
  case INT2OID:
    for (i = 0; i <= valsLength - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetInt16(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum / d);
    }
    break;
  case INT4OID:
    for (i = 0; i <= valsLength - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetInt32(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum / d);
    }
    break;
  case INT8OID:
    for (i = 0; i <= valsLength - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetInt64(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum / d);
    }
    break;
  case FLOAT4OID:
    for (i = 0; i <= valsLength - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetFloat4(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum / d);
    }
    break;
  case FLOAT8OID:
    for (i = 0; i <= valsLength - d; i += d) {
      sum = 0;
      for (j = 0; j < d; j++){
        sum += DatumGetFloat8(valsContent[i+j]);
      }
      retContent[i/d] = Float8GetDatum(sum / d);
    }
    break;
  }

  get_typlenbyvalalign(FLOAT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
  ret = construct_array(retContent, valsLength/d, FLOAT8OID, retTypeWidth, retTypeByValue, retTypeAlignmentCode);
  PG_RETURN_ARRAYTYPE_P(ret);
}
