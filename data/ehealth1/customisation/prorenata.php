<?php
function preProcessInputData(&$errorMessage, &$filePathList, &$relativeFilePathList, $tempDirectoryPath)
{
    $errorMessage = "";

    // Find root directory for input data
    $rootPath = "";
    for ($i = 0; $i < count($filePathList); $i++)
    {
        $filePath = $filePathList[$i];
        $relativeFilePath = $relativeFilePathList[$i];

        $directoryDepth = substr_count($relativeFilePath, '/');
        if ($directoryDepth == 0)
        {
            if (strlen($rootPath) > 0)
            {
                $errorMessage = "More than one root directory was found";
                return false;
            }
            $rootPath = $filePath;
        }
    }
    if (strlen($rootPath) == 0)
    {
        $errorMessage = "No root directory was found";
        return false;
    }

    // Create destination directory
    $destination = "{$tempDirectoryPath}/input";
    if (!mkdir($destination))
    {
        $errorMessage = "Failed to create directory: {$destination}";
        return false;
    }

    // Run transformation
    $newRootPath = "{$destination}/" . basename($rootPath);
    $runner = new PythonRunner();
    $scriptPath = __DIR__ . "/prorenata_metsmapper.py";
    $arguments = [$rootPath, $destination];
    if (!$runner->executeScript($scriptPath, $arguments))
    {
        $errorMessage = "Failed to generate METS: " . $runner->error();
        return false;
    }

    // Update paths
    // Parse directory recursively and add all files and directories
    $filePathList = [$newRootPath];
    $relativeFilePathList = [basename($newRootPath)];
    $rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($newRootPath, RecursiveDirectoryIterator::SKIP_DOTS), RecursiveIteratorIterator::SELF_FIRST);
    foreach ($rii as $file)
    {
        $filePath = $file->getPathname();
        $relativeFilePath = basename($newRootPath) . "/" . substr($filePath, strlen($newRootPath)+1);

        $filePath = normalizeFilePath($filePath);
        $relativeFilePath = normalizeFilePath($relativeFilePath);

        if (!file_exists($filePath))
        {
            $errorMessage = "The file to be added to archive does not exist: {$filePath}";
            return false;
        }

        array_push($filePathList, $filePath);
        array_push($relativeFilePathList, $relativeFilePath);
    }

    // Add submission agreement
    $submissionAgreementSourcePath = "{$rootPath}/submissionagreement.pdf";
    $submissionAgreementDestinationPath = "{$newRootPath}/" . basename($submissionAgreementSourcePath);
    if (!file_exists($submissionAgreementSourcePath))
    {
        $errorMessage = "Submission agreement not found, expected in: {$submissionAgreementSourcePath}";
        return false;
    }
    if (!copy($submissionAgreementSourcePath, $submissionAgreementDestinationPath))
    {
        $errorMessage = "Failed to add submission agreement";
        return false;
    }
    array_push($filePathList, $submissionAgreementDestinationPath);
    array_push($relativeFilePathList, basename($newRootPath) . "/" . basename($submissionAgreementDestinationPath));

    return true;
}
?>
