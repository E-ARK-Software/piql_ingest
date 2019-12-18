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
include_once("includes/general-functions.php");
include_once("includes/script-context.php");
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
$communicator = new Communicator(false);

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
if (!file_exists($tempDirectoryPath) || !is_dir($tempDirectoryPath))
{
    exitWithError("Temp directory does not exist: " . $tempDirectoryPath);
}

// Cleanup instructions
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

exitWithSuccess();
?>
