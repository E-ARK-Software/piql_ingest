<?php
class FileTransferDisk extends FileTransferBase
{
    private $m_OutputDirectory = "";
    private $m_ProgressCallbackFunction = "";

    public function __construct($outputDirectory)
    {
        $this->m_OutputDirectory = $outputDirectory;
    }

    public function name()
    {
        return "PHP DISK";
    }

    public function connect()
    {
        return true;
    }

    public function disconnect()
    {
        return true;
    }

    public function validate()
    {
        if (!is_dir($this->m_OutputDirectory))
        {
            return false;
        }

        return true;
    }

    public function send($sourcePath, $destinationPath)
    {
        if (!copy($sourcePath, "{$this->m_OutputDirectory}/{$destinationPath}"))
        {
            return false;
        }

        return true;
    }

    public function setProgressCallback($callbackFunctionName)
    {
        $this->m_ProgressCallbackFunction = $callbackFunctionName;
    }
}
?>
