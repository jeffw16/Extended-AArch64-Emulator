<?php
/**
 * Pull Failing Test Cases from Gheith's API
 * @author Jeffrey Wang
 * (c) 2019 - All rights reserved
*/

// SET THESE VARIABLES BEFORE RUNNING THIS SCRIPT
$course = "429h";
$semester = "sp19";
$project = "5";
#$commit_id = "c60e7626d96f105518ef45a3dbda8d1d9ba0546b";
$commit_id = "4e9447fed46ea6e2148ef5c8c41afd14fc813ab2";
$test_case_file_ext = "arm";
$timeout = 5;
$email_address = "wang@cs.utexas.edu";
$directory = "~wang/Documents/cs429h/p5";
$test_cases_to_ignore = array(
    'de3c5a8cee23c28d80abb74b44c7201509182f4c', // Michael Jarrett's
    '7869450bbbe4707032226af93b6959cd77ff3ab6', // LDUR https://piazza.com/class/jr6z18z1qk6o2?cid=383
);
// END OF VARIABLES TO CHANGE

if ( !empty($course) && ( !ctype_alnum($course) || !ctype_alnum($semester) || !ctype_alnum($project) || !ctype_alnum($commit_id) ) ) {
    die("Invalid input.");
}
$filepath = "https://www.cs.utexas.edu/~gheith/cs" . $course . "_" . $semester . "_p" . $project . ".json";
$res = fopen($filepath, 'r');
$contents = stream_get_contents($res);
$data = json_decode($contents, true);
fclose($res);
$results = $data['results'];
$relevant_results = array();
$failing_results = array();
// $gheiths_results = array();
$statistics = array(
    'pass' => 0,
    'fail' => 0,
    'timeout' => 0
);
foreach ( $results as $result ) {
    if ( $result['commitId'] == $commit_id ) {
        $relevant_results[] = $result;
        if ( $result['outcome'] === true ) {
            $statistics['pass']++;
        } else if ( $result['outcome'] === false ) {
            $statistics['fail']++;
            if ( !in_array($result['test'], $test_cases_to_ignore) ) {
                $failing_results[] = $result;
            }
        } else {
            $statistics['timeout']++;
            if ( !in_array($result['test'], $test_cases_to_ignore) ) {
                $failing_results[] = $result;
            }
        }
    }
}
echo "Passing " . $statistics['pass'] . "\r\n";
echo "Failing " . $statistics['fail'] . "\r\n";
echo "Timeout " . $statistics['timeout'] . "\r\n";
/*
$email_output = array();
foreach ( $failing_results as $result ) {
    echo $result['test'] . '.' . $test_case_file_ext . "\r\n";
    $email_output[] = $result['test'] . '.' . $test_case_file_ext;
    exec("cp ~gheith/public_html/cs$course" . '_' . $semester . '_' . "p$project/$result[test].$test_case_file_ext $directory/tests/" . $result['test'] . '.' . $test_case_file_ext, $email_output);
    exec("cp ~gheith/public_html/cs$course" . '_' . $semester . '_' . "p$project/$result[test].ok $directory/tests/" . $result['test'] . '.ok', $email_output);
    exec("~gheith/public/arm/bin/aarch64-linux-gnu-objdump -d $directory/tests/" . $result['test'] . ".$test_case_file_ext > $directory/tests/" . $result['test'] . '.disas', $email_output);
    exec("timeout " . $timeout . " $directory/arm $directory/tests/" . $result['test'] . ".$test_case_file_ext > $directory/tests/$result[test].out", $email_output);
    exec("diff $directory/tests/" . $result['test'] . ".ok $directory/tests/" . $result['test'] . ".out > $directory/tests/" . $result['test'] . ".diff", $email_output);
}
$email_str = "";
foreach ( $email_output as $line ) {
    $email_str .= $line . "\r\n";
}
*/
$email_str = "";
foreach ( $failing_results as $result ) {
    $email_str .= $result['test'] . "\r\n";
}
mail($email_address, $course . " p" . $project . " - Failing " . ($statistics['fail'] + $statistics['timeout']), $email_str);
