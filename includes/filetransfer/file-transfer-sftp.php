<?php
if (file_exists("thirdparty/phpseclib/Net/SFTP.php")) // W32
{
    set_include_path("thirdparty/phpseclib" . PATH_SEPARATOR . get_include_path());
    include_once("thirdparty/phpseclib/Net/SFTP.php");
}
else // Mac
{
    set_include_path(dirname(__FILE__)."/..".PATH_SEPARATOR.get_include_path());
    include('thirdparty/phpseclib/Net/SFTP.php');
}

//echo get_include_path() . PHP_EOL;
//define('NET_SFTP_LOGGING', NET_SFTP_LOG_COMPLEX);

class FileTransferSftp extends FileTransferBase
{
    private $m_Hostname = "";
    private $m_Username = "";
    private $m_Password = "";
    private $m_Port;
    private $m_SftpConnection = NULL;
    private $m_PutFileSize = -1;
    private $m_ProgressCallbackFunction = "";

    public function __construct($hostname, $username, $password, $port=22)
    {
        $this->m_Hostname = $hostname;
        $this->m_Username = $username;
        $this->m_Password = $password;
        $this->m_Port = $port;
    }

    public function name()
    {
        return "PHP SFTP";
    }

    public function connect()
    {
        $this->m_SftpConnection = new Net_SFTP($this->m_Hostname, $this->m_Port);
        if (!$this->m_SftpConnection->login( $this->m_Username, $this->m_Password ))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    public function disconnect()
    {
        $this->m_SftpConnection = NULL;
        return true;
    }

    public function validate()
    {
        if (strlen($this->m_Hostname) == 0)
        {
            return false;
        }

        if (strlen($this->m_Username) == 0)
        {
            return false;
        }

        if (strlen($this->m_Password) == 0)
        {
            return false;
        }

        if (!is_numeric($this->m_Port))
        {
            return false;
        }

        return true;
    }

    public function send($sourcePath, $destinationPath)
    {
        $this->m_PutFileSize = filesize($sourcePath);
        $ret = $this->m_SftpConnection->put($destinationPath, $sourcePath, NET_SFTP_LOCAL_FILE, -1, -1, array($this, "progressCallback"));
        $this->m_PutFileSize = -1;
        
        return $ret;
    }
    
    public function deleteFile(&$fileExists, $filePath)
    {
        $fileExists = false;
        $sftp = $this->m_SftpConnection;
    
        // Check connection
        if (!$sftp)
        {
            return false;
        }
    
        // Check if file exists on server
        if (!$sftp->stat($filePath))
        {
            return true;
        }
    
        $fileExists = true;
        if (!$sftp->delete($filePath))
        {
            return false;
        }
        
        return true;
    }
    
    public function cleanDirectory($directory, $skipHiddenFiles)
    {
        $sftp = $this->m_SftpConnection;
        
        // Check connection
        if (!$sftp)
        {
            return false;
        }
        
        // Get list of files
        $fileList = $sftp->nlist($directory, false);
        if ($fileList === false)
        {
            return false;
        }
        
        // Delete files
        for ($i = 0; $i < count($fileList); $i++)
        {
            $fileName = $fileList[$i];
            
            // Skip default paths
            if ($fileName == "." || $fileName == "..")
            {
                continue;
            }
            
            // Skip hidden files
            if ($skipHiddenFiles && substr($fileName, 0, 1) == ".")
            {
                continue;
            }

            // Delete file
            if (!$this->deleteFile($fileExists, "$directory/$fileName"))
            {
                return false;
            }
        }
        
        return true;
    }
    
    function calculateChecksumMd5(&$checksum, $fileName)
    {
        $checksum = "";
        
        // Calculate checksum on remote file
        if (!$this->exec("md5sum \"$fileName\"", $calcChecksum, $ret))
        {
            return false;
        }
        
        // Check return code
        if ($ret != 0)
        {
            return false;
        }
        
        // Validate checksum
        if (strlen($calcChecksum) < 32 || !ctype_xdigit(substr($calcChecksum, 0, 32)))
        {
            return false;
        }
        
        // Get checksum
        $checksum = substr($calcChecksum, 0, 32);

        return true;
    }
    
    private function exec($command, &$output, &$returnCode)
    {
        $returnCode = -1;
        $sftp = $this->m_SftpConnection;
    
        // Check connection
        if (!$sftp)
        {
            return false;
        }
        
        $output = $sftp->exec($command);
        if ($sftp->getExitStatus() === false)
        {
            return false;
        }
        $returnCode = $sftp->getExitStatus();
        
        return true;
    }
    
    public function setProgressCallback($callbackFunctionName)
    {
        $this->m_ProgressCallbackFunction = $callbackFunctionName;
    }
    
    public function progressCallback($bytesSent)
    {
        if (strlen($this->m_ProgressCallbackFunction) > 0)
        {
            call_user_func($this->m_ProgressCallbackFunction, $bytesSent, $this->m_PutFileSize);
        }
    }
}
?>
