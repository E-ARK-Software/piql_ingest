<?php
// Send error message and exit program with return code 1
function exitWithError($message)
{
    global $communicator;
    global $logger;

    $communicator->sendError($message);
    $logger->logError($message);
    sleep(0.2);
    exit(1);
}

// Send success message and exit program with return code 1
function exitWithSuccess()
{
    global $communicator;
    global $logger;

    $communicator->sendSuccess();
    $logger->logInfo("Finished successfully");
    sleep(0.2);
    exit(0);
}

// Delete directory and its content recursively
function rrmdir($dir) 
{
    $success = true;
    
    if (substr($dir, strlen($dir) - 2, 2) == '//') 
    {
        $dir = substr($dir, 0, strlen($dir) - 1);
    }
    
    if (is_dir($dir)) 
    {
        $objects = scandir($dir);
        foreach ($objects as $object) 
        {
            if ($object != "." && $object != "..") 
            {
                if (is_dir($dir . "/" . $object)) 
                {
                    if (!rrmdir($dir . "/" . $object)) 
                    {
                        $success = false;
                    }
                }
                else
                {
                    if (!unlink($dir . "/" . $object)) 
                    {
                        $success = false;
                    }
                }
            }
        }
        return rmdir($dir) && $success;
    }
    
    return false;
}

// Delete file or directory recursively
function deleteFromDisk($file, $errorOnMissingFile=true) 
{
    // Check if file or directory exists on disk
    if (!file_exists($file))
    {
        return !$errorOnMissingFile;
    }

    // Delete from disk
    if (is_dir($file)) 
    {
        return rrmdir($file);
    }
    else
    {
        return unlink($file);
    }
}

function normalizeFilePath($path)
{
    return str_replace("\\", "/", $path);
}

function createFileSender(&$fileSender, $configuration, $logger, $progressCallback)
{
    $fileSender = NULL;
    if ($configuration->getValue("FileSendMethod") == FILE_SEND_METHOD_DISK)
    {
        // Create file sender
        $fileSender = new FileTransferDisk($configuration->getValue("FileSenderDiskOutputDirectory"));
        if ($fileSender == NULL)
        {
            $logger->logError("Failed to create file sender" . $fileSender->name());
            return false;
        }
        
        if (!$fileSender->validate())
        {
            $logger->logError("Failed to validate file sender: " . $fileSender->name());
            return false;
        }
        if (!$fileSender->connect())
        {
            $logger->logError("Failed to connect with file sender: " . $fileSender->name());
            return false;
        }
        
        if (strlen($progressCallback) == 0)
        {
            $fileSender->setProgressCallback(function($bytesSent, $bytesTotal){});
        }
        else
        {
            $fileSender->setProgressCallback($progressCallback);
        }
    }
    else if ($configuration->getValue("FileSendMethod") == FILE_SEND_METHOD_SFTP)
    {
        // Create file sender
        $fileSender = new FileTransferSftp(
            $configuration->getValue("SshHostname"),
            $configuration->getValue("SshUsername"),
            $configuration->getValue("SshPassword"),
            $configuration->getValue("SshPort"));
        if ($fileSender == NULL)
        {
            $logger->logError("Failed to create file sender" . $fileSender->name());
            return false;
        }
        
        if (!$fileSender->validate())
        {
            $logger->logError("Failed to validate file sender: " . $fileSender->name());
            return false;
        }
        if (!$fileSender->connect())
        {
            $logger->logError("Failed to connect with file sender: " . $fileSender->name());
            return false;
        }
        
        if (strlen($progressCallback) == 0)
        {
            $fileSender->setProgressCallback(function($bytesSent, $bytesTotal){});
        }
        else
        {
            $fileSender->setProgressCallback($progressCallback);
        }
    }
    else if ($configuration->getValue("FileSendMethod") == FILE_SEND_METHOD_SSH)
    {
        $username = "";
        $password = "";
        $publicKeyFile = "";
        $privateKeyFile = "";
        $privateKeyPassword = "";
        if ($configuration->exists("SshUsername"))
        {
            $username = $configuration->getValue("SshUsername");
        }
        if ($configuration->exists("SshPassword"))
        {
            $password = $configuration->getValue("SshPassword");
        }
        if ($configuration->exists("SshPubkeyFile"))
        {
            $publicKeyFile = $configuration->getValue("SshPubkeyFile");
        }
        if ($configuration->exists("SshPrivkeyFile"))
        {
            $privateKeyFile = $configuration->getValue("SshPrivkeyFile");
        }
        if ($configuration->exists("SshPrivkeyPassword"))
        {
            $privateKeyPassword = $configuration->getValue("SshPrivkeyPassword");
        }

        // Create file sender
        $fileSender = new FileTransferSsh(
            $configuration->getValue("SshHostname"),
            $username,
            $password,
            $publicKeyFile,
            $privateKeyFile,
            $privateKeyPassword,
            $configuration->getValue("SshPort"));
        if ($fileSender == NULL)
        {
            $logger->logError("Failed to create file sender" . $fileSender->name());
            return false;
        }

        if (!$fileSender->validate())
        {
            $logger->logError("Failed to validate file sender: " . $fileSender->name());
            return false;
        }
        if (!$fileSender->connect())
        {
            $logger->logError("Failed to connect with file sender: " . $fileSender->name());
            return false;
        }

        if (strlen($progressCallback) == 0)
        {
            $fileSender->setProgressCallback(function($bytesSent, $bytesTotal){});
        }
        else
        {
            $fileSender->setProgressCallback($progressCallback);
        }
    }
    else
    {
        $logger->logError("Invalid file send method: " . $configuration->getValue("FileSendMethod"));
        return false;
    }
    
    return true;
}

function selectLanguage($language)
{
    $language = $language . '.UTF-8';
    putenv("LANG=$language");
    putenv("LC_ALL=$language");
    setlocale(LC_ALL, $language);

    // Set the text domain as 'messages'
    $domain = 'messages';
    bindtextdomain($domain, "locale");
    textdomain($domain);

    return true;
}

function guidv4($data)
{
    assert(strlen($data) == 16);

    $data[6] = chr(ord($data[6]) & 0x0f | 0x40); // set version to 0100
    $data[8] = chr(ord($data[8]) & 0x3f | 0x80); // set bits 6-7 to 10

    return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($data), 4));
}
?>
