<?php
define("COMMUNICATION_TYPE_PROGRESS", "PROGRESS");
define("COMMUNICATION_TYPE_ERROR", "ERROR");
define("COMMUNICATION_TYPE_SUCCESS", "SUCCESS");

class Communicator
{
    private $m_IncludeMessageTypes = false;

    public function __construct($includeMessageTypes)
    {
        $this->disableOutput();
        $this->m_IncludeMessageTypes = $includeMessageTypes;
    }

    public function sendProgress($progress, $status)
    { 
        $this->sendMessage(COMMUNICATION_TYPE_PROGRESS, "$progress $status");
    }

    public function sendError($message)
    {
        $this->sendMessage(COMMUNICATION_TYPE_ERROR, $message);
    }

    public function sendSuccess()
    { 
        $this->sendMessage(COMMUNICATION_TYPE_SUCCESS, "");
    }
    
    public function sendPlaintext($text)
    {
        $this->enableOutput();
        echo $text;
        $this->disableOutput();
    }

    private function sendMessage($type, $message)
    {
        $this->enableOutput();
        if ($this->m_IncludeMessageTypes)
        {
            echo $type . " " . $message . PHP_EOL;
        }
        else
        {
            echo $message . PHP_EOL;
        }
        $this->disableOutput();
    }

    private function enableOutput()
    {
        // Write php output to file
        global $logger;
        if (isset($logger))
        {
            $output = ob_get_contents();
            if (strlen($output) > 0)
            {
                $logger->logPhp($output);
            }
        }
    
        ob_end_clean();
    }

    private function disableOutput()
    {
        ob_start();
    }
}
?>
