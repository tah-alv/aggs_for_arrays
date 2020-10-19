load test_helper

@test "downsample full array double sum" {
    result="$(query "SELECT downsample('{1,1,2,2.5}'::double precision[], 2)")";
    echo $result;
    [ "$result" = "{2,4.5}" ]
}

@test "downsample full array int sum" {
    result="$(query "SELECT downsample('{1,1,1,2}'::int[], 2)")";
    echo $result;
    [ "$result" = "{2,3}" ]
}

@test "downsample full array real sum" {
    result="$(query "SELECT downsample('{1,1,1,2}'::real[], 2)")";
    echo $result;
    [ "$result" = "{2,3}" ]
}

@test "downsample full array real avg" {
    result="$(query "SELECT downsample('{1,1,1,2}'::real[], 2, 1, 4, true)")";
    echo $result;
    [ "$result" = "{1,1.5}" ]
}

@test "downsample sub array real sum" {
    result="$(query "SELECT downsample('{1,1,1,2}'::real[], 2, 3)")";
    echo $result;
    [ "$result" = "{3}" ]
}

@test "downsample sub array real avg" {
    result="$(query "SELECT downsample('{1,1,1,2}'::real[], 2, 3, 4, true)")";
    echo $result;
    [ "$result" = "{1.5}" ]
}

@test "downsample unit decimation factor" {
    result="$(query "SELECT downsample('{1,1,1,2}'::real[], 1)")";
    echo $result;
    [ "$result" = "{1,1,1,2}" ]
}


@test "downsample invalid decimation factor" {
    run query "SELECT downsample('{1,1,1,2}'::real[], 3)"
    [ "${lines[0]}" = "ERROR:  Array length must be multiple of decimation factor" ]
}

