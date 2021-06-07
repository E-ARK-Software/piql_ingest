<?php
class FileTransferSsh extends FileTransferBase
{
    private $m_Hostname = "";
    private $m_Username = "";
    private $m_Password = "";
    private $m_Port;
    private $m_PubkeyFile = "";
    private $m_PrivkeyFile = "";
    private $m_PrivkeyPassword = "";
    private $m_Connection = NULL;
    private $m_ProgressCallbackFunction = "";

    public function __construct($hostname, $username, $password, $pubkeyFile, $privkeyFile, $privkeyPassword, $port=22)
    {
        $this->m_Hostname = $hostname;
        $this->m_Username = $username;
        $this->m_Password = $password;
        $this->m_PubkeyFile = $pubkeyFile;
        $this->m_PrivkeyFile = $privkeyFile;
        $this->m_PrivkeyPassword = $privkeyPassword;
        $this->m_Port = $port;
    }

    public function name()
    {
        return "PHP SSH2";
    }

    public function connect()
    {
        // Create connection
        $this->m_Connection = ssh2_connect($this->m_Hostname, $this->m_Port, array('hostkey'=>'ssh-rsa'));
        if (!$this->m_Connection)
        {
            return false;
        }

        // Authenticate
        if (strlen($this->m_Username) > 0 && strlen($this->m_Password) > 0)
        {
            if (!ssh2_auth_password($this->m_Connection, $this->m_Username, $this->m_Password))
            {
                return false;
            }
        }
        else if (strlen($this->m_PubkeyFile) > 0 && strlen($this->m_PrivkeyFile) > 0)
        {
            if (!ssh2_auth_pubkey_file($this->m_Connection, $this->m_Username, $this->m_PubkeyFile, $this->m_PrivkeyFile, $this->m_PrivkeyPassword))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        return true;
    }

    public function disconnect()
    {
        $this->m_Connection = NULL;
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
            if (strlen($this->m_PubkeyFile) == 0 || strlen($this->m_PrivkeyFile) == 0)
            {
                return false;
            }
        }

        if (!is_numeric($this->m_Port))
        {
            return false;
        }

        return true;
    }

    public function send($sourcePath, $destinationPath)
    {
        if (!ssh2_scp_send($this->m_Connection, $sourcePath, $destinationPath))
        {
            return false;
        }

        return true;
    }

    public function executeCommand(&$returnCode, $command)
    {
        // Run command
        $stream = ssh2_exec($this->m_Connection, "($command); echo -en \"\\n$?\"");
        if (!$stream)
        {
            return false;
        }

        // Fetch output
        $errorStream = ssh2_fetch_stream( $stream, SSH2_STREAM_STDERR );
        stream_set_blocking( $stream, true );
        stream_set_blocking( $errorStream, true );
        $output = stream_get_contents( $stream );
        $errorOutput = stream_get_contents( $errorStream );
        fclose( $stream );
        fclose( $errorStream );

        // Check return code
        if( ! preg_match( "/^(.*)\n(0|-?[1-9][0-9]*)$/s", $output, $matches ) )
        {
            // Failed to find return status
            return false;
        }
        $returnCode = intval($matches[2]);

        return true;
    }

    public function setProgressCallback($callbackFunctionName)
    {
        $this->m_ProgressCallbackFunction = $callbackFunctionName;
    }
}
?>
