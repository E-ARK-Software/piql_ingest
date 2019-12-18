<?php
date_default_timezone_set('Europe/Kiev');
$configuration = new Configuration();
$configuration->add("SshHostname", "127.0.0.1");
$configuration->add("SshUsername", "ivancopc");
$configuration->add("SshPassword", "12345");
$configuration->add("SshDestinationDir", "");
$configuration->add("SshPort", 22);
$configuration->add("VerifyUploadChecksum", false);
$configuration->add("OutputArchiveFormat", OUTPUT_ARCHIVE_FORMAT_ZIP);
$configuration->add("OutputFormat", OUTPUT_FORMAT_BAGIT_V1);
$configuration->add("LogDir", sys_get_temp_dir());
$configuration->add("CommitAckMethod", COMMIT_ACK_METHOD_SHA1_V1);
$configuration->add("LogDebug", false);
$configuration->add("LogInfo", true);
$configuration->add("LogWarning", true);
$configuration->add("LogError", true);
$configuration->add("FileSendMethod", FILE_SEND_METHOD_SFTP);
$configuration->add("SenderMaxRetries", 4);
$configuration->add("SenderFailDelay", 30);
$configuration->add("MetadataOutputFormat", METADATA_OUTPUT_FORMAT_DUBLINCORE_CSV);
//$configuration->add("MetadataOutputItemOrder", array(0, 1, 6, 4, 5, 9, 10, 11, 13, 14, 16, 17, 8, 18, 2, 3, 12, 7, 15));
//$configuration->add("MetadataCsvFondPosition", 14);

function outputArchiveFileName($filePaths)
{
    global $configuration;
    
    // Set base name
    if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_DATAONLY)
    {
        $fileName = "archive-" . date("Y-m-d-H-i-s");
    }
    else if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_BAGIT_V1 || $configuration->getValue("OutputFormat") == OUTPUT_FORMAT_BAGIT_V097)
    {
        if (count($filePaths) == 0)
        {
            exit(1);
        }
        $fileNames = array();
        for ($i = 0; $i < count($filePaths); $i++)
        {
            array_push($fileNames, pathinfo(basename($filePaths[$i]), PATHINFO_FILENAME));
        }
        if (!sort($fileNames, SORT_STRING))
        {
            exit(1);
        }
        $fileName = $fileNames[0];
    }
    else
    {
        exit(1);
    }
    
    // Set extension
    if ($configuration->getValue("OutputArchiveFormat") == OUTPUT_ARCHIVE_FORMAT_TAR)
    {
        $fileName .= ".tar";
    }
    else if ($configuration->getValue("OutputArchiveFormat") == OUTPUT_ARCHIVE_FORMAT_ZIP)
    {
        $fileName .= ".zip";
    }
    else
    {
        exit(1);
    }
    
    // Replace spaces with underscore
    $fileName = str_replace(" ", "_", $fileName);
    
    return $fileName;
}
?>
