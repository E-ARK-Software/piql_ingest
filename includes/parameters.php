<?php
class Parameters
{
    public function __construct()
    {
    }

    public function get(&$ret, $index)
    { 
        global $argv;
        $ret = "";

        if (!isset($argv[$index]))
        {
            return false;
        }

        $ret = $argv[$index];

        return true;
    }
}
?>
