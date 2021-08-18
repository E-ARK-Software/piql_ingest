<?php
include_once(__DIR__ . "/python-runner.php");

class Ehealth1SipPatient
{
    private $m_DataFilePaths = [];
    private $m_PatientId = '';
    private $m_LastError = '';
    private $m_DescriptiveMetadataFilePaths = [];

    public function __construct($patientId)
    {
        $this->m_PatientId = $patientId;
    }

    public function addFile($sourcePath, $destinationPath)
    {
        $item = [];
        $item['source'] = $sourcePath;
        $item['destination'] = $destinationPath;
        array_push($this->m_DataFilePaths, $item);
    }

    public function files()
    {
        return $this->m_DataFilePaths;
    }

    public function produceSip($outBaseDirectory)
    {
        $patientBasePath = "{$outBaseDirectory}/Patientrecord_{$this->m_PatientId}";

        // Create base directory
        if (!mkdir($patientBasePath))
        {
            $this->setError("Failed to create directory: {$patientBasePath}");
            return false;
        }

        // Generate metadata
        $metadataPath = "{$patientBasePath}/metadata";
        if (!mkdir($metadataPath))
        {
            $this->setError("Failed to create directory: {$metadataPath}");
            return false;
        }
        if (!$this->generateMetadata($metadataPath))
        {
            $this->setError("Failed to generate metadata");
            return false;
        }

        // Generate data
        $dataPath = "{$patientBasePath}/data";
        if (!mkdir($dataPath))
        {
            $this->setError("Failed to create directory: {$dataPath}");
            return false;
        }
        if (!$this->generateData($dataPath))
        {
            $this->setError("Failed to generate data");
            return false;
        }

        return true;
    }

    public function error()
    {
        return $this->m_LastError;
    }

    public function patientId()
    {
        return $this->m_PatientId;
    }

    public function descriptiveMetadata()
    {
        return $this->m_DescriptiveMetadataFilePaths;
    }

    public function addDescriptiveMetadata($filePath)
    {
        array_push($this->m_DescriptiveMetadataFilePaths, $filePath);
    }

    public function isDescriptiveMetadata($path)
    {
        // E.g. Fhir_Condition_{ID}.xml

        return strtolower(pathinfo($path, PATHINFO_EXTENSION)) == 'xml';
    }

    private function setError($errorText)
    {
        $this->m_LastError = $errorText;
    }

    private function generateMetadata($outputDirectory)
    {
        // Generate descriptive metadata
        $descriptiveMetadataPath = "{$outputDirectory}/descriptive";
        if (!mkdir($descriptiveMetadataPath))
        {
            return false;
        }
        if (!$this->generateDescriptiveMetadata($descriptiveMetadataPath))
        {
            return false;
        }

        // Generate preservation metadata
        $preservationMetadataPath = "{$outputDirectory}/preservation";
        if (!mkdir($preservationMetadataPath))
        {
            return false;
        }
        if (!$this->generatePreservationMetadata($preservationMetadataPath))
        {
            return false;
        }

        return true;
    }

    private function generateDescriptiveMetadata($outputDirectory)
    {
        foreach($this->m_DescriptiveMetadataFilePaths as $filePath)
        {
            $fileName = basename($filePath);
            $destination = "{$outputDirectory}/{$fileName}";
            if (!copy($filePath, $destination))
            {
                return false;
            }
        }

        return true;
    }

    private function generatePreservationMetadata($outputDirectory)
    {
        // TODO
        return true;
    }

    private function generateData($outputDirectory)
    {
        foreach ($this->m_DataFilePaths as $path)
        {
            $source = $path['source'];
            $destination = "{$outputDirectory}/{$path['destination']}";

            if (!file_exists($source))
            {
                return false;
            }

            if (is_dir($source))
            {
                if (!mkdir($destination))
                {
                    return false;
                }
            }
            else
            {
                // Create directory if needed
                if (!is_dir(dirname($destination)) && !mkdir(dirname($destination), 0777, true))
                {
                    return false;
                }

                // Copy file
                if (!copy($source, $destination))
                {
                    return false;
                }
            }
        }

        return true;
    }
}

class Ehealth1Sip
{
    private $m_Patients = [];
    private $m_SubmissionAgreementFilePaths = [];
    private $m_DescriptiveMetadataFilePaths = [];
    private $m_SchemaFilePaths = [];
    private $m_InformationPackageId = '';
    private $m_PackageMetadata = [];
    private $m_LastError = '';

    public function __construct($informationPackageId='')
    {
        $this->m_InformationPackageId = $informationPackageId;
    }

    public function informationPackageId()
    {
        return $this->m_InformationPackageId;
    }

    public function setInformationPackageId($id)
    {
        $this->m_InformationPackageId = $id;
    }

    public function addPackageMetadata($key, $value)
    {
        $metadata = [];
        $metadata['key'] = $key;
        $metadata['value'] = $value;
        array_push($this->m_PackageMetadata, $metadata);
    }

    public function addPatient($patient)
    {
        array_push($this->m_Patients, $patient);
    }

    public function isPatientDirectory($path)
    {
        $expectStart = "patientrecord";
        if (substr(basename($path), 0, strlen($expectStart)) != $expectStart)
        {
            return false;
        }

        return true;
    }

    public function submissionAgreements()
    {
        return $this->m_SubmissionAgreementFilePaths;
    }

    public function addSubmissionAgreement($filePath)
    {
        array_push($this->m_SubmissionAgreementFilePaths, $filePath);
    }

    public function descriptiveMetadata()
    {
        return $this->m_DescriptiveMetadataFilePaths;
    }

    public function addDescriptiveMetadata($filePath)
    {
        array_push($this->m_DescriptiveMetadataFilePaths, $filePath);
    }

    public function schemaFiles()
    {
        return $this->m_SchemaFilePaths;
    }

    public function addSchemaFile($filePath)
    {
        array_push($this->m_SchemaFilePaths, $filePath);
    }

    public function produceSip(&$outPath, $outBaseDirectory)
    {
        // Clear output path
        $outPath = '';

        // Check that output directory exists
        if (!file_exists($outBaseDirectory) || !is_dir($outBaseDirectory))
        {
            $this->setError("Output directory doesn't exists: {$outBaseDirectory}");
            return false;
        }

        // Define base output path for SIP - full/output/dir/<packageID>
        $sipBasePath = rtrim($outBaseDirectory, '/') . '/';
        $sipBasePath .= $this->m_InformationPackageId;

        // Create SIP root directory
        if (!mkdir($sipBasePath))
        {
            $this->setError("Failed to create directory: {$sipBasePath}");
            return false;
        }

        // Generate package metadata
        if (!$this->generatePackageMetadata($sipBasePath))
        {
            $this->setError("Failed to generate package metadata");
            return false;
        }

        // Generate schemas
        $schemasPath = "{$sipBasePath}/schemas";
        if (!mkdir($schemasPath))
        {
            $this->setError("Failed to create directory: {$schemasPath}");
            return false;
        }
        if (!$this->generateSchemas($schemasPath))
        {
            $this->setError("Failed to generate schemas");
            return false;
        }

        // Generate metadata
        $metadataPath = "{$sipBasePath}/metadata";
        if (!mkdir($metadataPath))
        {
            $this->setError("Failed to create directory: {$metadataPath}");
            return false;
        }
        if (!$this->generateMetadata($metadataPath))
        {
            $this->setError("Failed to generate metadata");
            return false;
        }

        // Generate documentation
        $documentationPath = "{$sipBasePath}/documentation";
        if (!mkdir($documentationPath))
        {
            $this->setError("Failed to create directory: {$documentationPath}");
            return false;
        }
        if (!$this->generateDocumentation($documentationPath))
        {
            $this->setError("Failed to generate documentation");
            return false;
        }

        // Generate representations
        $representationsPath = "{$sipBasePath}/representations";
        if (!mkdir($representationsPath))
        {
            $this->setError("Failed to create directory: {$representationsPath}");
            return false;
        }
        foreach($this->m_Patients as $patient)
        {
            if (!$patient->produceSip($representationsPath))
            {
                $patientError = $patient->error();
                $patientId = $patient->patientId();
                $this->setError("Failed to produce patient data for patient {$patientId}: {$patientError}");
                return false;
            }
        }

        // Generate METS
        if (!$this->generateMets($sipBasePath))
        {
            return false;
        }

        // Cleanup temp files
        // TODO: Metadata file should be created as a temp file outside of SIP directory
        //       and passed to METS generator as input
        $metadataFilePath = "{$sipBasePath}/metadata.xml";
        if (file_exists($metadataFilePath) && !unlink($metadataFilePath))
        {
            $this->setError("Failed to delete metadata file: {$metadataFilePath}");
        }

        // Set output path
        $outPath = $sipBasePath;

        return true;
    }

    public function error()
    {
        return $this->m_LastError;
    }

    public function isSubmissionAgreement($path)
    {
        // Expects filename like submissionagreement.{xml|pdf|jpg|jpeg}
        $expectedBaseName = "submissionagreement";
        $allowedExtensions = ['xml', 'pdf', 'jpg', 'jpeg'];

        if (substr(pathinfo($path, PATHINFO_FILENAME), 0, strlen($expectedBaseName)) != $expectedBaseName)
        {
            return false;
        }
        if (!in_array(strtolower(pathinfo($path, PATHINFO_EXTENSION)), $allowedExtensions))
        {
            return false;
        }

        return true;
    }

    public function isDescriptiveMetadata($path)
    {
        if (basename($path) == 'Patients.xml')
        {
            return true;
        }

        return false;
    }

    private function generateMets($sipPath)
    {
        $runner = new PythonRunner();

        $scriptPath = __DIR__ . '/../thirdparty/metsgen/genrepmets.py';
        $arguments = [$sipPath];
        if (!$runner->executeScript($scriptPath, $arguments))
        {
            $this->setError('Failed to generate patient METS: ' . $runner->error());
            return false;
        }

        $scriptPath = __DIR__ . '/../thirdparty/metsgen/genrootmets.py ';
        $arguments = [$sipPath];
        if (!$runner->executeScript($scriptPath, $arguments))
        {
            $this->setError('Failed to generate root METS: ' . $runner->error());
            return false;
        }

        return true;
    }

    private function generatePackageMetadata($outputDirectory)
    {
        // Do not generate file if no metadata is added
        if (count($this->m_PackageMetadata) == 0)
        {
            return true;
        }

        $metadata = "";
        $metadata .= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        $metadata .= "<systemData>\n";
        foreach ($this->m_PackageMetadata as $md)
        {
            $key = $md["key"];
            $value = $md["value"];
            $metadata .= "    <{$key}>\n";
            $metadata .= "        <![CDATA[{$value}]]>\n";
            $metadata .= "    </{$key}>\n";
        }
        $metadata .= "</systemData>\n";

        $filePath = "{$outputDirectory}/metadata.xml";
        return file_put_contents($filePath, $metadata, LOCK_EX) !== false;
    }

    private function generateSchemas($outputDirectory)
    {
        foreach($this->m_SchemaFilePaths as $filePath)
        {
            $fileName = basename($filePath);
            $destination = "{$outputDirectory}/{$fileName}";
            if (!copy($filePath, $destination))
            {
                return false;
            }
        }

        return true;
    }

    private function generateMetadata($outputDirectory)
    {
        // Generate descriptive metadata
        $descriptiveMetadataPath = "{$outputDirectory}/descriptive";
        if (!mkdir($descriptiveMetadataPath))
        {
            return false;
        }
        if (!$this->generateDescriptiveMetadata($descriptiveMetadataPath))
        {
            return false;
        }

        // Generate preservation metadata
        $preservationMetadataPath = "{$outputDirectory}/preservation";
        if (!mkdir($preservationMetadataPath))
        {
            return false;
        }
        if (!$this->generatePreservationMetadata($preservationMetadataPath))
        {
            return false;
        }

        return true;
    }

    private function generateDescriptiveMetadata($outputDirectory)
    {
        foreach($this->m_DescriptiveMetadataFilePaths as $filePath)
        {
            $fileName = basename($filePath);
            $destination = "{$outputDirectory}/{$fileName}";
            if (!copy($filePath, $destination))
            {
                return false;
            }
        }

        return true;
    }

    private function generatePreservationMetadata($outputDirectory)
    {
        // TODO
        return true;
    }

    private function generateDocumentation($outputDirectory)
    {
        foreach($this->m_SubmissionAgreementFilePaths as $filePath)
        {
            $fileName = basename($filePath);
            $destination = "{$outputDirectory}/{$fileName}";
            if (!copy($filePath, $destination))
            {
                return false;
            }
        }

        return true;
    }

    private function setError($errorText)
    {
        $this->m_LastError = $errorText;
    }
}
?>
