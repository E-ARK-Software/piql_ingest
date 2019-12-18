<?php
define("LOG_LEVEL_DEBUG", "DEBUG");
define("LOG_LEVEL_INFO", "INFO");
define("LOG_LEVEL_WARNING", "WARNING");
define("LOG_LEVEL_ERROR", "ERROR");
define("LOG_LEVEL_NONE", "NONE");

class Logger
{
    private $m_LogFileDir = "";
    private $m_LogFileName = "";
    private $m_LogFileNamePhp = "";
    private $m_LogDebug = false;
    private $m_LogInfo = false;
    private $m_LogWarning = false;
    private $m_LogError = false;
    private $m_TimeStampFormat = "Y-m-d H:i:s";
    private $m_ExitOnFailure = false;

    public function __construct($logFileDir)
    {
        $this->m_LogFileDir = $logFileDir;
        $this->m_LogFileName = date("Y-m-d") . "_" . pathinfo($_SERVER['SCRIPT_NAME'], PATHINFO_FILENAME) . ".log";
        $this->m_LogFileNamePhp = date("Y-m-d") . "_" . pathinfo($_SERVER['SCRIPT_NAME'], PATHINFO_FILENAME) . "_php.log";
    }

    private function setTimeStamp($format)
    {
        $this->m_TimeStampFormat = $format;
    }

    public function setLogDebug($state)
    {
        $this->m_LogDebug = $state;
    }

    public function setLogInfo($state)
    {
        $this->m_LogInfo = $state;
    }

    public function setLogWarning($state)
    {
        $this->m_LogWarning = $state;
    }

    public function setLogError($state)
    {
        $this->m_LogError = $state;
    }

    public function logDebug($text)
    {
        $this->log(LOG_LEVEL_DEBUG, $text, $this->m_LogFileDir, $this->m_LogFileName);
    }
    
    public function logInfo($text)
    {
        $this->log(LOG_LEVEL_INFO, $text, $this->m_LogFileDir, $this->m_LogFileName);
    }
    
    public function logWarning($text)
    {
        $this->log(LOG_LEVEL_WARNING, $text, $this->m_LogFileDir, $this->m_LogFileName);
    }
    
    public function logError($text)
    {
        $this->log(LOG_LEVEL_ERROR, $text, $this->m_LogFileDir, $this->m_LogFileName);
    }
    
    public function logPhp($text)
    {
        $this->log(LOG_LEVEL_NONE, $text, $this->m_LogFileDir, $this->m_LogFileNamePhp);
    }

    private function log($logLevel, $text, $dirPath, $fileName)
    {
        $output = "";
        
        // Add timestamp
        if (strlen($this->m_TimeStampFormat) > 0)
        {
            $output = date($this->m_TimeStampFormat) . " ";
        }

        // Add log level
        if ($logLevel == LOG_LEVEL_DEBUG)
        {
            if (!$this->m_LogDebug)
            {
                return;
            }
            $output .= "-D- ";
        }
        else if ($logLevel == LOG_LEVEL_INFO)
        {
            if (!$this->m_LogInfo)
            {
                return;
            }
            $output .= "-I- ";
        }
        else if ($logLevel == LOG_LEVEL_WARNING)
        {
            if (!$this->m_LogWarning)
            {
                return;
            }
            $output .= "-W- ";
        }
        else if ($logLevel == LOG_LEVEL_ERROR)
        {
            if (!$this->m_LogError)
            {
                return;
            }
            $output .= "-E- ";
        }
        else if ($logLevel == LOG_LEVEL_NONE)
        {
            // Add nothing
        }
        else
        {
            // Unrecognized log level
            if ($this->m_ExitOnFailure)
            {
                exit(1);
            }
            return;
        }
        
        // Add message
        $output .= $text;

        // Write to file
        if (strlen($fileName) > 0)
        {
            $logPath = "";
            if (strlen($dirPath) > 0)
            {
                $logPath = $dirPath . "/";
                
                // Create directory if it doesn't exist
                if (!file_exists($dirPath))
                {
                    if (!mkdir($dirPath, 0777, true) && $this->m_ExitOnFailure)
                    {
                        exit(1);
                    }
                }
            }
            $logPath .= $fileName;
            if (!file_put_contents($logPath, $output . PHP_EOL, FILE_APPEND | LOCK_EX) && $this->m_ExitOnFailure)
            {
                // Failed to write to log
                exit(1);
            }
        }
    }
}
?>
