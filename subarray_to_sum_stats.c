
Datum subarray_to_sum_stats(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(subarray_to_sum_stats);

/**
 * Returns a sum from an array of integers, indicies of first and last bin > 0, and number of bins > 0.
 * by Todd Hay
 */
Datum
subarray_to_sum_stats(PG_FUNCTION_ARGS)
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
  int count;
  int minIndex = 0x7FFFFFFF;
  int maxIndex = -1;
  int compareValue, sum, binValue, idx;

  bool resultIsNull = true;

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
  case 4:
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
  case INT4OID:
    compareValue = nargs > 3 ? PG_GETARG_INT32(3) : 0;
    sum = 0;
    count = 0;
    for (i = 0; i < valsLength; i++) {
      if (valsNullFlags[i]) {
        continue;
      }else if (resultIsNull) {
        resultIsNull = false;}
      binValue = DatumGetInt32(valsContent[i]);
      sum += binValue;
      if (binValue > compareValue) {
        idx = i + startIndex;
        count += 1;
        if (idx > maxIndex)
          maxIndex = idx;
        if (idx < minIndex)
          minIndex = idx;
      }
    }
    /* if (minIndex == startIndex + valsLength) */
    /*   minIndex = 0; */
    break;
  default:
    ereport(ERROR, (errmsg("Sum stats subject must be INTEGER valued")));
  }

  retContent = palloc0(sizeof(Datum) * 4);
  retContent[0] = Int32GetDatum(sum);
  retContent[1] = Int32GetDatum(minIndex);
  retContent[2] = Int32GetDatum(maxIndex);
  retContent[3] = Int32GetDatum(count);

  if (resultIsNull) {
    PG_RETURN_NULL();
  }else {
    get_typlenbyvalalign(INT4OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
    ret = construct_array(retContent, 4, INT4OID, retTypeWidth, retTypeByValue, retTypeAlignmentCode);
    PG_RETURN_ARRAYTYPE_P(ret);
  }
}
