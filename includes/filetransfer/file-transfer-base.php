<?php
abstract class FileTransferBase
{
    abstract public function send($sourcePath, $destinationPath);

    abstract public function validate();

    abstract public function connect();

    abstract public function disconnect();

    public function __destruct()
    {
        $this->disconnect();
    }
}
?>
