load test_helper

@test "kstest full array balanced" {
    result="$(query "SELECT kstest (ARRAY[0.58, 0.42, 0.52, 0.33, 0.43, 0.23, 0.58, 0.76, 0.53, 0.64]::DOUBLE PRECISION[],
    ARRAY[0.0, 0.11111111, 0.22222222, 0.33333333, 0.44444444, 0.55555556, 0.66666667, 0.77777778, 0.88888889, 1.0]::DOUBLE PRECISION[])")";
    echo $result;
    [ "$result" = "-1.6206415147703976" ]
}

@test "kstest full array unbalanced" {
    result="$(query "SELECT kstest (ARRAY[0.58, 0.42, 0.52, 0.33, 0.43, 0.23, 0.58, 0.76, 0.53, 0.64]::DOUBLE PRECISION[],
    ARRAY[0.0, 0.11111111, 0.22222222, 0.33333333, 0.44444444, 0.55555556, 0.66666667, 0.77777778, 0.88888889, 1.0]::DOUBLE PRECISION[], 2, 20)")";
    echo $result;
    [ "$result" = "-0.707192907427058" ]
}

@test "kstest sub array unbalanced" {
    result="$(query "SELECT kstest (ARRAY[0.58, 0.42, 0.52, 0.33, 0.43, 0.23, 0.58, 0.76, 0.53, 0.64]::DOUBLE PRECISION[],
    ARRAY[0.0, 0.11111111, 0.22222222, 0.33333333, 0.44444444, 0.55555556, 0.66666667, 0.77777778, 0.88888889, 1.0]::DOUBLE PRECISION[], 2, 20, 2)")";
    echo $result;
    [ "$result" = "-0.6738595740937247" ]
}
