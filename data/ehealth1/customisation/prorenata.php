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
    $arguments = [$rootPath];
    if (!$runner->executeScript($scriptPath, $arguments))
    {
        $errorMessage = "Failed to generate METS: " . $runner->error();
        return false;
    }

    // Hack to move the output to expected path
    // TODO: Remove this hack when output path can be defined in script
    rename(__DIR__ . "/../../../output/" basename($rootPath), $newRootPath);

    // Set new root path
    for ($i = 0; $i < count($filePathList); $i++)
    {
        $filePath = $filePathList[$i];
        if (substr($filePath, 0, strlen($rootPath)) != $rootPath)
        {
            $errorMessage = "Found file that is not in root directory";
            return false;
        }
        $newFilePath = $newRootPath . substr($filePath, strlen($rootPath));
        $filePathList[$i] = $newFilePath;
    }

    return true;
}
?>
