<?php
register_shutdown_function("fatalErrorHandler");

function fatalErrorHandler()
{
    $error = error_get_last();
    if ($error !== NULL)
    {
        $json = json_encode($error);
        echo "ERROR {$json}" . PHP_EOL;
    }
}
?>
