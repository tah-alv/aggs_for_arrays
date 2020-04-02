
Datum subarray_to_sum(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(subarray_to_sum);

/**
 * Returns a sum from an array of numbers.
 * by Todd Hay
 */
Datum
subarray_to_sum(PG_FUNCTION_ARGS)
{
  // Our arguments:
  ArrayType *vals;

  // The array element type:
  Oid valsType;

  // The array element type widths for our input array:
  int16 valsTypeWidth;

  // The array element type "is passed by value" flags (not really used):
  bool valsTypeByValue;

  // The array element type alignment codes (not really used):
  char valsTypeAlignmentCode;

  // The array contents, as PostgreSQL "Datum" objects:
  Datum *valsContent;

  // List of "is null" flags for the array contents (not used):
  bool *valsNullFlags;

  // The size of the input array:
  int valsLength, nargs;
  int startIndex, endIndex;         // start and end indices

  bool resultIsNull = true;

  pgnum v;
  int i;

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
    v.i32 = 0;
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}
      v.i32 += (int32)DatumGetInt16(valsContent[i]);
    }
    if (resultIsNull) PG_RETURN_NULL();
    else PG_RETURN_INT32(v.i32);
  case INT4OID:
    v.i64 = 0;
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}
      v.i64 += (int64)DatumGetInt32(valsContent[i]);
    }
    if (resultIsNull) PG_RETURN_NULL();
    else {
      PG_RETURN_INT64(v.i64);}
  case INT8OID:
    v.i64 = 0;
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}
      v.i64 += DatumGetInt64(valsContent[i]);
    }
    if (resultIsNull) PG_RETURN_NULL();
    else {
      PG_RETURN_INT64(v.i64);}
  case FLOAT4OID:
    v.f4 = 0.0;
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}
      v.f4 += DatumGetFloat4(valsContent[i]);
    }
    if (resultIsNull) PG_RETURN_NULL();
    else PG_RETURN_FLOAT4(v.f4);
  case FLOAT8OID:
    v.f8 = 0.0;
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}
      v.f8 += DatumGetFloat8(valsContent[i]);
    }
    if (resultIsNull) PG_RETURN_NULL();
    else PG_RETURN_FLOAT8(v.f8);
  default:
    ereport(ERROR, (errmsg("Sum subject must be SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION values")));
  }
}
