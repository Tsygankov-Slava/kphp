@ok
<?php
require_once 'kphp_tester_include.php';

/**
 * @return shape(id: int, name: string)
 */
function getShape() {
    return shape(["id" => 1, "name" => "John"]);
}

function f() {
    $shape = getShape();

    echo $shape["id"];
    echo $shape["name"];
}

f();
