<?php
class Ehealth1SipPatient
{
    private $m_SourceDataFilePaths = [];
    private $m_DestinationDataFilePaths = [];
    private $m_PatientId = '';
    private $m_LastError = '';

    public function __construct($patientId)
    {
        $this->m_PatientId = $patientId;
    }

    public function addFile($sourcePath, $destinationPath)
    {
        array_push($this->m_SourceDataFilePaths, $sourcePath);
        array_push($this->m_DestinationDataFilePaths, $destinationPath);
    }

    public function produceSip($outBaseDirectory)
    {
        $patientBasePath = "{$outBaseDirectory}/{$this->m_PatientId}";

        // Generate METS
        if (!$this->generateMets($patientBasePath))
        {
            $this->setError("Failed to generate METS file");
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

    private function setError($errorText)
    {
        $this->m_LastError = $errorText;
    }

    private function generateMets($outputDirectory)
    {
        // TODO
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
        // TODO
        return true;
    }

    private function generatePreservationMetadata($outputDirectory)
    {
        // TODO
        return true;
    }
}

class Ehealth1Sip
{
    private $m_Patients = [];
    private $m_SubmissionAgreementFilePaths = [];
    private $m_InformationPackageId = '';
    private $m_LastError = '';

    public function __construct($informationPackageId)
    {
        $this->m_InformationPackageId = $informationPackageId;
    }

    public function addPatient($patient)
    {
        array_push($this->m_Patients, $patient);
    }

    public function addSubmissionAgreement($filePath)
    {
        array_push($this->m_SubmissionAgreementFilePaths, $filePath);
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
        $sipBasePath = rtrim($outBaseDirectory,'/').'/';
        $sipBasePath .= $this->m_InformationPackageId;

        // Create SIP root directory
        if (!mkdir($sipBasePath))
        {
            $this->setError("Failed to create directory: {$sipBasePath}");
            return false;
        }

        // Generate METS
        if (!$this->generateMets($sipBasePath))
        {
            $this->setError("Failed to generate METS file");
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
        $doctumentationPath = "{$sipBasePath}/documentation";
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

        // Set output path
        $outPath = $sipBasePath;

        return true;
    }

    public function error()
    {
        return $this->m_LastError;
    }

    private function generateMets($outputDirectory)
    {
        // TODO
        return true;
    }

    private function generateSchemas($outputDirectory)
    {
        // TODO
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
        // TODO
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
