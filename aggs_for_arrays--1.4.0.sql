/* aggs_for_arrays--1.4.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION aggs_for_arrays" to load this file. \quit

CREATE OR REPLACE FUNCTION 
array_to_hist(anyarray, anyelement, anyelement, int)
RETURNS int[]
AS 'aggs_for_arrays', 'array_to_hist'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_hist_2d(anyarray, anyarray, anyelement, anyelement, anyelement, anyelement, int, int)
RETURNS int[]
AS 'aggs_for_arrays', 'array_to_hist_2d'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_mean(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'array_to_mean'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_median(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'array_to_median'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
sorted_array_to_median(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'sorted_array_to_median'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_mode(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'array_to_mode'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
sorted_array_to_mode(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'sorted_array_to_mode'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_percentile(anyarray, float)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'array_to_percentile'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
sorted_array_to_percentile(anyarray, float)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'sorted_array_to_percentile'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_percentiles(anyarray, float[])
RETURNS DOUBLE PRECISION[]
AS 'aggs_for_arrays', 'array_to_percentiles'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
sorted_array_to_percentiles(anyarray, float[])
RETURNS DOUBLE PRECISION[]
AS 'aggs_for_arrays', 'sorted_array_to_percentiles'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_min(anyarray)
RETURNS anyelement
AS 'aggs_for_arrays', 'array_to_min'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_max(anyarray)
RETURNS anyelement
AS 'aggs_for_arrays', 'array_to_max'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_min_max(anyarray)
RETURNS anyarray
AS 'aggs_for_arrays', 'array_to_min_max'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_count(anyarray)
RETURNS integer
AS 'aggs_for_arrays', 'array_to_count'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_skewness(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'array_to_skewness'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
array_to_kurtosis(anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'array_to_kurtosis'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum(anyarray)
RETURNS anyelement
AS 'aggs_for_arrays', 'subarray_to_sum'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum(anyarray, int)
RETURNS anyelement
AS 'aggs_for_arrays', 'subarray_to_sum'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum(anyarray, int, int)
RETURNS anyelement
AS 'aggs_for_arrays', 'subarray_to_sum'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_minidx_gt(anyarray)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_minidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_minidx_gt(anyarray, anyelement)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_minidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_minidx_gt(anyarray, anyelement, int)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_minidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_minidx_gt(anyarray, anyelement, int, int)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_minidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_maxidx_gt(anyarray)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_maxidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_maxidx_gt(anyarray, anyelement)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_maxidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_maxidx_gt(anyarray, anyelement, int)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_maxidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_maxidx_gt(anyarray, anyelement, int, int)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_maxidx_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_count_gt(anyarray)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_count_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_count_gt(anyarray, anyelement)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_count_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_count_gt(anyarray, anyelement, int)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_count_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_count_gt(anyarray, anyelement, int, int)
RETURNS integer
AS 'aggs_for_arrays', 'subarray_to_count_gt'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum_stats(anyarray)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_sum_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum_stats(anyarray, anyelement)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_sum_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum_stats(anyarray, anyelement, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_sum_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_sum_stats(anyarray, anyelement, int, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_sum_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_moments(anyarray)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_moments'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_moments(anyarray, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_moments'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_moments(anyarray, int, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_moments'
LANGUAGE c IMMUTABLE;


CREATE OR REPLACE FUNCTION 
subarray_to_stats(anyarray)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_stats(anyarray, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_stats(anyarray, int, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
subarray_to_stats(anyarray, int, int, int)
RETURNS integer[]
AS 'aggs_for_arrays', 'subarray_to_stats'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
kstest(anyarray, anyarray)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'kstest'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
kstest(anyarray, anyarray, int, int)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'kstest'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
kstest(anyarray, anyarray, int, int, float8)
RETURNS DOUBLE PRECISION
AS 'aggs_for_arrays', 'kstest'
LANGUAGE c IMMUTABLE;

CREATE OR REPLACE FUNCTION 
downsample(anyarray, int)
RETURNS DOUBLE PRECISION[]
AS 'aggs_for_arrays', 'downsample'
LANGUAGE c IMMUTABLE;

