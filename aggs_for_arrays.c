#include <postgres.h>
#include <access/tupmacs.h>
#include <fmgr.h>
#include <utils/array.h>
#include <catalog/pg_type.h>
#include <utils/lsyscache.h>
#include <math.h>

PG_MODULE_MAGIC;

#include "util.c"
#include "array_to_hist.c"
#include "array_to_hist_2d.c"
#include "array_to_mean.c"
#include "array_to_median.c"
#include "sorted_array_to_median.c"
#include "array_to_mode.c"
#include "sorted_array_to_mode.c"
#include "array_to_max.c"
#include "array_to_min.c"
#include "array_to_min_max.c"
#include "array_to_count.c"
#include "array_to_percentile.c"
#include "sorted_array_to_percentile.c"
#include "array_to_percentiles.c"
#include "sorted_array_to_percentiles.c"
#include "array_to_skewness.c"
#include "array_to_kurtosis.c"
#include "subarray_to_sum.c"
#include "subarray_to_minidx_gt.c"
#include "subarray_to_maxidx_gt.c"
#include "subarray_to_count_gt.c"
#include "subarray_to_sum_stats.c"
#include "subarray_to_moments.c"
#include "subarray_to_stats.c"
#include "kstest.c"
