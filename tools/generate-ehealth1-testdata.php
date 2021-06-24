<?php
include(__DIR__ . '/../includes/general-functions.php');

function createDirectory($path)
{
    if (is_dir($path))
    {
        echo "Directory already exists: {$path}" . PHP_EOL;
        exit(1);
    }

    if (!mkdir($path))
    {
        echo "Failed to create directory: {$path}" . PHP_EOL;
        exit(1);
    }

    echo "Created directory: {$path}" . PHP_EOL;
}

function createFile($path)
{
    $content = substr(md5(rand()), 0, 10);
    if (file_put_contents($path, $content, LOCK_EX) === false)
    {
        echo "Failed to write file: {$path}" . PHP_EOL;
        exit (1);
    }

    echo "Wrote file: {$path}" . PHP_EOL;
}

echo "Generating eHealth1 testdata" . PHP_EOL;

// Create test data directory
$testDataDirectory = "ehealth1_testdata";
if (!is_dir($testDataDirectory))
{
    createDirectory($testDataDirectory);
}

// Create schemas
$schemasDirectory = "{$testDataDirectory}/schemas";
if (!is_dir($schemasDirectory))
{
    createDirectory($schemasDirectory);
}
createFile("{$schemasDirectory}/Fhir.condition.xsd");
createFile("{$schemasDirectory}/Fhir.patient.xsd");
createFile("{$schemasDirectory}/PREMIS.xsd");

// Generate informationpackage ID
$informationPackageId = guidv4(openssl_random_pseudo_bytes(16));

// Create base directory for informationpackage
$informationPackageDirectory = "{$testDataDirectory}/Informationpackage_{$informationPackageId}";
createDirectory($informationPackageDirectory);

// Create submission agreement
createFile("{$informationPackageDirectory}/Submissionagreement_{$informationPackageId}.pdf");

// Create descriptive metadata
createFile("{$informationPackageDirectory}/Patients.xml");

// Create data directory
$dataDirectory = "{$informationPackageDirectory}/Data";
createDirectory($dataDirectory);

// Create patient data
$patients = 6;
$casesPerPatient = 5;
$subcasesPerCase = 4;
$documentsPerSubCase = 3;
$filesPerDocument = 2;
for ($i = 0; $i < $patients; $i++)
{
    $patientId = guidv4(openssl_random_pseudo_bytes(16));

    // Create patient direcotry
    $patientDirectory = "{$dataDirectory}/Patientrecord_{$patientId}";
    createDirectory($patientDirectory);

    // Create patient metadata
    createFile("{$patientDirectory}/Fhircondition_{$patientId}.xml");

    // Create cases
    for ($j = 0; $j < $casesPerPatient; $j++)
    {
        $caseId = guidv4(openssl_random_pseudo_bytes(16));

        // Create case directory
        $caseDirectory = "{$patientDirectory}/{$caseId}";
        createDirectory($caseDirectory);

        // Create subcases
        for ($k = 0; $k < $subcasesPerCase; $k++)
        {
            $subCaseId = guidv4(openssl_random_pseudo_bytes(16));

            // Create subcase directory
            $subCaseDirectory = "{$caseDirectory}/{$subCaseId}";
            createDirectory($subCaseDirectory);

            // Create documents
            for ($l = 0; $l < $documentsPerSubCase; $l++)
            {
                $documentId = guidv4(openssl_random_pseudo_bytes(16));

                // Create document directory
                $documentDirectory = "{$subCaseDirectory}/{$documentId}";
                createDirectory($documentDirectory);

                // Create document files
                for ($m = 0; $m < $filesPerDocument; $m++)
                {
                    createFile("{$documentDirectory}/file{$m}.pdf");
                }
            }
        }
    }
}

echo "Finished creating testdata" . PHP_EOL;
?>
