@ok
<?php

require_once("Classes/autoload.php");

/**
 * @param Classes\IDo $x
 */
function check_is_a($x) {
    var_dump($x instanceof Classes\A);
}

/** @var Classes\IDo $x */
$x = new Classes\A;
$x = new Classes\B;
check_is_a($x);