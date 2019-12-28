<?php
// Includes
//

include_once("includes/defines.php");
include_once("includes/configuration.php");
include_once("includes/communicator.php");
include_once("includes/platforms.php");
include_once("includes/parameters.php");
include_once("includes/filetransfer/file-transfer-base.php");
include_once("includes/filetransfer/file-transfer-ssh.php");
include_once("includes/filetransfer/file-transfer-sftp.php");
include_once("includes/logger.php");
include_once("includes/metadata-parser.php");
include_once("includes/general-functions.php");
include_once("includes/script-context.php");
include_once("metadata-description.php");
include_once("config.php");

// Setup
//

// Setup logging
$logger = new Logger($configuration->getValue("LogDir"));
$logger->setLogDebug($configuration->getValue("LogDebug"));
$logger->setLogInfo($configuration->getValue("LogInfo"));
$logger->setLogWarning($configuration->getValue("LogWarning"));
$logger->setLogError($configuration->getValue("LogError"));

// Create communication line to GUI
$communicator = new Communicator(true);

// Input arguments
//
$parameters = new Parameters();

// Get language
if (!$parameters->get($language, 1))
{
    exitWithError("Missing argument: Language");
}
if (!selectLanguage($language))
{
    exitWithError("Failed to setup language: $language");
}

// Get context
if (!$parameters->get($contextPath, 2))
{
    exitWithError("Missing argument: Context");
}
$contextPath = normalizeFilePath($contextPath);
$scriptContext = new ScriptContext();
if (!$scriptContext->readFile($contextPath))
{
    exitWithError("Failed to read context: $contextPath");
}

// Get temp directory path
if (!$scriptContext->valueAsString($tempDirectoryPath, "TempFileDirectory"))
{
    exitWithError("Failed to get temp directory");
}
$tempDirectoryPath = normalizeFilePath($tempDirectoryPath);
if (substr($tempDirectoryPath, strlen($tempDirectoryPath) - 1, 1) == "/")
{
    $tempDirectoryPath = substr($tempDirectoryPath, 0, strlen($tempDirectoryPath) - 1);
}
if (!file_exists($tempDirectoryPath) || !is_dir($tempDirectoryPath))
{
    exitWithError("Temp directory does not exist: " . $tempDirectoryPath);
}

// Get files
if (!$scriptContext->valueAsArray($filePathList, "FilePaths"))
{
    exitWithError("Failed to get file paths");
}
if (count($filePathList) == 0)
{
    exitWithError("Path list is empty");
}
for ($i = 0; $i < count($filePathList); $i++)
{
    $filePathList[$i] = normalizeFilePath($filePathList[$i]);
    if (!file_exists($filePathList[$i]))
    {
        exitWithError("File does not exist on disk: " . $filePathList[$i]);
    }
}

// Get relative paths of files
if (!$scriptContext->valueAsArray($relativeFilePathList, "RelativeFilePaths"))
{
    exitWithError("Failed to get relative file paths");
}
if (count($relativeFilePathList) == 0)
{
    exitWithError("Relative path list is empty");
}
for ($i = 0; $i < count($relativeFilePathList); $i++)
{
    $relativeFilePathList[$i] = normalizeFilePath($relativeFilePathList[$i]);
}

// Get file item types - file, directory etc.
if (!$scriptContext->valueAsArray($fileItemTypeList, "FileItemTypes"))
{
    exitWithError("Failed to get file item types");
}
if (count($fileItemTypeList) == 0)
{
    exitWithError("File item type list is empty");
}

// Get file IDs
if (!$scriptContext->valueAsArray($fileIdList, "FileIds"))
{
    exitWithError("Failed to get file IDs");
}
if (count($fileIdList) == 0)
{
    exitWithError("File ID list is empty");
}
for ($i = 0; $i < count($fileIdList); $i++)
{
    if (!is_numeric($fileIdList[$i]))
    {
        exitWithError("Invalid file ID: " . $fileIdList[$i]);
    }
}

// Get parent file IDs
if (!$scriptContext->valueAsArray($parentFileIdList, "ParentFileIds"))
{
    exitWithError("Failed to get parent file IDs");
}
if (count($parentFileIdList) == 0)
{
    exitWithError("Parent file ID list is empty");
}
for ($i = 0; $i < count($parentFileIdList); $i++)
{
    if (!is_numeric($parentFileIdList[$i]))
    {
        exitWithError("Invalid parent file ID: " . $parentFileIdList[$i]);
    }
}

// Get metadata
if (!$scriptContext->valueAsArray($metadataList, "FileMetadata"))
{
    exitWithError("Failed to get metadata");
}
if (count($metadataList) == 0)
{
    exitWithError("Metadata list is empty");
}

// Functions
//

function logError($text)
{
    global $logger;
    $logger->logError($text);
}

function loginfo($text)
{
    global $logger;
    $logger->logInfo($text);
}

function createMetadata($metadataListIn, $metadataFileOut, $fileNameList, $metadataTemplate, $configuration)
{
    $metadataParser = new MetadataParser();
    
    // Get output format
    $metadataOutputFormat = $configuration->getValue("MetadataOutputFormat");
    
    // Setup expected keys
    for ($i = 0; $i < $metadataTemplate->groupCount(); $i++)
    {
        if (!$metadataTemplate->getGroup($group, $i))
        {
            logError("Failed to get group");
            return false;
        }
    
        for ($j = 0; $j < $group->itemCount(); $j++)
        {
            if (!$group->getItem($item, $j))
            {
                 logError("Failed to get metadata item");
                 return false;
            }
        
            $required = true;
            if (!$metadataParser->addKeyTemplate(new MetadataParserKeyTemplate($item->name(), $required)))
            {
                logError("Failed to add key template");
                return false;
            }
        }
    }
    
    // Read metadata for file
    for ($i = 0; $i < count($metadataListIn); $i++)
    {
        if (!$metadataParser->readString($metadataListIn[$i], $errorMessage))
        {
            logError("Failed to read metadata: $errorMessage");
            return false;
        }
    }
    
    // Skip metadata if empty
    if ($metadataParser->itemCount() == 0)
    {
        return true;
    }
    
    // Write output in CSV format
    if ($metadataOutputFormat == METADATA_OUTPUT_FORMAT_DUBLINCORE_CSV)
    {
        // Get output order
        $outputOrder = array();
        if ($configuration->exists("MetadataOutputItemOrder"))
        {
            $outputOrder = $configuration->getValue("MetadataOutputItemOrder");
        }
        
        // Get output order
        $fondPosition = 2;
        if ($configuration->exists("MetadataCsvFondPosition"))
        {
            $fondPosition = $configuration->getValue("MetadataCsvFondPosition");
        }
    
        if (!$metadataParser->writeDublinCoreCsvToFile($metadataFileOut, $fileNameList, $outputOrder, $fondPosition))
        {
            logError("Failed to write metadata to output file: $metadataFileOut");
            return false;
        }
    }
    else
    {
        logError("Invalid metadata output format");
        return false;
    }
    
    return true;
}

function createBagitDescriptionFile($descriptionOutFile, $version)
{
    $text = "";
    $text .= "BagIt-Version: " . $version . PHP_EOL;
    $text .= "Tag-File-Character-Encoding: UTF-8" . PHP_EOL;
    file_put_contents($descriptionOutFile, $text, LOCK_EX);
    
    return true;
}

function createChecksumManifestFile($checksumManifestOut, $filePathList, $relativeFilePathList)
{
    $text = "";
    for ($i = 0; $i < count($filePathList); $i++)
    {
        $filePath = $filePathList[$i];
        if (!is_dir($filePath))
        {
            $text .= md5_file($filePath) . " " . "data/" . $relativeFilePathList[$i] . PHP_EOL;
        }
    }
    file_put_contents($checksumManifestOut, $text, LOCK_EX);
    
    return true;
}

function sendProgressCallback($bytesSent, $bytesTotal)
{
    global $communicator;
    $percent = round(($bytesSent/$bytesTotal)*100);
    $communicator->sendProgress($percent, gettext("Sending to server"));
}

// Commit instructions
//

// Identify platform
$platforms = new Platforms();
$platforms->addSupported(PLATFORM_CENTOS6);
$platforms->addSupported(PLATFORM_CENTOS7);
$platforms->addSupported(PLATFORM_WINDOWS7);
$platforms->addSupported(PLATFORM_WINDOWS8);
$platforms->addSupported(PLATFORM_WINDOWS10);
$platforms->addSupported(PLATFORM_OSX);

$platform = $platforms->identify($os, $distribution);
if (!$platforms->isSupported($platform))
{
    exitWithError("Unsupported platform: platform='$platform' os='$os' distribution='$distribution'");
}

// Send first progress
$communicator->sendProgress(0, gettext("Preparing data"));

// Build data for archive
$archiveFiles = array();
{
    // Create package files
    // Create package files
    if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_BAGIT_V097)
    {
        // Create a SIP in bagit format
        // - One or multiple files per SIP
        // - One metadata row per file
        // - Directories are supported
        // - Directory structure is maintained
        // - Can be processed by Archivematica
        //
        // Bag structure:
        // <file_basename>
        //   metadata
        //     metadata.csv
        //   data
        //     <filename_1>
        //     <filename_2>
        //   bagit.txt
        //   manifest-md5.txt
        
        $dataRootDir = "";
        $dataDir = "";
        for ($fileIndex = 0; $fileIndex < count($filePathList); $fileIndex++)
        {
            // Get file info
            $filePath = $filePathList[$fileIndex];
            $fileName = basename($filePath);
            $fileItemType = $fileItemTypeList[$fileIndex];
            $relativeFilePath = $relativeFilePathList[$fileIndex];
        
            if ($fileItemType != FILE_ITEM_TYPE_FILE && $fileItemType != FILE_ITEM_TYPE_DIRECTORY)
            {
                exitWithError("Invalid file item type: $fileItemType");
            }
        
            if ($fileIndex == 0)
            {
                if (count($filePathList) < 1)
                {
                    exitWithError("One or multiple files is supported for format OUTPUT_FORMAT_BAGIT_V097, " . count($filePathList) . " was given");
                }
                
                $dataRootDir = "$tempDirectoryPath";

                // Create data directory
                $dataDir = "$dataRootDir/data";
                if (!mkdir($dataDir, 0777, false))
                {
                    exitWithError("Failed to create data directory");
                }
                array_push($archiveFiles, basename($dataDir));
                
                // Create metadata directory
                $metadataDir = "$dataRootDir/metadata";
                if (!mkdir($metadataDir, 0777, false))
                {
                    exitWithError("Failed to create metadata directory");
                }
                
                // Create metadata file
                $fileNames = array();
                for ($i = 0; $i < count($filePathList); $i++)
                {
                    array_push($fileNames, basename($filePathList[$i]));
                }
                $metadataOut = "$metadataDir/metadata.csv";
                if (!createMetadata($metadataList, $metadataOut, $fileNames, $metadataTemplate, $configuration))
                {
                    exitWithError("Failed to create metadata file");
                }
                
                // Include metadata if generated
                if (is_file($metadataOut))
                {
                    array_push($archiveFiles, basename($metadataDir));
                }
                
                // Create BagIt description file
                $descriptionOut = "$dataRootDir/bagit.txt";
                if (!createBagitDescriptionFile($descriptionOut, "0.97"))
                {
                    exitWithError("Failed to create BagIt description file");
                }
                array_push($archiveFiles, basename($descriptionOut));
                
                // Create checksum manifest
                $checksumManifestOut = "$dataRootDir/manifest-md5.txt";
                if (!createChecksumManifestFile($checksumManifestOut, $filePathList, $relativeFilePathList))
                {
                    exitWithError("Failed to create checksum manifest file");
                }
                array_push($archiveFiles, basename($checksumManifestOut));
            }
            
            // Add data
            $source = $filePath;
            $destination = "$dataDir/$relativeFilePath";
            if ($fileItemType == FILE_ITEM_TYPE_DIRECTORY)
            {
                if (!mkdir($destination, 0777, false))
                {
                    exitWithError("Failed to create directory: $destination");
                }
            }
            else
            {
                if (!copy($source, $destination))
                {
                    exitWithError("Failed to copy file '$source' to '$destination'");
                }
            }
        }
    }
    else if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_BAGIT_V1)
    {
        // Create a SIP in bagit format
        // - One or multiple files per SIP
        // - One metadata row per file
        // - Directories not supported
        // - Directory structure is ignored
        // - Can be processed by Archivematica if repackaged
        //
        // Bag structure:
        // <file_basename>
        //   metadata
        //     metadata.csv
        //   <filename_1>
        //   <filename_2>
    
        $dataRootDir = "";
        for ($fileIndex = 0; $fileIndex < count($filePathList); $fileIndex++)
        {
            // Get file info
            $filePath = $filePathList[$fileIndex];
            $fileName = basename($filePath);
            $fileItemType = $fileItemTypeList[$fileIndex];
        
            if ($fileItemType == FILE_ITEM_TYPE_DIRECTORY)
            {
                exitWithError("Directories are not allowed for format: " . OUTPUT_FORMAT_BAGIT_V1);
            }
            else if ($fileItemType != FILE_ITEM_TYPE_FILE)
            {
                exitWithError("Invalid file item type: $fileItemType");
            }
        
            if ($fileIndex == 0)
            {
                if (count($filePathList) < 1)
                {
                    exitWithError("One or multiple files is supported for format OUTPUT_FORMAT_BAGIT_V1, " . count($filePathList) . " was given");
                }
                
                // Get basename of first file in alphabetical order
                $fileNames = array();
                for ($i = 0; $i < count($filePathList); $i++)
                {
                    array_push($fileNames, pathinfo(basename($filePathList[$i]), PATHINFO_FILENAME));
                }
                if (!sort($fileNames, SORT_STRING))
                {
                    exitWithError("Failed to sort file names");
                }
                $fileNameBase = $fileNames[0];
                
                // Create root directory
                $dataRootDir = "$tempDirectoryPath/$fileNameBase";
                if (!mkdir($dataRootDir, 0777, false))
                {
                    exitWithError("Failed to create data root directory: $dataRootDir");
                }
                
                // Create metadata directory
                $metadataDir = "$dataRootDir/metadata";
                if (!mkdir($metadataDir, 0777, false))
                {
                    exitWithError("Failed to create metadata directory");
                }
                
                // Create metadata file
                $fileNames = array();
                for ($i = 0; $i < count($filePathList); $i++)
                {
                    array_push($fileNames, basename($filePathList[$i]));
                }
                $metadataOut = "$metadataDir/metadata.csv";
                createMetadata($metadataList, $metadataOut, $fileNames, $metadataTemplate, $configuration);
                
                array_push($archiveFiles, $fileNameBase);
            }
            
            // Add data
            $source = $filePath;
            $destination = "$dataRootDir/$fileName";
            if (!copy($source, $destination))
            {
                exitWithError("Failed to copy file '$source' to '$destination'");
            }
        }
    }
    else if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_DATAONLY)
    {
        // Create a package containing only the input files
        // - Maintains directory structure
        // - Supports multiple files
        // - Can be sent directly to AMU
    
        for ($fileIndex = 0; $fileIndex < count($filePathList); $fileIndex++)
        {
            // Get file info
            $filePath = $filePathList[$fileIndex];
            $fileItemType = $fileItemTypeList[$fileIndex];
            $relativeFilePath = $relativeFilePathList[$fileIndex];
            $fileId = $fileIdList[$fileIndex];
        
            if ($fileItemType == FILE_ITEM_TYPE_DIRECTORY)
            {
                // Only add directory if it's empty, otherwise it will be created when adding files
                for ($i = 0; $i < count($parentFileIdList); $i++)
                {
                    if ($fileId == $parentFileIdList[$i])
                    {
                        break;
                    }
                    if ($i == count($parentFileIdList) - 1)
                    {
                        // Create directory
                        $dirPath = "$tempDirectoryPath/$relativeFilePath";
                        if (substr($dirPath, strlen($dirPath) - 1) == "/")
                        {
                            $dirPath = substr($dirPath, 0, strlen($dirPath) - 1);
                        }
                        
                        if (file_exists($dirPath))
                        {
                            exitWithError("Directory already exists: $dirPath");
                        }
                        else
                        {
                            if (!mkdir($dirPath, 0777, true))
                            {
                                exitWithError("Failed to create directory: $dirPath");
                            }
                            array_push($archiveFiles, $dirPath);
                        }
                    }
                }
            }
            else if ($fileItemType == FILE_ITEM_TYPE_FILE)
            {
                // Create directory structure
                $pathComponents = explode("/", $relativeFilePath);
                if (count($pathComponents) < 1)
                {
                    exitWithError("Failed to return components for path: $relativeFilePath");
                }
                for ($i = 0; $i < count($pathComponents) - 1; $i++)
                {
                    $dirPath = "$tempDirectoryPath";
                    for ($j = 0; $j <= $i; $j++)
                    {
                        $dirPath .= "/" . $pathComponents[$j];
                    }
                
                    if (!is_dir($dirPath))
                    {
                        if (!mkdir($dirPath, 0777, true))
                        {
                            exitWithError("Failed to create directory: $dirPath");
                        }
                    }
                }
            
                // Add data
                $source = $filePath;
                $destination = "$tempDirectoryPath/$relativeFilePath";
                if (!copy($source, $destination))
                {
                    exitWithError("Failed to copy file '$source' to '$destination'");
                }
                
                array_push($archiveFiles, $relativeFilePath);
            }
            else
            {
                exitWithError("Invalid file item type: $fileItemType");
            }
        }
    }
    else
    {
        exitWithError("Invalid output format: " . $configuration->getValue("OutputFormat"));
    }
}

// Check for duplicates of files
if (count(array_unique($archiveFiles)) < count($archiveFiles))
{
    exitWithError("Archive contains duplicate file names");
}

// Create archive
$communicator->sendProgress(0, gettext("Creating archive"));
$archiveFileName = outputArchiveFileName($filePathList);
$zipFilePath = "$tempDirectoryPath/$archiveFileName";
$command = "";
if ($configuration->getValue("OutputArchiveFormat") == OUTPUT_ARCHIVE_FORMAT_TAR)
{
    $archiveFilesStr = "";
    for ($i = 0; $i < count($archiveFiles); $i++)
    {
        $archiveFilesStr .= "\"" . normalizeFilePath($archiveFiles[$i]) . "\" ";
    }

    if ($platform == PLATFORM_CENTOS6 || $platform == PLATFORM_CENTOS7 || $platform == PLATFORM_OSX)
    {
        $command = "cd $tempDirectoryPath && tar -cf \"$archiveFileName\" $archiveFilesStr";
    }
    else if ($platform == PLATFORM_WINDOWS7 || $platform == PLATFORM_WINDOWS8 || $platform == PLATFORM_WINDOWS10 )
    {
        $command = "cd /d $tempDirectoryPath && " . __DIR__ . "/tar.exe cf \"$archiveFileName\" $archiveFilesStr";
    }
    else
    {
        exitWithError("Platform is not supported: $platform");
    }
    $ret = false;
    $output = system($command, $ret);
    if ($ret != 0)
    {
        exitWithError("Command failed: command='$command' output='$output'");
    }
}
else if ($configuration->getValue("OutputArchiveFormat") == OUTPUT_ARCHIVE_FORMAT_ZIP)
{
    $archiveFilesStr = "";
    for ($i = 0; $i < count($archiveFiles); $i++)
    {
        $archiveFilesStr .= "\"" . $archiveFiles[$i] . "\" ";
    }

    if ($platform == PLATFORM_CENTOS6 || $platform == PLATFORM_CENTOS7 || $platform == PLATFORM_OSX)
    {
        $command = "cd $tempDirectoryPath && zip -r \"$archiveFileName\" $archiveFilesStr";
    }
    else if ($platform == PLATFORM_WINDOWS7 || $platform == PLATFORM_WINDOWS8 || $platform == PLATFORM_WINDOWS10 )
    {
        $command = "cd /d \"$tempDirectoryPath\" && Winrar.exe a -afzip \"$archiveFileName\" $archiveFilesStr";
    }
    else
    {
        exitWithError("Platform is not supported: $platform");
    }
    $ret = false;
    $output = system($command, $ret);
    if ($ret != 0)
    {
        exitWithError("Command failed: command='$command' output='$output'");
    }
}
else
{
    exitWithError("Invalid output package format: " . $configuration->getValue("OutputArchiveFormat"));
}

// Send file to server
$communicator->sendProgress(0, gettext("Sending to server"));
$remoteFile = basename($zipFilePath);
if (strlen($configuration->getValue("SshDestinationDir")) > 0)
{
    $remoteFile = $configuration->getValue("SshDestinationDir");
    if (substr($remoteFile, strlen($remoteFile) - 1, 1) != "/")
    {
        $remoteFile .= "/";
    }
    $remoteFile .= basename($zipFilePath);
}
for ($i = 0; true; $i++)
{
    if ($i > $configuration->getValue("SenderMaxRetries"))
    {
        exitWithError("Failed to send file to server: $zipFilePath");
    }
    
    if ($i > 0)
    {
        sleep($configuration->getValue("SenderFailDelay"));
    }
    
    // Reconnect to server
    if (($i > 0 || !isset($fileSender)) && !createFileSender($fileSender, $configuration, $logger, "sendProgressCallback"))
    {
        logError("Failed to create file sender");
        continue;
    }
    
    // Upload to server
    if (!$fileSender->send($zipFilePath, $remoteFile))
    {
        logError("Failed to send file with file sender: " . $fileSender->name());
        continue;
    }
    
    break;
}

// Verify upload
if ($configuration->getValue("VerifyUploadChecksum"))
{
    logInfo("Verifying checksum");

    $communicator->sendProgress(100, gettext("Verifying transfer"));
    $localChecksum = md5_file($zipFilePath);
    if (strlen($localChecksum) != 32)
    {
        exitWithError("Wrong checksum on local side: $localChecksum");
    }
    
    for ($i = 0; true; $i++)
    {
        if ($i > $configuration->getValue("SenderMaxRetries"))
        {
            exitWithError("Failed to verify upload");
        }
        
        if ($i > 0)
        {
            sleep($configuration->getValue("SenderFailDelay"));
        }
        
        // Reconnect to server
        if (($i > 0 || !isset($fileSender)) && !createFileSender($fileSender, $configuration, $logger, ""))
        {
            logError("Failed to create file sender");
            continue;
        }
    
        if (!$fileSender->calculateChecksumMd5($remoteChecksum, $remoteFile))
        {
            logError("Failed to calculate checksum of file: " . $configuration->getValue("SshDestinationDir") . "/$fileNameBase.zip");
            continue;
        }
        
        break;
    }
    
    if ($localChecksum != $remoteChecksum)
    {
        exitWithError("Wrong checksum on server side, should be $localChecksum");
    }
}
else
{
    logInfo("Skipped checksum verification");
}

// Send acknowledgement
logInfo("Using commit method: " . $configuration->getValue("CommitAckMethod"));
if ($configuration->getValue("CommitAckMethod") == COMMIT_ACK_METHOD_SHA1_V1)
{
    // Send acknowledgement in as basename.sha1
    // File name example: document.sha1
    // File content example: d1ef1c2cee5d83f608c222b65bf8752b65a6d86c document.zip

    $communicator->sendProgress(100, gettext("Sending acknowledgement"));
    $checksum = sha1_file($zipFilePath);
    if (strlen($checksum) != 40)
    {
        exitWithError("Wrong checksum on local side: $checksum");
    }
    
    $remoteChecksumFile = $remoteFile;
    while (substr($remoteChecksumFile, strlen($remoteChecksumFile) - 1, 1) != ".")
    {
        if (strlen($remoteChecksumFile) == 0)
        {
            exitWithError("Failed to assign name to remote checksum file");
        }
        $remoteChecksumFile = substr($remoteChecksumFile, 0, strlen($remoteChecksumFile) - 1);
    }
    $remoteChecksumFile .= "sha1";
    
    $localChecksumFile = "$tempDirectoryPath/$fileNameBase.sha1";
    file_put_contents($localChecksumFile, "$checksum " . pathinfo($zipFilePath, PATHINFO_BASENAME) . "\n");
    
    for ($i = 0; true; $i++)
    {
        if ($i > $configuration->getValue("SenderMaxRetries"))
        {
            exitWithError("Failed to verify upload");
        }
        
        if ($i > 0)
        {
            sleep($configuration->getValue("SenderFailDelay"));
        }
        
        // Reconnect to server
        if (($i > 0 || !isset($fileSender)) && !createFileSender($fileSender, $configuration, $logger, ""))
        {
            logError("Failed to create file sender");
            continue;
        }
    
        if (!$fileSender->send($localChecksumFile, $remoteChecksumFile))
        {
            logError("Failed to send file with file sender: " . $fileSender->name());
            continue;
        }
        
        break;
    }
}
else if ($configuration->getValue("CommitAckMethod") == COMMIT_ACK_METHOD_NONE)
{
    // Do nothing
}
else
{
    exitWithError("Invalid commit ack method: " . $configuration->getValue("CommitAckMethod"));
}

// Cleanup temp files
$deleteFiles = array();
array_push($deleteFiles, "metadata.xml");
array_push($deleteFiles, "$fileNameBase");
array_push($deleteFiles, $archiveFileName);
for ($i = 0; $i < count($deleteFiles); $i++)
{
    if (strlen($deleteFiles[$i]) > 0 && !deleteFromDisk("$tempDirectoryPath/" . $deleteFiles[$i], false))
    {
        exitWithError("Failed to delete temp files");
    }
}

// Finished successfully
$communicator->sendProgress(100, gettext("Done"));
exitWithSuccess();
?>