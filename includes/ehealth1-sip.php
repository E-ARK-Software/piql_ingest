<?php
class Ehealth1SipPatient
{
    private $m_DataFilePaths = [];
    private $m_PatientId = '';

    public function __construct($patientId)
    {
        $this->m_PatientId = $patientId;
    }

    public function addFile($path)
    {
        array_push($this->m_DataFilePaths, $path);
    }
}

class Ehealth1Sip
{
    private $m_Patients = [];
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

        // Generate METS file
        if (!generateMets($sipBasePath))
        {
            $this->setError("Failed to generate METS file");
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

    private function setError($errorText)
    {
        $this->m_LastError = $errorText;
    }
}
?>
