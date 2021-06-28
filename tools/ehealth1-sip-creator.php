<?php
include(__DIR__ . '/../includes/ehealth1-sip.php');
include(__DIR__ . '/../includes/general-functions.php');

define("ARCHIVE_TYPE_ZIP", "ZIP");
define("ARCHIVE_TYPE_TAR", "TAR");
define("ARCHIVE_TYPE_NONE", "NONE");

// Help text
if ($argc != 3)
{
    echo "Usage: " . $argv[0] . " <IP directory> <schema directory>" . PHP_EOL;
    exit(1);
}

// IP direcotry
$informationPackageDirectory = $argv[1];
if (!is_dir($informationPackageDirectory))
{
    echo "IP directory is invalid: $informationPackageDirectory" . PHP_EOL;
}

// Schema direcotry
$schemaDirectory = $argv[2];
if (!is_dir($schemaDirectory))
{
    echo "Schema directory is invalid: $schemaDirectory" . PHP_EOL;
}

$outputDirectory = './';
$archiveType = ARCHIVE_TYPE_ZIP;

// Get ID of informationpackage
$parts = explode("_", basename($informationPackageDirectory));
$informationPackageId = $parts[count($parts)-1];

// Define SIP
$outPath;
$sip = new Ehealth1Sip($informationPackageId);

// Schema
$schemaFiles = scandir($schemaDirectory);
if ($schemaFiles === false)
{
    echo "Failed to read schema directory: {$schemaDirectory}";
    exit(1);
}
foreach ($schemaFiles as $schemaFile)
{
    if (substr($schemaFile, 0, 1) == '.')
    {
        // Skipping file
        continue;
    }
    $sip->addSchemaFile("{$schemaDirectory}/{$schemaFile}");
}

// Submission agreement
$submissionAgreementPath = "{$informationPackageDirectory}/Submissionagreement_{$informationPackageId}.pdf";
if (!file_exists($submissionAgreementPath))
{
    echo "Submission agreement does not exist: {$submissionAgreementPath}" . PHP_EOL;
    exit(1);
}
$sip->addSubmissionAgreement($submissionAgreementPath);

// Add files in root level of informationpackage
$files = scandir($informationPackageDirectory);
if ($files === false)
{
    echo "Failed to read IP directory: {$informationPackageDirectory}";
    exit(1);
}
foreach ($files as $file)
{
    $filePath = "{$informationPackageDirectory}/{$file}";
    if (substr($file, 0, 1) == '.')
    {
        // Skipping file
        continue;
    }

    if ($sip->isSubmissionAgreement($filePath))
    {
        // Add submission agreement
        $sip->addSubmissionAgreement($filePath);
    }
    else if ($sip->isDescriptiveMetadata($filePath))
    {
        // Add descriptive metadata
        $sip->addDescriptiveMetadata($filePath);
    }
    else if (is_dir($filePath) && basename($filePath) == 'Data')
    {
        $patientDirectories = scandir($filePath);
        if ($patientDirectories === false)
        {
            echo "Failed to read patient directories from data directory: {$filePath}";
            exit(1);
        }

        foreach ($patientDirectories as $patientDirectory)
        {
            $patientPath = "{$filePath}/{$patientDirectory}";

            if (substr($patientDirectory, 0, 1) == '.')
            {
                // Skipping file
                continue;
            }

            if (!is_dir($patientPath))
            {
                echo "Invalid file: {$patientPath}" . PHP_EOL;
                exit(1);
            }

            // Define patient
            $parts = explode('_', basename($patientPath));
            if (count($parts) != 2)
            {
                echo "Patient directory has invalid naming: {$patientPath}" . PHP_EOL;
                exit(1);
            }
            $patient = new Ehealth1SipPatient($parts[1]);

            $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($patientPath, RecursiveDirectoryIterator::SKIP_DOTS));
            foreach ($rii as $patientFile)
            {
                $patientFilePath = $patientFile->getPathname();
                $relativePatientFilePath = substr($patientFilePath, strlen($patientPath)+1);

                if ($patient->isDescriptiveMetadata($relativePatientFilePath))
                {
                    // Add descriptive metadata
                    $patient->addDescriptiveMetadata($patientFilePath);
                }
                else
                {
                    // Add payload
                    $patient->addFile($patientFilePath, $relativePatientFilePath);
                }
            }

            // Check that patient has data files
            if (count($patient->files()) == 0)
            {
                echo "Patient has no data files" . PHP_EOL;
            }

            // Add patient to SIP
            $sip->addPatient($patient);
        }
    }
    else
    {
        echo "File was not recognized as SIP data: {$filePath}";
        exit(1);
    }
}

// Output SIP
$outPath;
if (!$sip->produceSip($outPath, $outputDirectory))
{
    echo $sip->error() . PHP_EOL;
    echo "Failed to produce SIP" . PHP_EOL;
    return 1;
}

$archivePath = '';
if ($archiveType == ARCHIVE_TYPE_ZIP)
{
    $archivePath = "{$outPath}.zip";
    $zip = new ZipArchive;
    if($zip->open($archivePath, ZipArchive::CREATE) === true)
    {
        $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($outPath, RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::SELF_FIRST);
        foreach ($rii as $file)
        {
            $filePath = $file->getPathname();
            $relativeFilePath = basename($outPath) . "/" . substr($filePath, strlen($outPath)+1);

            if (!file_exists($filePath))
            {
                echo "The file to be added to archive does not exist: {$filePath}" . PHP_EOL;
                exit(1);
            }

            if (is_dir($filePath))
            {
                if (!$zip->addEmptyDir($relativeFilePath))
                {
                    echo "Failed to add directory to zip archive: {$filePath}" . PHP_EOL;
                }
            }
            else
            {
                if (!$zip->addFile($filePath, $relativeFilePath))
                {
                    echo "Failed to add file to zip archive: {$filePath}" . PHP_EOL;
                    exit(1);
                }
            }
        }

        if (!$zip->close())
        {
            echo "Failed to close archive: {$archivePath}" . PHP_EOL;
            exit(1);
        }
    }

    if (!deleteFromDisk($outPath, true))
    {
        echo "Failed to delete SIP: {$outPath}" . PHP_EOL;
        exit(1);
    }
}
else if ($archiveType == ARCHIVE_TYPE_TAR)
{
    $archivePath = "{$outPath}.tar";
    $tar = new PharData($archivePath);

    // Add base directory
    $tar->addEmptyDir(basename($outPath));

    $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($outPath, RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::SELF_FIRST);
    foreach ($rii as $file)
    {
        $filePath = $file->getPathname();
        $relativeFilePath = basename($outPath) . "/" . substr($filePath, strlen($outPath)+1);
echo "File path: " . $filePath . PHP_EOL;
        if (!file_exists($filePath))
        {
            echo "The file to be added to archive does not exist: {$filePath}" . PHP_EOL;
            exit(1);
        }

        if (is_dir($filePath))
        {
            $tar->addEmptyDir($relativeFilePath);
        }
        else
        {
            $tar->addFile($filePath, $relativeFilePath);
        }
    }

    if (!deleteFromDisk($outPath, true))
    {
        echo "Failed to delete SIP: {$outPath}" . PHP_EOL;
        exit(1);
    }
}
else if ($archiveType == ARCHIVE_TYPE_NONE)
{
    // Keep output as is
    $archivePath = $outPath;
}
else
{
    echo "Archive type is invalid: {$achiveType}" . PHP_EOL;
    exit(1);
}

echo "Wrote SIP to $archivePath" . PHP_EOL;
?>
