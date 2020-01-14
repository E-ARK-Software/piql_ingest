<?php
// Send error message and exit program with return code 1
function exitWithError($message)
{
    global $communicator;
    global $logger;

    $communicator->sendError(gettext("Error in script. Check logs."));
    $logger->logError($message);
    exit(1);
}

// Send success message and exit program with return code 1
function exitWithSuccess()
{
    global $communicator;
    global $logger;

    $communicator->sendSuccess();
    $logger->logInfo("Finished successfully");
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
    if ($configuration->getValue("FileSendMethod") == FILE_SEND_METHOD_SFTP)
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
    else
    {
        $logger->logError("Invalid file send method: " . $configuration->getValue("FileSendMethod"));
        return false;
    }
    
    return true;
}

function selectLanguage($language)
{
    $language = $language . '.utf8';
    putenv("LANG=$language");
    setlocale(LC_ALL, $language);

    // Set the text domain as 'messages'
    $domain = 'messages';
    bindtextdomain($domain, "locale");
    textdomain($domain);

    return true;
}

function guidv4($data)
{
    $data = $data ?? random_bytes(16);
    return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($data), 4));
}
?>
