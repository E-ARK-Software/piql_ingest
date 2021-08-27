<?php
try {
    // Includes
    //

    include_once("includes/error-handler.php");
    include_once("includes/defines.php");
    include_once("includes/configuration.php");
    include_once("includes/communicator.php");
    include_once("includes/platforms.php");
    include_once("includes/parameters.php");
    include_once("includes/filetransfer/file-transfer-base.php");
    include_once("includes/filetransfer/file-transfer-ssh.php");
    include_once("includes/filetransfer/file-transfer-sftp.php");
    include_once("includes/filetransfer/file-transfer-disk.php");
    include_once("includes/logger.php");
    include_once("includes/metadata-parser.php");
    include_once("includes/general-functions.php");
    include_once("includes/script-context.php");
    include_once("includes/ehealth1-sip.php");
    include_once("includes/python-runner.php");
    include_once("metadata-description.php");
    include_once("metadata-description-package.php");
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
        // Normalize directory separators
        $filePathList[$i] = normalizeFilePath($filePathList[$i]);

        // Check that file exists on disk
        if (!file_exists($filePathList[$i]))
        {
            exitWithError("File does not exist on disk: " . $filePathList[$i]);
        }

        // Remove trailing slash on directories
        $filePathList[$i] = rtrim($filePathList[$i], '/');
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
        // Normalize directory separators
        $relativeFilePathList[$i] = normalizeFilePath($relativeFilePathList[$i]);

        // Remove trailing slash on directories
        $relativeFilePathList[$i] = rtrim($relativeFilePathList[$i], '/');
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

    // Get package metadata
    if (!$scriptContext->valueAsString($packageMetadata, "PackageMetadata"))
    {
        // Package metadata is not mandatory
        $packageMetadata = "";
    }

    // Functions
    //

    function logError($text)
    {
        global $logger;
        $logger->logError($text);
    }

    function logInfo($text)
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
            $fondPosition = -1;
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
        $text .= "BagIt-Version: " . $version . "\n";
        $text .= "Tag-File-Character-Encoding: UTF-8" . "\n";
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
                $text .= md5_file($filePath) . " " . "data/" . $relativeFilePathList[$i] . "\n";
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

    function isSubmissionAgreement($path)
    {
        $fileName = basename($path);

        // Check file extension
        if (strtolower(pathinfo($fileName, PATHINFO_EXTENSION)) != 'pdf')
        {
            return false;
        }

        // Check that fileName starts with "SA-"
        if (substr($fileName, 0, 3) != 'SA-')
        {
            return false;
        }

        // Validate OID
        $oid = substr($fileName, 3, strlen($fileName) - 7);
        if (strlen($oid) < 1)
        {
            return false;
        }
        for ($i = 0; $i < strlen($oid); $i++)
        {
            if ($oid[$i] != '.' && !is_numeric($oid[$i]))
            {
                return false;
            }
        }

        return true;
    }

    // Commit instructions
    //

    // Identify platform
    $platforms = new Platforms();
    $platforms->addSupported(PLATFORM_CENTOS6);
    $platforms->addSupported(PLATFORM_CENTOS7);
    $platforms->addSupported(PLATFORM_RHEL7);
    $platforms->addSupported(PLATFORM_RHEL8);
    $platforms->addSupported(PLATFORM_WINDOWS7);
    $platforms->addSupported(PLATFORM_WINDOWS8);
    $platforms->addSupported(PLATFORM_WINDOWS10);
    $platforms->addSupported(PLATFORM_OSX);

    $platform = $platforms->identify($os, $distribution);
    if (!$platforms->isSupported($platform))
    {
        exitWithError("Unsupported platform: platform='$platform' os='$os' distribution='$distribution'");
    }
    logInfo("Identified OS: {$platform}");

    // Pre-process input data
    if ($configuration->exists("PreProcessInputDataScript"))
    {
        $communicator->sendProgress(0, gettext("Pre-processing data"));

        $scriptPath = $configuration->getValue("PreProcessInputDataScript");
        include_once($scriptPath);

        if (!preProcessInputData($errorMessage, $filePathList, $relativeFilePathList, $tempDirectoryPath))
        {
            exitWithError("Failed to pre-process input data: {$errorMessage}");
        }
    }

    // Build data for archive
    logInfo("Building SIP");
    $communicator->sendProgress(0, gettext("Preparing data"));
    $archiveFiles = array();
    {
        // Create package files
        if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_EHEALTH1_SIP)
        {
            // Create a SIP compliant to the eHealth1 standard
            // The payload can contain files or directory stuctures
            //
            // SIP structure:
            //   {TRANSFER_NAME}
            //    mets.xml
            //    metadata/
            //     descriptive/
            //      Patients.xml
            //     preservation/
            //    schemas/
            //     condition.xsd
            //     DILCISExtensionMETS.xsd
            //     DILCISExtensionSIPMETS.xsd
            //     patient.xsd
            //     xlink.xsd
            //     xml.xsd
            //    documentation/
            //     submissionagreement.pdf
            //    representations/
            //     Patientrecord_{ID}
            //      mets.xml
            //      metadata/
            //       descriptive/
            //        Patient1_condition.xml
            //       preservation/
            //      data/
            //       {PAYLOAD}
            //
            // Input structure:
            //   eHealth1_Transfer_{ID}
            //    Patients.xml
            //    submissionagreement.pdf
            //    patientrecord_1/
            //     Patient1_condition.xml
            //     Patient1Case1/
            //      Patient1Case1Document1/
            //       patient1_record1.pdf
            //     Patient1Case2/
            //      Patient1Case2Document1/
            //       patient1_record2.pdf
            //    patientrecord_2/
            //     Patient2_condition.xml
            //     Patient2Case1/
            //      Patient2Case1Subcase1/
            //       Patient2Case1Subcase1Document1/
            //        patient2_record1.pdf
            //       Patient2Case1Subcase1Document2/
            //        patient2_record2.pdf

            // Define SIP
            $sip = new Ehealth1Sip();

            // Populate files in SIP
            for ($i = 0; $i < count($filePathList); $i++)
            {
                $filePath = $filePathList[$i];
                $relativeFilePath = $relativeFilePathList[$i];

                $directoryDepth = substr_count($relativeFilePath, '/');
                if ($directoryDepth == 0)
                {
                    // This is a SIP
                    //

                    // Set transfer name
                    if (strlen($sip->transferName()) > 0)
                    {
                        exitWithError('Only one SIP per commit is allowed');
                    }
                    $parts = explode('_', $relativeFilePath);
                    if (count($parts) == 0)
                    {
                        exitWithError("SIP name is in wrong format: {$relativeFilePath}");
                    }
                    $sip->setTransferName($relativeFilePath);
                }
                else if ($directoryDepth == 1)
                {
                    // This file should be added to SIP
                    //

                    if ($sip->isSubmissionAgreement($relativeFilePath))
                    {
                        // Add submission agreement
                        $sip->addSubmissionAgreement($filePath);
                    }
                    else if ($sip->isDescriptiveMetadata($relativeFilePath))
                    {
                        // Add descriptive metadata
                        $sip->addDescriptiveMetadata($filePath);
                    }
                    else if (is_dir($filePath) && $sip->isPatientDirectory($relativeFilePath))
                    {
                        // This is a patient data folder
                        //

                        // Set patient ID
                        $parts = explode('_', basename($filePath));
                        $patientId = '';
                        if (count($parts) >= 2)
                        {
                            $patientId = $parts[count($parts)-1];
                        }
                        else
                        {
                            $patientId = guidv4(openssl_random_pseudo_bytes(16));
                        }
                        $patient = new Ehealth1SipPatient($patientId);

                        // Add files
                        $patientDirectoryPath = $filePath;
                        for ($j = 0; $j < count($filePathList); $j++)
                        {
                            $filePath = $filePathList[$j];
                            $relativeFilePath = $relativeFilePathList[$j];

                            // Check if file is part of this patient directory
                            if (substr($filePath, 0, strlen($patientDirectoryPath)) != $patientDirectoryPath || $filePath == $patientDirectoryPath)
                            {
                                continue;
                            }

                            if ($patient->isDescriptiveMetadata($relativeFilePath))
                            {
                                // Add descriptive metadata
                                $patient->addDescriptiveMetadata($filePath);
                            }
                            else
                            {
                                // Add payload
                                $patient->addFile($filePath, substr($filePath, strlen($patientDirectoryPath)+1));
                            }
                        }

                        // Check that patient has data files
                        if (count($patient->files()) == 0)
                        {
                            exitWithError("Patient has no data files");
                        }

                        // Add patient to SIP
                        $sip->addPatient($patient);
                    }
                    else
                    {
                        exitWithError("File was not recognized as SIP data: {$relativeFilePath}");
                    }
                }
            }

            // Add schemas
            $schemaDirectory = $configuration->getValue("Ehealth1SipSchemaDirectory");
            if (!is_dir($schemaDirectory))
            {
                exitWithError("Schema directory could not be found: {$schemaDirectory}");
            }
            $schemaFiles = scandir($schemaDirectory);
            if ($schemaFiles === false)
            {
                exitWithError("Failed to read schema directory: {$schemaDirectory}");
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

            // Populate package metadata
            $metadataParser = new MetadataParser();
            if (!$metadataParser->readString($packageMetadata, $errorMessage))
            {
                exitWithError($errorMessage);
            }
            $metadataList = $metadataParser->getMetadata();
            if (count($metadataList) == 0)
            {
                exitWithError("No package metadata was submitted");
            }
            foreach ($metadataList as $metadataItem)
            {
                $sip->addPackageMetadata($metadataItem->key(), $metadataItem->value());
            }

            // Check that the SIP has an ID
            if (strlen($sip->transferName()) == 0)
            {
                exitWithError('The SIP does not have an ID');
            }

            // Check that SIP contains exactly one submission agreement
            $submissionAgreementCount = count($sip->submissionAgreements());
            if ($submissionAgreementCount != 1)
            {
                exitWithError("One submission agreement expected, {$submissionAgreementCount} found");
            }

            // Check that SIP contains exactly one document of descriptive metadata
            $descriptiveMetadataCount = count($sip->descriptiveMetadata());
            if ($descriptiveMetadataCount != 1)
            {
                exitWithError("One document of descriptive metadata expected, {$descriptiveMetadataCount} found");
            }

            // Check that SIP contains schema files
            if (count($sip->schemaFiles()) == 0)
            {
                exitWithError("SIP does not contain schema files");
            }

            // Produce SIP
            if (!$sip->produceSip($outPath, $tempDirectoryPath))
            {
                exitWithError('Producing SIP gave following error: ' . $sip->error());
            }

            // Setup path for SIP to be archived
            array_push($archiveFiles, basename($outPath));
        }
        else if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_NHA_OTHER)
        {
            // Create a SIP compliant to the NHA 'other' SIP package type
            //
            // SIP structure:
            //   {UUID}/
            //    OTHER/
            //     journal/
            //       avlxml.xml
            //       {SUBMISSION_AGREEMENT}
            //     objekter/
            //       {PAYLOAD}
            //    metadata/
            //     checksum.md5
            //     metadata.csv
            //     identifiers.json

            $checksumPaths = array();

            // Get info from package metadata
            $metadataParser = new MetadataParser();
            if (!$metadataParser->readString($packageMetadata, $errorMessage))
            {
                exitWithError($errorMessage);
            }
            if (!$metadataParser->getValue("avleveringsidentifikator", 0, $avleveringsidentifikator) ||
                !$metadataParser->getValue("avleveringsbeskrivelse", 0, $avleveringsbeskrivelse) ||
                !$metadataParser->getValue("avtaleidentifikator", 0, $avtaleidentifikator) ||
                !$metadataParser->getValue("avtaledato", 0, $avtaledato) ||
                !$metadataParser->getValue("avtalebeskrivelse", 0, $avtalebeskrivelse) ||
                !$metadataParser->getValue("virksomhetsnavn", 0, $virksomhetsnavn) ||
                !$metadataParser->getValue("foretaksnavn", 0, $foretaksnavn) ||
                !$metadataParser->getValue("organisasjonsnummer", 0, $organisasjonsnummer))
            {
                exitWithError("Failed to read values from metadata");
            }

            // Create SIP root directory
            $sipPath = $tempDirectoryPath . "/" . guidv4(openssl_random_pseudo_bytes(16));
            if (!mkdir($sipPath))
            {
                exitWithError("Failed to create directory: $sipPath");
            }

            // Create metadata directory
            $metadataPath = $sipPath . "/metadata";
            if (!mkdir($metadataPath))
            {
                exitWithError("Failed to create directory: $metadataPath");
            }

            // Create other directory
            $otherPath = $sipPath . "/OTHER";
            if (!mkdir($otherPath))
            {
                exitWithError("Failed to create directory: $otherPath");
            }

            // Create journal directory
            $journalPath = $otherPath . "/journal";
            if (!mkdir($journalPath))
            {
                exitWithError("Failed to create directory: $journalPath");
            }

            // Create objects directory
            $objectsPath = $otherPath . "/objekter";
            if (!mkdir($objectsPath))
            {
                exitWithError("Failed to create directory: $objectsPath");
            }

            // Create AVLXML
            $avlxmlPath = $otherPath . "/journal/avlxml.xml";
            array_push($checksumPaths, "OTHER/journal/avlxml.xml");
            $avlxml = "";
            $avlxml .= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" . "\n";
            $avlxml .= "<avlxml xmlns=\"http://www.arkivverket.no/standarder/nha/avlxml\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.arkivverket.no/standarder/nha/avlxml avlxml.xsd\">" . "\n";
            $avlxml .= "        <avlxmlversjon>2.16.578.1.39.100.5.2.2</avlxmlversjon>" . "\n";
            $avlxml .= "        <avleveringsidentifikator>" . htmlspecialchars($avleveringsidentifikator) . "</avleveringsidentifikator>" . "\n";
            $avlxml .= "        <avleveringsbeskrivelse>" . htmlspecialchars($avleveringsbeskrivelse) . "</avleveringsbeskrivelse>" . "\n";
            $avlxml .= "        <arkivskaper>NHA</arkivskaper>" . "\n";
            $avlxml .= "        <avtale>" . "\n";
            $avlxml .= "                <avtaleidentifikator>" . htmlspecialchars($avtaleidentifikator) . "</avtaleidentifikator>" . "\n";
            $avlxml .= "                <avtaledato>" . htmlspecialchars($avtaledato) . "</avtaledato>" . "\n";
            $avlxml .= "                <avtalebeskrivelse>" . htmlspecialchars($avtalebeskrivelse) . "</avtalebeskrivelse>" . "\n";
            $avlxml .= "                <virksomhet>" . "\n";
            $avlxml .= "                        <virksomhetsnavn>" . htmlspecialchars($virksomhetsnavn) . "</virksomhetsnavn>" . "\n";
            $avlxml .= "                        <foretaksnavn>" . htmlspecialchars($foretaksnavn) . "</foretaksnavn>" . "\n";
            $avlxml .= "                        <organisasjonsnummer>" . htmlspecialchars($organisasjonsnummer) . "</organisasjonsnummer>" . "\n";
            $avlxml .= "                </virksomhet>" . "\n";
            $avlxml .= "        </avtale>" . "\n";
            $avlxml .= "</avlxml>" . "\n";
            if (file_put_contents($avlxmlPath, $avlxml) === false)
            {
                exitWithError("Failed to write to file: " . $avlxmlPath);
            }

            // Create identifiers.json
            $identifiersPath = "$metadataPath/identifiers.json";
            $identifiersOut = "";
            $identifiersOut .= "[ { \"file\": \"objects/OTHER/journal/avlxml.xml\", \"identifiers\": [" . "\n";
            $identifiersOut .= "          {" . "\n";
            $identifiersOut .= "            \"identifierType\": \"avleveringsidentifikator\"," . "\n";
            $identifiersOut .= "            \"identifier\": \"" . $avleveringsidentifikator . "\"" . "\n";
            $identifiersOut .= "          }," . "\n";
            $identifiersOut .= "          {" . "\n";
            $identifiersOut .= "            \"identifierType\": \"avtaleidentifikator\"," . "\n";
            $identifiersOut .= "            \"identifier\": \"" . $avtaleidentifikator . "\"" . "\n";
            $identifiersOut .= "          }," . "\n";
            $identifiersOut .= "          {" . "\n";
            $identifiersOut .= "            \"identifierType\": \"organisasjonsnummer\"," . "\n";
            $identifiersOut .= "            \"identifier\": \"" . $organisasjonsnummer . "\"" . "\n";
            $identifiersOut .= "          }" . "\n";
            $identifiersOut .= "        ]" . "\n";
            $identifiersOut .= "    }" . "\n";
            $identifiersOut .= "]" . "\n";
            if (file_put_contents($identifiersPath, $identifiersOut) === false)
            {
                exitWithError("Failed to write to file: " . $identifiersPath);
            }

            // Add submission agreement
            $submissionAgreements = 0;
            for ($fileIndex = 0; $fileIndex < count($filePathList); $fileIndex++)
            {
                // Get file info
                $filePath = $filePathList[$fileIndex];
                $fileItemType = $fileItemTypeList[$fileIndex];

                // Skip files that are not submission agreements
                if (!isSubmissionAgreement($filePath))
                {
                    continue;
                }

                // Copy file
                $submissionAgreementPath = $otherPath . "/journal/" . basename($filePath);
                array_push($checksumPaths, "OTHER/journal/" . basename($filePath));
                if (!copy($filePath, $submissionAgreementPath))
                {
                    exitWithError("Failed to copy file $filePath to $submissionAgreementPath");
                }
                $submissionAgreements++;
            }
            if ($submissionAgreements < 1)
            {
                exitWithError("Failed to find submission agreement");
            }
            if ($submissionAgreements > 1)
            {
                exitWithError("Found more than one submission agreement");
            }

            // Remove submission agreement metadata
            $temporaryMetadataList = $metadataList;
            $temporaryFileNames = $fileNames;
            for ($i = 0; $i < count($fileNames); $i++)
            {
                if (!isSubmissionAgreement($fileNames[$i]))
                {
                    array_push($temporaryFileNames, $fileNames[$i]);
                    array_push($temporaryMetadataList, $metadataList[$i]);
                }
            }

            // Create metadata file
            $temporaryFileNames = array();
            $temporaryMetadataList = array();
            for ($i = 0; $i < count($filePathList); $i++)
            {
                if (!isSubmissionAgreement($filePathList[$i]))
                {
                    array_push($temporaryFileNames, basename($filePathList[$i]));
                    array_push($temporaryMetadataList, $metadataList[$i]);
                }
            }
            $metadataOut = "$metadataPath/metadata.csv";
            if (!createMetadata($temporaryMetadataList, $metadataOut, $temporaryFileNames, $metadataTemplate, $configuration))
            {
                exitWithError("Failed to create metadata file");
            }

            // Fetch objects
            for ($fileIndex = 0; $fileIndex < count($filePathList); $fileIndex++)
            {
                // Get file info
                $filePath = $filePathList[$fileIndex];
                $fileItemType = $fileItemTypeList[$fileIndex];

                // Skip submission agreement
                if (isSubmissionAgreement($filePath))
                {
                    continue;
                }

                if ($fileItemType != FILE_ITEM_TYPE_FILE)
                {
                    exitWithError("Invalid file item type: " . $fileItemType);
                }

                $destination = $objectsPath . "/" . basename($filePath);
                if (!copy($filePath, $destination))
                {
                    exitWithError("Failed to copy file $filePath to $destination");
                }
                array_push($checksumPaths, "OTHER/objekter/" . basename($filePath));
            }

            // Create checksum file
            $checksumFilePath = $metadataPath . "/checksum.md5";
            foreach ($checksumPaths as $relativePath)
            {
                $absolutePath = $sipPath . "/" . $relativePath;

                $checksum = md5_file($absolutePath);
                if ($checksum === false)
                {
                    exitWithError("Failed to calculate checksum for file: " . $absolutePath);
                }

                if (file_put_contents($checksumFilePath, $checksum . "  " . $relativePath . "\n", FILE_APPEND | LOCK_EX) === false)
                {
                    exitWithError("Failed to write to file: " . $checksumFilePath);
                }
            }

            // Setup path for SIP to be archived
            array_push($archiveFiles, basename($sipPath));

            // Create AVLXML item on server
            $serverAddress = $configuration->getValue("ProductionServerAddress");
            $url="http://$serverAddress/api/v1/prod/avlxml";
            $ch = curl_init("$url");
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0); // TODO REMOVE ME!
            curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0); // TODO REMOVE ME!
            curl_setopt($ch, CURLOPT_POST, 1);
            $avlxml=json_encode($avlxml);
            $post = [
                'data' => "{\"xml\": $avlxml, \"validate\" : \"0\", \"indextype\" : \"\", \"avlxmltype\" : \"3\" }",
            ];
            curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
            $data = curl_exec($ch);
            if (!$data)
            {
                exitWithError("Failed to register avlxml in server");
            }
            $response = curl_getinfo($ch, CURLINFO_RESPONSE_CODE);
            curl_close($ch);
            if ( "$response" != "201" )
            {
                exitWithError("Got error response from production server: $response");
            }
        }
        else if ($configuration->getValue("OutputFormat") == OUTPUT_FORMAT_BAGIT_V097)
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

            $dataRootDirectory = "";
            $dataDirectory = "";
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
                
                    $dataRootDirectory = "$tempDirectoryPath";

                    // Create data directory
                    $dataDirectory = "$dataRootDirectory/data";
                    if (!mkdir($dataDirectory, 0777, false))
                    {
                        exitWithError("Failed to create data directory");
                    }
                    array_push($archiveFiles, basename($dataDirectory));

                    // Create metadata directory
                    $metadataDirectory = "$dataRootDirectory/metadata";
                    if (!mkdir($metadataDirectory, 0777, false))
                    {
                        exitWithError("Failed to create metadata directory");
                    }

                    // Create metadata file
                    $fileNames = array();
                    for ($i = 0; $i < count($filePathList); $i++)
                    {
                        array_push($fileNames, basename($filePathList[$i]));
                    }
                    $metadataOut = "$metadataDirectory/metadata.csv";
                    if (!createMetadata($metadataList, $metadataOut, $fileNames, $metadataTemplate, $configuration))
                    {
                        exitWithError("Failed to create metadata file");
                    }

                    // Include metadata if generated
                    if (is_file($metadataOut))
                    {
                        array_push($archiveFiles, basename($metadataDirectory));
                    }

                    // Create BagIt description file
                    $descriptionOut = "$dataRootDirectory/bagit.txt";
                    if (!createBagitDescriptionFile($descriptionOut, "0.97"))
                    {
                        exitWithError("Failed to create BagIt description file");
                    }
                    array_push($archiveFiles, basename($descriptionOut));

                    // Create checksum manifest
                    $checksumManifestOut = "$dataRootDirectory/manifest-md5.txt";
                    if (!createChecksumManifestFile($checksumManifestOut, $filePathList, $relativeFilePathList))
                    {
                        exitWithError("Failed to create checksum manifest file");
                    }
                    array_push($archiveFiles, basename($checksumManifestOut));
                }

                // Add data
                $source = $filePath;
                $destination = "$dataDirectory/$relativeFilePath";
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

            $dataRootDirectory = "";
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
                    $dataRootDirectory = "$tempDirectoryPath/$fileNameBase";
                    if (file_exists($dataRootDirectory))
                    {
                        exitWithError("Directory already exists: {$dataRootDirectory}");
                    }
                    if (!mkdir($dataRootDirectory, 0777, false))
                    {
                        exitWithError("Failed to create data root directory: $dataRootDirectory");
                    }

                    // Create metadata directory
                    $metadataDirectory = "$dataRootDirectory/metadata";
                    if (file_exists($metadataDirectory))
                    {
                        exitWithError("Directory already exists: {$metadataDirectory}");
                    }
                    if (!mkdir($metadataDirectory, 0777, false))
                    {
                        exitWithError("Failed to create metadata directory");
                    }

                    // Create metadata file
                    $fileNames = array();
                    for ($i = 0; $i < count($filePathList); $i++)
                    {
                        array_push($fileNames, basename($filePathList[$i]));
                    }
                    $metadataOut = "$metadataDirectory/metadata.csv";
                    createMetadata($metadataList, $metadataOut, $fileNames, $metadataTemplate, $configuration);

                    array_push($archiveFiles, $fileNameBase);
                }

                // Add data
                $source = $filePath;
                $destination = "$dataRootDirectory/$fileName";
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
                            $directoryPath = "$tempDirectoryPath/$relativeFilePath";
                            if (substr($directoryPath, strlen($directoryPath) - 1) == "/")
                            {
                                $directoryPath = substr($directoryPath, 0, strlen($directoryPath) - 1);
                            }

                            if (file_exists($directoryPath))
                            {
                                exitWithError("Directory already exists: $directoryPath");
                            }
                            else
                            {
                                if (!mkdir($directoryPath, 0777, true))
                                {
                                    exitWithError("Failed to create directory: $directoryPath");
                                }
                                array_push($archiveFiles, $directoyPath);
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
                        $directoryPath = "$tempDirectoryPath";
                        for ($j = 0; $j <= $i; $j++)
                        {
                            $directoryPath .= "/" . $pathComponents[$j];
                        }

                        if (!is_dir($directoryPath))
                        {
                            if (!mkdir($directoryPath, 0777, true))
                            {
                                exitWithError("Failed to create directory: $directoryPath");
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
    logInfo("Done building SIP");

    // Check for duplicates of files
    if (count(array_unique($archiveFiles)) < count($archiveFiles))
    {
        exitWithError("Archive contains duplicate file names");
    }

    // Create archive
    logInfo("Pack SIP");
    $communicator->sendProgress(0, gettext("Creating archive"));
    $archiveFileName = outputArchiveFileName($filePathList);
    $archiveFilePath = "$tempDirectoryPath/$archiveFileName";
    if ($configuration->getValue("OutputArchiveFormat") == OUTPUT_ARCHIVE_FORMAT_TAR)
    {
        try {
            $tar = new PharData($archiveFilePath);
            for ($i = 0; $i < count($archiveFiles); $i++)
            {
                $archiveFile = "{$tempDirectoryPath}/{$archiveFiles[$i]}";
                if (!file_exists($archiveFile))
                {
                    exitWithError("Failed to TAR since it doesn't exist: " . $archiveFiles[$i]);
                }
                else if (is_dir($archiveFile))
                {
                    // Add base directory
                    $tar->addEmptyDir(basename($archiveFile));

                    // Parse directory recursively and add all files and directories
                    // Recursive building in PharData is not used since we then leave out the empty directories
                    $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($archiveFile, RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::SELF_FIRST);
                    foreach ($rii as $file)
                    {
                        $filePath = $file->getPathname();
                        $relativeFilePath = basename($archiveFile) . "/" . substr($filePath, strlen($archiveFile)+1);

                        if (!file_exists($filePath))
                        {
                            exitWithError("The file to be added to archive does not exist: {$filePath}");
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
                }
                else
                {
                    $filePath = $archiveFile;
                    $relativeFilePath = basename($archiveFile);
                    $tar->addFile($filePath, $relativeFilePath);
                }
            }
        } catch (Exception $e) {
            exitWithError("Failed to create TAR archive: " . $e->getMessage());
        }
    }
    else if ($configuration->getValue("OutputArchiveFormat") == OUTPUT_ARCHIVE_FORMAT_ZIP)
    {
        // Open archive for writing
        $zip = new ZipArchive;
        if($zip->open($archiveFilePath, ZipArchive::CREATE) !== true)
        {
            exitWithError("Failed to open file to create zip: {$archiveFilePath}");
        }

        // Add files to archive
        for ($i = 0; $i < count($archiveFiles); $i++)
        {
            $archiveFile = "{$tempDirectoryPath}/{$archiveFiles[$i]}";
            if (!file_exists($archiveFile))
            {
                exitWithError("Failed to zip since it doesn't exist: " . $archiveFiles[$i]);
            }
            else if (is_dir($archiveFile))
            {
                // Add base directory
                $zip->addEmptyDir(basename($archiveFile));

                // Parse directory recursively and add all files and directories
                // Recursive building in ZipArchive is not used since we then leave out the empty directories
                $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($archiveFile, RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::SELF_FIRST);
                foreach ($rii as $file)
                {
                    $filePath = $file->getPathname();
                    $relativeFilePath = basename($archiveFile) . "/" . substr($filePath, strlen($archiveFile)+1);

                    if (!file_exists($filePath))
                    {
                        exitWithError("The file to be added to archive does not exist: {$filePath}");
                    }

                    if (is_dir($filePath))
                    {
                        if (!$zip->addEmptyDir($relativeFilePath))
                        {
                            exitWithError("Failed to add directory to zip archive: {$filePath}");
                        }
                    }
                    else
                    {
                        if (!$zip->addFile($filePath, $relativeFilePath))
                        {
                            exitWithError("Failed to add file to zip archive: {$filePath}");
                        }
                    }
                }
            }
            else
            {
                $filePath = $archiveFile;
                $relativeFilePath = basename($archiveFile);
                if (!$zip->addFile($filePath, $relativeFilePath))
                {
                    exitWithError("Failed to add file to zip archive: {$filePath}");
                }
            }
        }
        if (!$zip->close())
        {
            exitWithError("Failed to close zip file: {$archiveFilePath}");
        }
    }
    else
    {
        exitWithError("Invalid output package format: " . $configuration->getValue("OutputArchiveFormat"));
    }
    logInfo("Done packing SIP");

    // Send file to server
    $communicator->sendProgress(0, gettext("Sending to server"));
    $remoteFile = basename($archiveFilePath);
    if (($configuration->getValue("FileSendMethod") == FILE_SEND_METHOD_SFTP || $configuration->getValue("FileSendMethod") == FILE_SEND_METHOD_SSH) && strlen($configuration->getValue("SshDestinationDir")) > 0)
    {
        $remoteFile = $configuration->getValue("SshDestinationDir");
        if (substr($remoteFile, strlen($remoteFile) - 1, 1) != "/")
        {
            $remoteFile .= "/";
        }
        $remoteFile .= basename($archiveFilePath);
    }
    for ($i = 0; true; $i++)
    {
        if ($i > $configuration->getValue("SenderMaxRetries"))
        {
            exitWithError("Failed to send file to server: $archiveFilePath");
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
        if (!$fileSender->send($archiveFilePath, $remoteFile))
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
        $localChecksum = md5_file($archiveFilePath);
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
                logError("Failed to calculate checksum of file: " . $configuration->getValue("SshDestinationDir") . "/" . basename($remoteFile));
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
    logInfo("Using commit ack method: " . $configuration->getValue("CommitAckMethod"));
    if ($configuration->getValue("CommitAckMethod") == COMMIT_ACK_METHOD_NHA_OTHER)
    {
        // Send acknowledgement in as basename.md5
        // File name example: document.md5
        // File content example: 14b68e4f906cf498f9c5e3d51dc7c9f0  document.mft

        $communicator->sendProgress(100, gettext("Sending acknowledgement"));
        $checksum = md5_file($archiveFilePath);
        if (strlen($checksum) != 32)
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
        $remoteChecksumFile .= "md5";

        $localChecksumFile = "$tempDirectoryPath/$fileNameBase.md5";
        file_put_contents($localChecksumFile, "$checksum  " . substr(basename($archiveFilePath), 0, strlen(basename($archiveFilePath)) - 3) . "tar" . "\n");

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

        // Set status on server
        $serverAddress = $configuration->getValue("ProductionServerAddress");
        $url="http://$serverAddress/api/v1/prod/avlxml/$avleveringsidentifikator/status";
        $ch = curl_init("$url");
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0); // TODO REMOVE ME!
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0); // TODO REMOVE ME!
        curl_setopt($ch, CURLOPT_POST, 1);
        $post = [
            'data' => "{\"operation\": \"sip-ingest-other\", \"result\": \"ok\", \"description\": \"\" }",
        ];
        curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
        $data = curl_exec($ch);
        if (!$data)
        {
            exitWithError("Failed to register status in server");
        }
        $response = curl_getinfo($ch, CURLINFO_RESPONSE_CODE);
        curl_close($ch);
        if ( "$response" != "200" )
        {
            exitWithError("Got error response from production server: $response");
        }
    }
    else if ($configuration->getValue("CommitAckMethod") == COMMIT_ACK_METHOD_SHA1_V1)
    {
        // Send acknowledgement in as basename.sha1
        // File name example: document.sha1
        // File content example: d1ef1c2cee5d83f608c222b65bf8752b65a6d86c document.zip

        $communicator->sendProgress(100, gettext("Sending acknowledgement"));
        $checksum = sha1_file($archiveFilePath);
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
        file_put_contents($localChecksumFile, "$checksum " . pathinfo($archiveFilePath, PATHINFO_BASENAME) . "\n");

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
} catch (Exception $e) {
    ob_end_clean();
    echo "ERROR Exception thrown: " . $e->getMessage();
}
?>
